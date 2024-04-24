# This program was used to find the maximum/minimum possible Perlin samples in Minecraft Java/Bedrock 1.18+ (+-1.0363538112118025...),
#   and the configurations capable of generating them.
# This can also be used to list the maximum/minimum sample generatable by any arbitrary set of indexedLerp indices.
# 
# Each entry outputted by this program lists:
#   - the maximum/minimum sample possible given the listed indices.
#   - the fractional portions of the a, b, and c values that would produce that maximum/minimum sample.
#   - the current indices that would be provided to indexedLerp. Normally those would be determined by the integer portions of
#       a + px / b / c + pz alongside the Perlin octave's d-array, all modulo 16, but by only concerning ourselves with the final 8
#       values that get selected (and removing cases 12-15 which are duplicates of other cases), we can categorize
#       all possible Perlin octaves into 12^8 = 429981696 possibilities, which drops to 3^8 = 6561 if we optimize for
#       finding the global maximum/minimum.


# The mode to search with ("max" or "min", case insensitive).
#   "max"/"min" with GLOBAL = True finds the maximum/minimum Perlin samples across all index combinations.
#   "max"/"min" with GLOBAL = False finds the maximum/minimum Perlin samples for each individual index combination.
MODE = "max"
GLOBAL = True

"""Only relevant if GLOBAL = True:"""
# The initial threshold to begin with. Samples less than this (if "max") or greater than this (if "min") will be ignored.
threshold = 0
# The maximum difference a local min/max sample can have from the global min/max sample and still be printed,
#   to account for potential floating-point errors.
EPSILON = 0.00001

# ---------------------------------------------------------------------------------
from itertools import product
from ....Pybiomes import lerp, indexedLerp

# Standardizes MODE as lowercase
MODE = MODE.lower()

def isBetter(old: float, new: float, epsilon: float = 0) -> bool:
    """Compares `old` and `new` values based on `MODE` and returns if `new` is "better" or not.
    Also accepts an optional parameter `epsilon` that is factored into `new` to account for previous floating-point errors.

    Equality does not count as being better than the previous value, to prevent code later on from getting stuck
    iterating back and forth over plateaus, which for extremely miniscule values of displacement would not finish
    on a human timescale."""
    match MODE:
        case "max": return old < new + epsilon
        case "min": return old > new - epsilon
        case _: raise ValueError('MODE must be "max" or "min" (case-insensitive).')

# "max"/"min" contain the indices that will return the most positive/negative samples if GLOBAL is true.
#   (For example, for +a, +b, and +c (which is case 0), the most positive samples will occur with a + b, a + c, or b + c,
#    which are indices 0, 4, and 8.
#    The two lists are direct reversals of one another, which is not surprising if one looks at the logic.)
# Otherwise if GLOBAL is false, the entries contain all indices.
IDEAL_INDICES: dict[str, tuple] = {
    #       +a,+b,+c  -a,+b,+c  +a,-b,+c  -a,-b,+c  +a,+b,-c  -a,+b,-c  +a,-b,-c  -a,-b,-c
    "max": ((0,4,8) , (1,5,8) , (2,4,9) , (3,5,9) , (0,6,10), (1,7,10), (2,6,11), (3,7,11)) if GLOBAL else (range(12),)*8,
    "min": ((3,7,11), (2,6,11), (1,7,10), (0,6,10), (3,5,9) , (2,4,9) , (1,5,8) , (0,4,8) ) if GLOBAL else (range(12),)*8,
}

# Lists of indices in the original indexedLerp function that were duplicates of an index in the reduced indexedLerp function,
#   with the latter being each list's sort key.
INDEXED_LERP_DUPLICATE_CASES: dict[int, list[int]] = {
    0:  [0, 12],
    1:  [1, 14],
    9:  [9, 13],
    11: [11, 15]
}

def samplePerlin(a: float, b: float, c: float, i1: int, i2: int, i3: int, i4: int, i5: int, i6: int, i7: int, i8: int) -> float:
    """Modified copy of the samplePerlin function from Cubiomes.
    
    It accepts floats a, b, and c in the range [0,1), and integer indices i1, ..., i8 in the range [0,12)
    to use for the indexed lerps. In reality the function derives i1, ..., i8 from the integer parts of a + px/b/c + pz
    and the Perlin octave's d-array, but this way we can model its behavior if a particular set of indices
    were to be chosen by that."""
    t1 = a * a * a * (a * (a * 6 - 15) + 10)
    l1 = lerp(indexedLerp(i1, a, b, c), indexedLerp(i2, a-1, b, c), t1)
    l3 = lerp(indexedLerp(i3, a, b-1, c), indexedLerp(i4, a-1, b-1, c), t1)
    l5 = lerp(indexedLerp(i5, a, b, c-1), indexedLerp(i6, a-1, b, c-1), t1)
    l7 = lerp(indexedLerp(i7, a, b-1, c-1), indexedLerp(i8, a-1, b-1, c-1), t1)
    t2 = b * b * b * (b * (b * 6 - 15) + 10)
    l1 = lerp(l1, l3, t2)
    l5 = lerp(l5, l7, t2)
    return lerp(l1, l5, c * c * c * (c * (c * 6 - 15) + 10))

