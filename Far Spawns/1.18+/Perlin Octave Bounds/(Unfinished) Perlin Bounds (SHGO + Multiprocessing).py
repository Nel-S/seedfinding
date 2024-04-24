# This program finds the maximum/minimum sample generatable by a Perlin octave with an arbitrary set of indexedLerp indices.
# 
# Each entry outputted by this program lists:
#   - the maximum/minimum sample possible given the listed indices.
#   - the fractional portions of the a, b, and c values that would produce that maximum/minimum sample.
#   - the current indices that would be provided to indexedLerp. Normally those would be determined by the integer portions of
#       a + px / b / c + pz alongside the Perlin octave's d-array, all modulo 16, but by only concerning ourselves with the final 8
#       values that get selected (and removing cases 12-15 which are duplicates of other cases), we can categorize
#       all possible Perlin octaves into 12^8 = 429981696 possibilities, which drops to 3^8 = 6561 if we optimize for
#       finding the global maximum/minimum.

from os import sep
from os.path import join

# The mode to search with ("max" or "min", case insensitive).
#   "max"/"min" with IDEAL_INDICES_ONLY = True finds the maximum/minimum Perlin samples for the index combinations most ideal for maximum/minimum samples.
#   "max"/"min" with IDEAL_INDICES_ONLY = False finds the maximum/minimum Perlin samples for all index combinations.
MODE = "min"
IDEAL_INDICES_ONLY = False
NUMBER_OF_PROCESSES = 4

# The initial threshold to begin with. Samples less than this (if "max") or greater than this (if "min") will be ignored.
# threshold = -0.537489692875767
INIT_THRESHOLD = float('inf')
# 
UPDATE_THRESHOLD = False
# The maximum difference a local min/max sample can have from the threshold and still be printed,
#   to account for potential floating-point errors.

FILEPATH = join("C:" + sep, "perlinMins.txt")
SHGO_EPSILON = 0.001
HIGHER_PRECISION_MAX_ITERATIONS = 10000
HIGHER_PRECISION_EPSILON = 0

# ---------------------------------------------------------------------------------
# from itertools import product
from multiprocessing import Lock, Pool, Value
from numpy import format_float_positional, ndarray
from scipy.optimize import shgo
from ....Pybiomes import lerp, indexedLerp

# Standardizes MODE as lowercase
MODE = MODE.lower()
threshold = Value('d', lock=True)
filelock = Lock()
file = open(FILEPATH, "w")
threshold.value = INIT_THRESHOLD

def isBetter(old: float, new: float, epsilon: float = 0) -> bool:
    """Compares `old` and `new` based on `MODE` and returns whether `new` is "better" or not.
    Also accepts an optional parameter `epsilon` that is factored into `new` to account for previous floating-point errors."""
    match MODE:
        case "max": return old < new + epsilon
        case "min": return old > new - epsilon
        case _: raise ValueError('isBetter: MODE must be "max" or "min" (case-insensitive).')

# Defines bounds of [0, 1) for all three dimensions for SciPy algorithm
BOUNDS = [(0, 1-1/2**53)]*3

# "max"/"min" contain the indices that will return the most positive/negative samples if IDEAL_INDICES_ONLY is true.
#   (For example, for +a, +b, and +c (which is case 0), the most positive samples will occur with a + b, a + c, or b + c,
#    which are indices 0, 4, and 8.
#    The two lists are direct reversals of one another, which is not surprising if one looks at the logic.)
# Otherwise if IDEAL_INDICES_ONLY is false, the entries contain all indices.
INDICES: dict[str, tuple] = {
    #       +a,+b,+c  -a,+b,+c  +a,-b,+c  -a,-b,+c  +a,+b,-c  -a,+b,-c  +a,-b,-c  -a,-b,-c
    "max": ((0,4,8) , (1,5,8) , (2,4,9) , (3,5,9) , (0,6,10), (1,7,10), (2,6,11), (3,7,11)) if IDEAL_INDICES_ONLY else (range(12),)*8,
    "min": ((3,7,11), (2,6,11), (1,7,10), (0,6,10), (3,5,9) , (2,4,9) , (1,5,8) , (0,4,8) ) if IDEAL_INDICES_ONLY else (range(12),)*8,
}

# Lists of indices in the original indexedLerp function that were duplicates of an index in the reduced indexedLerp function,
#   with the latter being each list's sort key.
INDEXED_LERP_DUPLICATE_CASES: dict[int, list[int]] = {
    0:  [0, 12],
    1:  [1, 14],
    9:  [9, 13],
    11: [11, 15]
}

