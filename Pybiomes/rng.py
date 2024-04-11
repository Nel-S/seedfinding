from math import trunc
from multimethod import multimethod

# Bitlengths
FOUR_BITS                     = 0xf
EIGHT_BITS                    = 0xff
SIXTEEN_BITS                  = 0xffff
THIRTY_TWO_BITS               = 0xffffffff
FORTY_EIGHT_BITS              = 0xffffffffffff
SIXTY_FOUR_BITS               = 0xffffffffffffffff
ONE_HUNDRED_TWENTY_EIGHT_BITS = 0xffffffffffffffffffffffffffffffff
FLOAT_LENGTH                  = 0x1000000
DOUBLE_LENGTH                 = 0x20000000000000

# Might convert into classes for the different-width integers

def toSigned(n: int, *, width: int = 64) -> int:
    """Returns the signed `width`-bit integer equivalent of `n`. `width` must be positive.
    Logic courtesy of https://graphics.stanford.edu/~seander/bithacks.html#VariableSignExtend."""
    n &= (1 << width) - 1
    minInt = 1 << (width - 1)
    return (n ^ minInt) - minInt

def toUnsigned(n: int, *, width: int = 64) -> int:
    """Returns the unsigned `width`-bit integer equivalent of `n`. `width` must be positive."""
    return n & ((1 << width) - 1)

def rotl(n: int, bits: int, *, width: int = 64) -> int:
    """Returns the unsigned left circular rotation of `n`, as a `width`-bit integer, by `bits` bits.
    A right circular rotation can be done by calling this with a negative value for `bits` instead.
    `width` must be positive."""
    window = (1 << width) - 1
    n &= window
    bits %= width
    return ((n << bits) | (n >> (width - bits))) & window

# Copies for the fixed widths of 32 and 64 bits, to maybe speed those up?

# def toSigned32(n: int) -> int:
#     """Returns the signed 32-bit integer equivalent of `n`."""
#     n &= THIRTY_TWO_BITS
#     return (n ^ 0x80000000) - 0x80000000

# def toUnsigned32(n: int) -> int:
#     """Returns the unsigned 32-bit integer equivalent of `n`."""
#     return n & THIRTY_TWO_BITS

# def toSigned64(n: int) -> int:
#     """Returns the signed 64-bit integer equivalent of `n`."""
#     n &= SIXTY_FOUR_BITS
#     return (n ^ 0x8000000000000000) - 0x8000000000000000

# def toUnsigned64(n: int) -> int:
#     """Returns the unsigned 64-bit integer equivalent of `n`."""
#     return n & THIRTY_TWO_BITS

# def rotl32(n: int, bits: int) -> int:
#     """Returns the unsigned left circular rotation of `n`, as a 32-bit integer, by `bits` bits.
#     A right circular rotation can be done by calling this with a negative value for `bits` instead."""
#     n &= THIRTY_TWO_BITS
#     bits &= 31
#     return ((n << bits) | (n >> (32 - bits))) & THIRTY_TWO_BITS

# def rotl64(n: int, bits: int) -> int:
#     """Returns the unsigned left circular rotation of `n`, as a 64-bit integer, by `b` bits.
#     A right circular rotation can be done by calling this with a negative value for `bits` instead."""
#     n &= SIXTY_FOUR_BITS
#     bits &= 63
#     return ((n << bits) | (n >> (64 - bits))) & SIXTY_FOUR_BITS


def floordiv(a: int, b: int) -> int:
    return a//b

def lerp(lowerBound: float, upperBound: float, weight: float, *, clamp: bool = True) -> float:
    """Returns the result of a round of linear interpolation.
    If `weight` is in the range [0,1], or if `clamp` is true, the returned value will be in the range [`lowerBound`, `upperBound`]."""
    if clamp:
        if weight <= 0: return lowerBound
        if weight >= 1: return upperBound
    return lowerBound + weight*(upperBound - lowerBound)

def doubleLerp(lowerBound1: float, upperBound1: float, lowerBound2: float, upperBound2: float, weight1: float, weight2: float, *, clamp: bool = True) -> float:
    """Returns the result of two nested rounds of linear interpolation."""
    # TODO: Figure out range
    return lerp(lerp(lowerBound1, upperBound1, weight1, clamp=clamp), lerp(lowerBound2, upperBound2, weight1, clamp=clamp), weight2, clamp=clamp)