# Iterates over all indices for the specified mode.
#   (Syntax from https://stackoverflow.com/a/16384126 and https://stackoverflow.com/a/36908)
for i1, i2, i3, i4, i5, i6, i7, i8 in product(*IDEAL_INDICES[MODE]):
    # Begin with an initial state of (a, b, c) = (0.5, 0.5, 0.5) (the midpoint of all three ranges)
    state = [0.5]*3
    # Take the sample there to have a benchmark for the current combination
    currentBestSample = samplePerlin(state[0], state[1], state[2], i1, i2, i3, i4, i5, i6, i7, i8)
    # This specifies how much displacement each value will undergo at a time on each iteration.
    #   It will be halved after every iteration, allowing us to gradually converge
    #   on the optimal a/b/c values.
    # (0.5 was chosen because it is half of the range of the a/b/c values.)
    displacement = 0.5
    # While said displacement is nonzero:
    while displacement:
        # TODO: Remove repetition?
        # + a finetuning
        while True:
            # Moves the a-value right by displacement, breaking if that goes out of bounds
            stateWithDisplacement = state[0] + displacement
            if stateWithDisplacement >= 1: break
            # Otherwise, take the sample
            sample = samplePerlin(stateWithDisplacement, state[1], state[2], i1, i2, i3, i4, i5, i6, i7, i8)
            # If that sample is not better than the best previous, stop.
            #   If any additional fine-tuning needs to be done, it can be done on a later iteration.
            if not isBetter(currentBestSample, sample): break
            # Otherwise the new sample was better than the previous, in which case we update the a/b/c state and the best sample.
            currentBestSample = sample
            state[0] = stateWithDisplacement
        # - a finetuning using the same logic
        while True:
            stateWithDisplacement = state[0] - displacement
            if stateWithDisplacement < 0: break
            sample = samplePerlin(stateWithDisplacement, state[1], state[2], i1, i2, i3, i4, i5, i6, i7, i8)
            if not isBetter(currentBestSample, sample): break
            currentBestSample = sample
            state[0] = stateWithDisplacement
        # + b finetuning using the same logic
        while True: 
            stateWithDisplacement = state[1] + displacement
            if stateWithDisplacement >= 1: break
            sample = samplePerlin(state[0], stateWithDisplacement, state[2], i1, i2, i3, i4, i5, i6, i7, i8)
            if not isBetter(currentBestSample, sample): break
            currentBestSample = sample
            state[1] = stateWithDisplacement
        # - b finetuning using the same logic
        while True:
            stateWithDisplacement = state[1] - displacement
            if stateWithDisplacement < 0: break
            sample = samplePerlin(state[0], stateWithDisplacement, state[2], i1, i2, i3, i4, i5, i6, i7, i8)
            if not isBetter(currentBestSample, sample): break
            currentBestSample = sample
            state[1] = stateWithDisplacement
        # + c finetuning using the same logic
        while True:
            stateWithDisplacement = state[2] + displacement
            if stateWithDisplacement >= 1: break
            sample = samplePerlin(state[0], state[1], stateWithDisplacement, i1, i2, i3, i4, i5, i6, i7, i8)
            if not isBetter(currentBestSample, sample): break
            currentBestSample = sample
            state[2] = stateWithDisplacement
        # - c finetuning using the same logic
        while True:
            stateWithDisplacement = state[2] - displacement
            if stateWithDisplacement < 0: break
            sample = samplePerlin(state[0], state[1], stateWithDisplacement, i1, i2, i3, i4, i5, i6, i7, i8)
            if not isBetter(currentBestSample, sample): break
            currentBestSample = sample
            state[2] = stateWithDisplacement
        # Then halves the value of displacement for the next iteration.
        displacement /= 2
    # Then, once all of that finishes, if we're only concerned with the local minimums/maximums,
    #   or if the current best sample is within EPSILON of threshold or better, print the attributes.
    if not GLOBAL or isBetter(threshold, currentBestSample, EPSILON):
        # Some cases in the original indexedLerp are duplicates: if any of the indices being printed are,
        #   print an array of the index's duplicates instead of the index itself.
        # TODO: Find a cleaner way of doing this
        print(f"{currentBestSample} via {state}\t{INDEXED_LERP_DUPLICATE_CASES[i1] if i1 in INDEXED_LERP_DUPLICATE_CASES else i1} {INDEXED_LERP_DUPLICATE_CASES[i2] if i2 in INDEXED_LERP_DUPLICATE_CASES else i2} {INDEXED_LERP_DUPLICATE_CASES[i3] if i3 in INDEXED_LERP_DUPLICATE_CASES else i3} {INDEXED_LERP_DUPLICATE_CASES[i4] if i4 in INDEXED_LERP_DUPLICATE_CASES else i4} {INDEXED_LERP_DUPLICATE_CASES[i5] if i5 in INDEXED_LERP_DUPLICATE_CASES else i5} {INDEXED_LERP_DUPLICATE_CASES[i6] if i6 in INDEXED_LERP_DUPLICATE_CASES else i6} {INDEXED_LERP_DUPLICATE_CASES[i7] if i7 in INDEXED_LERP_DUPLICATE_CASES else i7} {INDEXED_LERP_DUPLICATE_CASES[i8] if i8 in INDEXED_LERP_DUPLICATE_CASES else i8}", flush=True)
        # Then if we're concerned with the global minimums/maximums, and the current best sample actually is better than
        #   the threshold, update the threshold.
        if GLOBAL and isBetter(threshold, currentBestSample): threshold = currentBestSample