def samplePerlin(x: ndarray | list[float], i1: int, i2: int, i3: int, i4: int, i5: int, i6: int, i7: int, i8: int) -> float:
    """Modified copy of the samplePerlin function from Cubiomes.
    
    It accepts a list `x` of three floats in the range [0,1), and a tuple `Is` of eight integer indices in the range [0,12)
    to use for the indexed lerps. In reality the function derives the indices from the integer parts of a + px/b/c + pz
    and the Perlin octave's d-array, but this way we can model its behavior if a particular set of indices
    were to have been chosen by that."""
    t1 = x[0]**3 * (x[0] * (x[0] * 6 - 15) + 10)
    l1 = lerp(indexedLerp(i1, x[0], x[1], x[2]), indexedLerp(i2, x[0]-1, x[1], x[2]), t1)
    l3 = lerp(indexedLerp(i3, x[0], x[1]-1, x[2]), indexedLerp(i4, x[0]-1, x[1]-1, x[2]), t1)
    l5 = lerp(indexedLerp(i5, x[0], x[1], x[2]-1), indexedLerp(i6, x[0]-1, x[1], x[2]-1), t1)
    l7 = lerp(indexedLerp(i7, x[0], x[1]-1, x[2]-1), indexedLerp(i8, x[0]-1, x[1]-1, x[2]-1), t1)
    t2 = x[1]**3 * (x[1] * (x[1] * 6 - 15) + 10)
    l1 = lerp(l1, l3, t2)
    l5 = lerp(l5, l7, t2)
    return lerp(l1, l5, x[2]**3 * (x[2] * (x[2] * 6 - 15) + 10))