def tripleLerp(lowerBound1: float, upperBound1: float, lowerBound2: float, upperBound2: float, lowerBound3: float, upperBound3: float, lowerBound4: float, upperBound4: float, weight1: float, weight2: float, weight3: float, *, clamp: bool = True) -> float:
    """Returns the result of three nested rounds of linear interpolation."""
    # TODO: Figure out range
    return lerp(doubleLerp(lowerBound1, upperBound1, lowerBound2, upperBound2, weight1, weight2, clamp=clamp), doubleLerp(lowerBound3, upperBound3, lowerBound4, upperBound4, weight1, weight2, clamp=clamp), weight3, clamp=clamp)

def indexedLerp(index: int, a: float, b: float, c: float) -> float:
    """Returns two of the three values `a`, `b`, or `c` added to or subtracted from one another, depending on the value of `index`."""
    match index & FOUR_BITS:
        case 0 | 12 : return  a + b
        case 1 | 14 : return -a + b
        case 2      : return  a - b
        case 3      : return -a - b
        case 4      : return  a + c
        case 5      : return -a + c
        case 6      : return  a - c
        case 7      : return -a - c
        case 8      : return  b + c
        case 9 | 13 : return -b + c
        case 10     : return  b - c
        case 11 | 15: return -b - c
    return 0

def modularInverse(x: int, modulo: int, *, width=64) -> int:
    """Returns the modulo inverse `x`^-1 (mod `modulo`)--equivalently the value p such that `x`*p (mod `modulo`) = 1--or 0 if no such inverse exists."""
    if toSigned(modulo, width=width) <= 1: return 0
    originalModulo, a, b = modulo, 0, 1
    x = toUnsigned(x, width=width)
    while toSigned(x, width=width) > 1:
        if not modulo: return 0
        temp = a
        a = b - a*trunc(x/modulo)
        b = temp
        temp = modulo
        modulo = x % modulo
        x = temp
    if toSigned(b, width=width) < 0: b += originalModulo
    return toUnsigned(b, width=width)


class Random:
    """Implementation of `java.util.Random` (which uses a linear congruential generator with a 2^48 state space). This is most commonly used for pre-Java 1.18 mechanics, such as potential structure positions and a world's End."""
    seed: int
    def __init__(self, seed: int, *, asState: bool = False) -> None:
        """Creates and initializes an LCG with the seed `seed`.
        If `asState` is True, the LCG will instead set `seed` directly as the internal state without first converting it."""
        self.seed = (seed ^ (0 if asState else 0x5deece66d)) & FORTY_EIGHT_BITS

    def __next(self, bits: int) -> int:
        # TODO: Finish description
        """Advances the LCG's internal state once, returning a signed pseudorandom min(`bits`, 32)-bit integer in the process."""
        bits &= THIRTY_TWO_BITS
        self.seed = (self.seed * 0x5deece66d + 0xb) & FORTY_EIGHT_BITS
        return toSigned(self.seed >> (48 - bits), width=32)

    def nextInt(self, n: int) -> int:
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^32)-1]. The LCG's internal state will be advanced almost always once, but rarely multiple times if `n` is not a power of two."""
        n &= THIRTY_TWO_BITS
        if not n & n - 1: return ((n * self.__next(31)) >> 31) & THIRTY_TWO_BITS
        while True:
            bits = self.__next(31)
            val = (bits & THIRTY_TWO_BITS) % n
            if toSigned(bits - val + n, width=32) < 1: break
        return val
    
    def nextLong(self) -> int:
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the LCG's internal state twice in the process.
        Since the LCG uses a 48-bit state, only 2^48 integers within the aforementioned range are actually reachable."""
        return ((self.__next(32) << 32) + self.__next(32)) & SIXTY_FOUR_BITS

    def nextFloat(self) -> float:
        """Returns a pseudorandom single-precision float in the range [0, 1), advancing the LCG's internal state once in the process."""
        return self.__next(24) / FLOAT_LENGTH
    
    def nextDouble(self) -> float:
        """Returns a pseudorandom double-precision float in the range [0, 1), advancing the LCG's internal state twice in the process."""
        return ((self.__next(26) << 27) + self.__next(27)) / DOUBLE_LENGTH
    
    def skipN(self, n: int) -> None:
        """Advances the LCG's internal state `n` times if `n` is non-negative, or steps it back `n` times if `n` is negative."""
        m, a = 1, 0
        im, ia = (0x5deece66d, 0xb) if n >= 0 else (0xdfe05bcb1365, 0x615c0e462aa9)
        n &= FORTY_EIGHT_BITS
        while n:
            if n & 1:
            #     m = (m * im) & FORTY_EIGHT_BITS
            #     a = (a * im + ia) & FORTY_EIGHT_BITS
            # ia = (ia * (im + 1)) & FORTY_EIGHT_BITS
            # im = (im * im) & FORTY_EIGHT_BITS
                m *= im
                a = (a * im + ia)
            ia *= im + 1
            im *= im
            n >>= 1
        self.seed = (self.seed * m + a) & FORTY_EIGHT_BITS


class Xoroshiro:
    """Implementation of `xoroshiro128++` (developed by [TODO: NAME]) which has a 2^128 state space. Most commonly used for Overworld mechanics introduced in or after 1.18."""
    lo: int
    hi: int
    @multimethod
    def __init__(self, seed: int) -> None:
        """Creates and initializes a xoroshiro128++ generator with the seed `seed`.
        Since `seed` is treated as if a 64-bit integer, only 2^64 initial states for xoroshiro128++ are possible, despite its 2^128 overall state space."""
        # l = (seed ^ 0x6a09e667f3bcc909) & SIXTY_FOUR_BITS
        # h = (l + 0x9e3779b97f4a7c15) & SIXTY_FOUR_BITS
        # l = ((l ^ (l >> 30)) * 0xbf58476d1ce4e5b9) & SIXTY_FOUR_BITS
        # l = ((l ^ (l >> 30)) * 0x94d049bb133111eb) & SIXTY_FOUR_BITS
        # self.lo = l ^ (l >> 31)
        # h = ((h ^ (h >> 30)) * 0xbf58476d1ce4e5b9) & SIXTY_FOUR_BITS
        # h = ((h ^ (h >> 30)) * 0x94d049bb133111eb) & SIXTY_FOUR_BITS
        # self.hi = h ^ (h >> 31)
        l = (seed ^ 0x6a09e667f3bcc909)
        h = (l + 0x9e3779b97f4a7c15)
        l = ((l ^ (l >> 30)) * 0xbf58476d1ce4e5b9)
        l = ((l ^ (l >> 30)) * 0x94d049bb133111eb)
        self.lo = (l ^ (l >> 31)) & SIXTY_FOUR_BITS
        h = ((h ^ (h >> 30)) * 0xbf58476d1ce4e5b9)
        h = ((h ^ (h >> 30)) * 0x94d049bb133111eb)
        self.hi = (h ^ (h >> 31)) & SIXTY_FOUR_BITS
    
    @multimethod
    def __init__(self, lo: int, hi: int) -> None:
        """Creates and initializes a xoroshiro128++ generator with internal 64-bit lower bits `lo` and higher bits `hi`."""
        self.lo = lo & SIXTY_FOUR_BITS
        self.hi = hi & SIXTY_FOUR_BITS

    
    def __prev(self) -> None:
        """Steps the generator's internal state backwards once."""
        loXorHi = rotl(self.hi, 36)
        self.lo = rotl(self.lo ^ loXorHi ^ (loXorHi << 21), 15)
        self.hi = loXorHi ^ self.lo

    def __next(self) -> None:
        """Advances the generator's internal state once."""
        loXorHi = self.lo ^ self.hi
        self.lo = (rotl(self.lo, 49) ^ loXorHi ^ (loXorHi << 21)) & SIXTY_FOUR_BITS
        self.hi = rotl(loXorHi, 28)

    def nextLong(self) -> int:
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the generator's internal state once in the process."""
        n = toSigned(rotl(self.lo + self.hi, 17) + self.lo)
        self.__next()
        return n
    
    def nextInt(self, n: int) -> int:
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^32)-1].The generator's internal state is almost always advanced once in the process, but rarely multiples times."""
        n &= THIRTY_TWO_BITS
        r = ((self.nextLong() & THIRTY_TWO_BITS) * n) & SIXTY_FOUR_BITS
        # I don't believe that code will ever run, but keeping just in case.
        if ((r & THIRTY_TWO_BITS) < n):
            while ((r & THIRTY_TWO_BITS) < (~n + 1) % n):
                r = ((self.nextLong() & THIRTY_TWO_BITS) * n) & SIXTY_FOUR_BITS
        return r >> 32

    def nextFloat(self) -> float:
        """Returns a pseudorandom single-precision float in the range [0,1), advancing the internal state once in the process."""
        return (self.nextLong() >> 40) / FLOAT_LENGTH

    def nextDouble(self) -> float:
        """Returns a pseudorandom double-precision float in the range [0,1), advancing the internal state once in the process."""
        return (self.nextLong() >> 11) / DOUBLE_LENGTH
    
    def skipN(self, n: int) -> None:
        """Advances the generator's internal state `n` times if `n` is non-negative, or steps it back `n` times if `n` is negative."""
        if n >= 0:
            for _ in range(n & ONE_HUNDRED_TWENTY_EIGHT_BITS): self.__next()
        else:
            for _ in range(n & ONE_HUNDRED_TWENTY_EIGHT_BITS): self.__prev()

    def nextLongJava(self) -> int:
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the generator's internal state twice in the process.
        Since the LCG uses a 48-bit state, only 2^48 integers within the aforementioned range are actually reachable."""
        return toSigned(((self.nextLong() & THIRTY_TWO_BITS) << 32) + (self.nextLong() & THIRTY_TWO_BITS))
    
    def nextIntJava(self, n: int) -> int:
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^32)-1].The generator's internal state is almost always advanced once in the process, but rarely multiple times."""
        n &= THIRTY_TWO_BITS
        if not n & n - 1:
            return ((n*(self.nextLong() >> 33)) >> 31) & THIRTY_TWO_BITS
        while True:
            bits = (self.nextLong() >> 33) & THIRTY_TWO_BITS
            val = bits % n
            if toSigned(bits - val + n, width=32) < 1: break
        return val