def writePerlinMin(config: int) -> None:
    global file, filelock
    configCopy = config
    Is = []
    for _ in range(8):
        Is.append(configCopy % 12)
        configCopy //= 12
    i8, i7, i6, i5, i4, i3, i2, i1 = Is
    # First uses a SciPy global optimaztion algorithm to quickly find the minimum of the function.
    result = shgo(samplePerlin, BOUNDS, args=(i1, i2, i3, i4, i5, i6, i7, i8))
    # result = basinhopping(samplePerlin, [0.5]*3, stepsize=0.5, minimizer_kwargs={'args': (i1, i2, i3, i4, i5, i6, i7, i8)})
    # If that failed, print error message and continue to next case
    # if not result['success']:
    #     print(f"\tCASE {i1} {i2} {i3} {i4} {i5} {i6} {i7} {i8} failed: {result['message']}", flush=True)
    #     continue
    state = list(result['x'])
    # currentBestSample = samplePerlin(state, i1, i2, i3, i4, i5, i6, i7, i8)
    currentBestSample = result['fun']
    # if not isBetter(threshold.value, currentBestSample, SHGO_EPSILON): return

    # Otherwise then manually converges using the values in that local area to achieve better precision
    # BUG: Extremely slow with certain octave configurations
    # TODO: Could be replaced with Newton's Method if I don't mind manually solving 1152 first + second derivatives and probably losing a piece of my sanity...
    if HIGHER_PRECISION_MAX_ITERATIONS:
        # This specifies how much displacement each value will undergo at a time on each iteration.
        #   It will be halved after every iteration, allowing us to gradually converge
        #   on the optimal a/b/c values.
        displacement = 0.000005
        # While said displacement is nonzero:
        while displacement:
            # TODO: Remove repetition?
            # + a finetuning
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                # Moves the a-value right by displacement, breaking if that goes out of bounds
                stateWithDisplacement = state[0] + displacement
                if stateWithDisplacement >= 1: break
                # Otherwise, take the sample
                sample = samplePerlin([stateWithDisplacement, state[1], state[2]], i1, i2, i3, i4, i5, i6, i7, i8)
                # If that sample is not better than the best previous, stop.
                #   If any additional fine-tuning needs to be done, it can be done on a later iteration.
                if not isBetter(currentBestSample, sample): break
                # Otherwise the new sample was better than the previous, in which case we update the a/b/c state and the best sample.
                currentBestSample = sample
                state[0] = stateWithDisplacement
            # - a finetuning using the same logic
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                stateWithDisplacement = state[0] - displacement
                if stateWithDisplacement < 0: break
                sample = samplePerlin([stateWithDisplacement, state[1], state[2]], i1, i2, i3, i4, i5, i6, i7, i8)
                if not isBetter(currentBestSample, sample): break
                currentBestSample = sample
                state[0] = stateWithDisplacement
            # + b finetuning using the same logic
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                stateWithDisplacement = state[1] + displacement
                if stateWithDisplacement >= 1: break
                sample = samplePerlin([state[0], stateWithDisplacement, state[2]], i1, i2, i3, i4, i5, i6, i7, i8)
                if not isBetter(currentBestSample, sample): break
                currentBestSample = sample
                state[1] = stateWithDisplacement
            # - b finetuning using the same logic
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                stateWithDisplacement = state[1] - displacement
                if stateWithDisplacement < 0: break
                sample = samplePerlin([state[0], stateWithDisplacement, state[2]], i1, i2, i3, i4, i5, i6, i7, i8)
                if not isBetter(currentBestSample, sample): break
                currentBestSample = sample
                state[1] = stateWithDisplacement
            # + c finetuning using the same logic
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                stateWithDisplacement = state[2] + displacement
                if stateWithDisplacement >= 1: break
                sample = samplePerlin([state[0], state[1], stateWithDisplacement], i1, i2, i3, i4, i5, i6, i7, i8)
                if not isBetter(currentBestSample, sample): break
                currentBestSample = sample
                state[2] = stateWithDisplacement
            # - c finetuning using the same logic
            for _ in range(HIGHER_PRECISION_MAX_ITERATIONS):
                stateWithDisplacement = state[2] - displacement
                if stateWithDisplacement < 0: break
                sample = samplePerlin([state[0], state[1], stateWithDisplacement], i1, i2, i3, i4, i5, i6, i7, i8)
                if not isBetter(currentBestSample, sample): break
                currentBestSample = sample
                state[2] = stateWithDisplacement
            # Then halves the value of displacement for the next iteration.
            displacement /= 2
        # If the current sample is better than the specified threshold:
        # if not isBetter(threshold.value, currentBestSample, HIGHER_PRECISION_EPSILON): return
    # Print the values
    # print(f"{currentBestSample} via {state}\t{INDEXED_LERP_DUPLICATE_CASES[i1] if i1 in INDEXED_LERP_DUPLICATE_CASES else i1} {INDEXED_LERP_DUPLICATE_CASES[i2] if i2 in INDEXED_LERP_DUPLICATE_CASES else i2} {INDEXED_LERP_DUPLICATE_CASES[i3] if i3 in INDEXED_LERP_DUPLICATE_CASES else i3} {INDEXED_LERP_DUPLICATE_CASES[i4] if i4 in INDEXED_LERP_DUPLICATE_CASES else i4} {INDEXED_LERP_DUPLICATE_CASES[i5] if i5 in INDEXED_LERP_DUPLICATE_CASES else i5} {INDEXED_LERP_DUPLICATE_CASES[i6] if i6 in INDEXED_LERP_DUPLICATE_CASES else i6} {INDEXED_LERP_DUPLICATE_CASES[i7] if i7 in INDEXED_LERP_DUPLICATE_CASES else i7} {INDEXED_LERP_DUPLICATE_CASES[i8] if i8 in INDEXED_LERP_DUPLICATE_CASES else i8}", flush=True)
    # print(f"{currentBestSample}\t{state}\t{i1} {i2} {i3} {i4} {i5} {i6} {i7} {i8}", flush=True)
    # print(f"{config}\t{format_float_positional(currentBestSample, trim='-')}\t{format_float_positional(state[0], trim='-')}\t{format_float_positional(state[1], trim='-')}\t{format_float_positional(state[2], trim='-')}", flush=True)
    # filelock.acquire()
    # try:
    #     file.write(f"{config}\t{format_float_positional(currentBestSample, trim='-')}\t{format_float_positional(state[0], trim='-')}\t{format_float_positional(state[1], trim='-')}\t{format_float_positional(state[2], trim='-')}\n")
    # finally:
    #     filelock.release()
    with filelock:
        print(f"\t{config}", flush=True)
        file.write(f"{config}\t{format_float_positional(currentBestSample, trim='-')}\t{format_float_positional(state[0], trim='-')}\t{format_float_positional(state[1], trim='-')}\t{format_float_positional(state[2], trim='-')}\n")
    # Then if we're concerned with the global minimums/maximums, and the current best sample actually is better than
    #   the threshold, update the threshold.
    if UPDATE_THRESHOLD and isBetter(threshold.value, currentBestSample):
        # NOTE: Think simple assignment is atomic, if not use with
        # with threshold.get_lock():
        threshold.value = currentBestSample

if __name__ == '__main__':
    with Pool(processes=NUMBER_OF_PROCESSES) as p:
        # Iterates over all indices for the specified mode.
        #   (Syntax from https://stackoverflow.com/a/16384126 and https://stackoverflow.com/a/36908)
        # for i1, i2, i3, i4, i5, i6, i7, i8 in product(*INDICES[MODE]):
        list(p.map(writePerlinMin, range(1000), chunksize=32))
    file.close()