class SeedHelper:
    """A 64-bit PRNG used in Java 1.17- chunk generation."""
    def getLayerSalt(self, salt: int) -> int:
        """Returns a layer's salt given its initial `salt`.
        This output is then fed to `getStartSalt()` and `getStartSeed()`."""
        return self.__getStartSalt(salt, salt)
    
    def __getStartSalt(self, worldseed: int, layerSalt: int) -> int:
        """Returns a starting salt given a worldseed and a layer salt (see `getLayerSalt()`).
        Not to be confused with `getStartSeed()`."""
        return self.stepSeed(self.stepSeed(self.stepSeed(worldseed, layerSalt), layerSalt), layerSalt)
    
    def getStartSeed(self, worldseed: int, layerSalt: int) -> int:
        """Returns a starting seed given a worldseed and a layer salt (see `getLayerSalt()`).
        This output is then fed to `getChunkSeed()`."""
        return self.stepSeed(self.__getStartSalt(worldseed, layerSalt), 0)

    @multimethod
    def getChunkSeed(self, startSeed: int, x: int, z: int) -> int:
        """Returns a chunk seed given coordinates and a starting seed (see `getStartSeed()`).
        This output is then fed to `firstInt()`."""
        return self.stepSeed(self.stepSeed(self.stepSeed(startSeed + x, z), x), z)

    @multimethod
    def getChunkSeed(self, worldseed: int, x: int, z: int, salt: int) -> int:
        """Returns a chunk seed given a worldseed, coordinates, and an initial `salt`.
        This output is then fed to `firstInt()`."""
        return self.stepSeed(self.stepSeed(self.stepSeed(self.getStartSeed(worldseed, self.getLayerSalt(salt)) + x, z), x), z)

    @multimethod
    def firstInt(self, chunkSeed: int, mod: int) -> int:
        """Returns the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)], given a chunk seed (see `getChunkSeed()`).
        The PRNG can then be advanced by calling `stepSeed()`."""
        # TODO: Double check bound in description
        mod &= THIRTY_TWO_BITS
        # TODO: Compare Python signed % to C signed %
        ret = toSigned((chunkSeed >> 24) % mod, width=32)
        if (ret < 0): ret += mod
        return ret

    @multimethod
    def firstInt(self, worldseed: int, x: int, z: int, salt: int, mod: int) -> int:
        """Returns the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)], given a worldseed, coordinates, and an initial salt.
        The PRNG can then be advanced by calling `stepSeed()`."""
        # TODO: Double check bound in description
        mod &= THIRTY_TWO_BITS
        # TODO: Compare Python signed % to C signed %
        ret = toSigned((self.getChunkSeed(worldseed, x, z, salt) >> 24) % mod, width=32)
        if (ret < 0): ret += mod
        return ret
    
    @multimethod
    def firstIntIsZero(self, chunkSeed: int, mod: int) -> bool:
        """Returns whether the first pseudorandom integer returned by the PRNG initialized by a chunk seed (see `getChunkSeed()`) is zero."""
        return not (((chunkSeed >> 24) % (mod & THIRTY_TWO_BITS)) & THIRTY_TWO_BITS)
    
    @multimethod
    def firstIntIsZero(self, worldseed: int, x: int, z: int, salt: int, mod: int) -> bool:
        """Returns whether the first pseudorandom integer returned by the PRNG initialized by a worldseed, coordinates, and initial salt is zero."""
        return not (((self.getChunkSeed(worldseed, x, z, salt) >> 24) % (mod & THIRTY_TWO_BITS)) & THIRTY_TWO_BITS)
    
    def stepSeed(self, chunkSeed: int, startSalt: int) -> int:
        """Advances the PRNG once.
        To initialize the PRNG first, see `firstInt()`."""
        return (chunkSeed * (chunkSeed * 6364136223846793005 + 1442695040888963407) + startSalt) & SIXTY_FOUR_BITS