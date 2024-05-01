from .base import (
    overload, Sequence,
    THIRTY_TWO_BITS, FORTY_EIGHT_BITS, SIXTY_FOUR_BITS, ONE_HUNDRED_TWENTY_EIGHT_BITS, FLOAT_LENGTH, DOUBLE_LENGTH,
    toSigned, toUnsigned, rotateLeft,
    Coordinate
)

class Random:
    """Implementation of [java.util.Random](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/Random.html) (which uses a [linear congruential generator](https://en.wikipedia.org/wiki/Linear_congruential_generator) with a 2^48 state space). This is most commonly used for pre-Java 1.18 mechanics, potential structure positions, and a world's Nether and End."""
    state: int
    def __init__(self, seed: int, *, asState: bool = False) -> None: # rng.setSeed()
        """Creates and initializes an LCG with the seed `seed`.
        If `asState` is True, the LCG will instead set `seed` directly as the internal state without first converting it."""
        if not asState: self.seed(seed)
        else: self.state = seed & FORTY_EIGHT_BITS

    def seed(self, seed: int) -> None:
        """Reseeds an existing LCG with the provided `seed`."""
        self.state = (seed ^ 0x5deece66d) & FORTY_EIGHT_BITS

    def seedForStructurePieceAttempt(self, chunkCoord: Sequence[int]) -> None:
        """"""
        self.seed(self.state ^ (chunkCoord[0] >> 4) ^ (chunkCoord[1] & -16))
        self.nextInt(31)

    def seedForChunk(self, seed: int, chunkCoord: Sequence[int]) -> None:
        self.seed(seed)
        self.seed(seed ^ (self.nextLong() * chunkCoord[0]) ^ (self.nextLong() * chunkCoord[-1]))

    def _next(self, bits: int) -> int: # rng.next()
        # TODO: Finish description
        """Advances the LCG's internal state once, returning a pseudorandom min(`bits`, 32)-bit integer in the process.
        If `bits` >= 32, the integer will be signed; otherwise it will be unsigned."""
        bits = min(max(0, bits), 32)
        self.state = (self.state * 0x5deece66d + 0xb) & FORTY_EIGHT_BITS
        return toSigned(self.state >> (48 - bits), width=32)

    def nextInt(self, n: int) -> int: # rng.nextInt()
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^31)-1]. The LCG's internal state will be advanced almost always once, but rarely multiple times (with probability (2^31 mod `n`)/2^31)."""
        n &= THIRTY_TWO_BITS
        if not (n & n - 1): return ((n * self._next(31)) >> 31) & THIRTY_TWO_BITS
        while True:
            bits = self._next(31)
            val = (bits & THIRTY_TWO_BITS) % n
            if toSigned(bits - val + n, width=32) < 1: break
        return val
    
    def nextLong(self) -> int: # rng.nextLong()
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the LCG's internal state twice in the process.
        Since the LCG uses a 48-bit state, only 2^48 integers within the aforementioned range are actually reachable."""
        return toSigned((self._next(32) << 32) + self._next(32))

    def nextFloat(self) -> float: # rng.nextFloat()
        """Returns a pseudorandom single-precision float in the range [0, 1), advancing the LCG's internal state once in the process."""
        return (self._next(24)) / FLOAT_LENGTH
    
    def nextDouble(self) -> float: # rng.nextDouble()
        """Returns a pseudorandom double-precision float in the range [0, 1), advancing the LCG's internal state twice in the process."""
        return (((self._next(26) << 27) + self._next(27)) & SIXTY_FOUR_BITS) / DOUBLE_LENGTH
    
    def skip(self, n: int) -> None: # rng.skipNextN()
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
        self.state = (self.state * m + a) & FORTY_EIGHT_BITS


class Xoroshiro:
    """Implementation of [xoroshiro128++](https://prng.di.unimi.it/xoroshiro128plusplus.c) (developed by David Blackman and Sebastiano Vigna) which has a 2^128 state space. Most commonly used for Overworld mechanics introduced in or after Java/Bedrock 1.18."""
    lo: int
    hi: int
    @overload
    def __init__(self, seed: int, /) -> None: # rng.xSetSeed()
        """Creates and initializes a xoroshiro128++ generator from a PRNG seed.
        Since the seed is interpreted as a 64-bit integer, only 2^64 initial states for xoroshiro128++ are possible, despite its 2^128 overall state space."""
        ...

    @overload
    def __init__(self, lo: int, hi: int, /) -> None:
        """Creates a xoroshiro128++ generator and sets its internal state directly to the two provided values."""
        ...

    @overload
    def __init__(self, bits: Sequence[int], /) -> None:
        """Creates a xoroshiro128++ generator and sets its internal state directly to the first two values from the provided sequence.
        The values will be evaluated in the order `loBits`, `hiBits`."""
        ...

    def __init__(self, seedOrLo: int | Sequence[int], hi: int | None = None) -> None:
        if isinstance(seedOrLo, int):
            if hi is None: self.seed(seedOrLo)
            else:
                self.lo = seedOrLo & SIXTY_FOUR_BITS
                self.hi = hi & SIXTY_FOUR_BITS
        else:
            self.lo = seedOrLo[0] & SIXTY_FOUR_BITS
            self.hi = seedOrLo[1] & SIXTY_FOUR_BITS

    def seed(self, seed: int) -> None:
        """Reseeds an existing xoroshrio128++ generator with the seed `seed`."""
        l = seed ^ 0x6a09e667f3bcc909
        h = l + 0x9e3779b97f4a7c15
        l = (l ^ (l >> 30)) * 0xbf58476d1ce4e5b9
        l = (l ^ (l >> 30)) * 0x94d049bb133111eb
        self.lo = (l ^ (l >> 31)) & SIXTY_FOUR_BITS
        h = (h ^ (h >> 30)) * 0xbf58476d1ce4e5b9
        h = (h ^ (h >> 30)) * 0x94d049bb133111eb
        self.hi = (h ^ (h >> 31)) & SIXTY_FOUR_BITS

    def _prev(self) -> None:
        """Steps the generator's internal state backwards once."""
        loXorHi = rotateLeft(self.hi, 36)
        self.lo = rotateLeft(self.lo ^ loXorHi ^ (loXorHi << 21), 15)
        self.hi = loXorHi ^ self.lo

    def _next(self) -> None:
        """Advances the generator's internal state once."""
        loXorHi = self.lo ^ self.hi
        self.lo = (rotateLeft(self.lo, 49) ^ loXorHi ^ (loXorHi << 21)) & SIXTY_FOUR_BITS
        self.hi = rotateLeft(loXorHi, 28)

    def nextLong(self) -> int: # rng.xNextLong()
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the generator's internal state once in the process."""
        n = toSigned(rotateLeft(self.lo + self.hi, 17) + self.lo)
        self._next()
        return n
    
    def nextInt(self, n: int) -> int: # rng.xNextInt()
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^32)-1]. The generator's internal state is almost always advanced once in the process, but rarely multiples times."""
        # TODO: Check--2^32 or 2^31?
        n &= THIRTY_TWO_BITS
        r = ((self.nextLong() & THIRTY_TWO_BITS) * n) & SIXTY_FOUR_BITS
        # I don't believe that code will ever run, but keeping just in case.
        if (r & THIRTY_TWO_BITS) < n:
            while (r & THIRTY_TWO_BITS) < (~n + 1) % n: r = ((self.nextLong() & THIRTY_TWO_BITS) * n) & SIXTY_FOUR_BITS
        return r >> 32

    def nextFloat(self) -> float:
        """Returns a pseudorandom single-precision float in the range [0,1), advancing the internal state once in the process."""
        return (self.nextLong() >> 40) / FLOAT_LENGTH

    def nextDouble(self) -> float: # rng.xNextDouble()
        """Returns a pseudorandom double-precision float in the range [0,1), advancing the internal state once in the process."""
        return (self.nextLong() >> 11) / DOUBLE_LENGTH
    
    def skip(self, n: int) -> None: # rng.xSkipN()
        """Advances the generator's internal state `n` times if `n` is non-negative, or steps it back `n` times if `n` is negative."""
        if n >= 0:
            for _ in range(n & ONE_HUNDRED_TWENTY_EIGHT_BITS): self._next()
        else:
            for _ in range(n & ONE_HUNDRED_TWENTY_EIGHT_BITS): self._prev()

    def nextLongJava(self) -> int: # rng.xNextLongJ()
        """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the generator's internal state twice in the process."""
        return toSigned(((self.nextLong() & THIRTY_TWO_BITS) << 32) + (self.nextLong() & THIRTY_TWO_BITS))
    
    def nextIntJava(self, n: int) -> int:
        """Returns a pseudorandom integer in the range [0, min(`n`, 2^32)-1].The generator's internal state is almost always advanced once in the process, but rarely multiple times."""
        n &= THIRTY_TWO_BITS
        if not n & n - 1:
            return ((n*(self.nextLong() >> 33)) >> 31) & THIRTY_TWO_BITS
        while True:
            bits = (self.nextLong() >> 33) & THIRTY_TWO_BITS
            val = bits % n
            if toSigned(bits - val + n, width=32) >= 0: break
        return val


# """From Chunkbiomes"""
# class MersenneTwister:
#     state: list[int]
#     index: int
#     def __init__(self, seed: int) -> None:
#         """Creates and initializes a Mersenne Twister with the seed `seed`."""
#         self.state = [i for i in range(624)]
#         self.seed(seed)

#     def seed(self, seed: int) -> None:
#         """Reseeds an existing Mersenne Twister with the seed `seed`."""
#         self.state[0] = seed & THIRTY_TWO_BITS
#         for i in range(1, 624):
#             seed = self.state[-1] ^ (self.state[-1] >> 30)
#             self.state[i] = (1812433253 * seed + i) & THIRTY_TWO_BITS
#         self.index = 624

#     def _twist(self) -> None:
#         """Twists the internal state of the generator, creating 624 new integers."""
#         for i in range(624):
#             val = (self.state[i] & 0x80000000) | (self.state[i + 1 if i < 624 else 0] & 0x7fffffff)
#             self.state[i] = self.state[i + 397 if i < 227 else i - 227] ^ (val >> 1) ^ ((2567483615*(val & 1)) & THIRTY_TWO_BITS)
#         self.index = 0

#     def _next(self) -> int:
#         # TODO: Finish description
#         """Advances the generator's internal state once, returning an unsigned pseudorandom 32-bit integer in the process."""
#         if self.index > 624: self._twist()
#         out = self.state[self.index]
#         self.index += 1
#         out ^= out >> 11
#         out ^= (out << 7) & 2636928640
#         out ^= (out << 15) & 4022730752
#         return out ^ (out >> 18)

#     def nextInt(self, n: int | None = None) -> int:
#         """Returns a pseudorandom integer, either in the range [0, 2^31 - 1] if `n` is not specified, or in the range [0, min(`n`, 2^32)-1] if `n` is specified. The generator's internal state will be advanced once."""
#         if n is None: return self._next() >> 1
#         n &= THIRTY_TWO_BITS
#         return self._next() % n
    
#     def nextLong(self) -> int:
#         """Returns a pseudorandom integer in the range [-2^63, 2^63 - 1], advancing the LCG's internal state twice in the process.
#         Since the LCG uses a 48-bit state, only 2^48 integers within the aforementioned range are actually reachable."""
#         return ((self._next() << 32) + self._next()) & SIXTY_FOUR_BITS

#     def nextFloat(self) -> float:
#         """Returns a pseudorandom single-precision float in the range [0, 1), advancing the LCG's internal state once in the process."""
#         return self.nextDouble()
    
#     def nextDouble(self) -> float:
#         """Returns a pseudorandom double-precision float in the range [0, 1), advancing the LCG's internal state twice in the process."""
#         return self._next() * 2.3283064365386963E-10
    
#     def skip(self, n: int) -> None:
#         """Advances the LCG's internal state `n` times if `n` is non-negative, or steps it back `n` times if `n` is negative."""
#         assert n >= 0
#         self.index += n
#         for _ in range(self.index//624): self._twist()
#         self.index %= 624


class SeedHelper:
    """A 64-bit PRNG used in Java 1.17- chunk generation."""
    @classmethod
    def getLayerSalt(cls, initialSalt: int) -> int: # rng.getLayerSalt()
        """Returns a layer's salt given its initial `salt`.
        This output is then fed to `getStartSalt()` and `getStartSeed()`."""
        return cls.getStartSalt(initialSalt, initialSalt)
    
    @classmethod
    def getStartSalt(cls, worldseed: int, layerSalt: int) -> int: # rng.getStartSalt()
        """Returns a starting salt given a worldseed and a layer salt (see `getLayerSalt()`).
        Not to be confused with `getStartSeed()`."""
        return cls.stepSeed(cls.stepSeed(cls.stepSeed(worldseed, layerSalt), layerSalt), layerSalt)
    
    @classmethod
    def getStartSeed(cls, worldseed: int, layerSalt: int) -> int: # rng.getStartSeed()
        """Returns a starting seed given a worldseed and a layer salt (see `getLayerSalt()`).
        This output is then fed to `getChunkSeed()`."""
        return cls.stepSeed(cls.getStartSalt(worldseed, layerSalt), 0)

    @overload
    @classmethod
    def getChunkSeed(cls, startingSeed: int, coordinate: Coordinate, /) -> int: # rng.getChunkSeed()
        """Returns a chunk seed given a starting seed (see `getStartSeed()`) and a 2D coordinate.
        This output is then usually fed to `firstInt()`."""
        ...

    @overload
    @classmethod
    def getChunkSeed(cls, worldseed: int, coordinate: Coordinate, initialSalt: int, /) -> int:
        """Returns a chunk seed given a worldseed, a 2D coordinate, and an initial salt.
        This output is then usually fed to `firstInt()`."""
        ...

    @classmethod
    def getChunkSeed(cls, seed: int, coordinate: Coordinate, initialSalt: int | None = None) -> int:
        return cls.stepSeed(cls.stepSeed(cls.stepSeed((seed if initialSalt is None else cls.getStartSeed(seed, cls.getLayerSalt(initialSalt))) + coordinate.x, coordinate.z), coordinate.x), coordinate.z)

    @overload
    @classmethod
    def firstInt(cls, chunkSeed: int, mod: int, /) -> int: #rng.mcFirstInt()
        """Returns the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)], given a chunk seed (see `getChunkSeed()`).
        The PRNG can then be advanced by calling `stepSeed()`."""
        # TODO: Double check bound in description
        ...

    @overload
    @classmethod
    def firstInt(cls, worldseed: int, mod: int, coordinate: Coordinate, initialSalt: int, /) -> int:
        """Returns the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)], given a worldseed, a 2D coordinate, and an initial salt.
        The PRNG can then be advanced by calling `stepSeed()`."""
        # TODO: Double check bound in description
        ...

    @classmethod
    def firstInt(cls, seed: int, mod: int, coordinate: Coordinate | None = None, initialSalt: int | None = None) -> int:
        mod &= THIRTY_TWO_BITS
        ret = toSigned(((seed if coordinate is None or initialSalt is None else cls.getChunkSeed(seed, coordinate, initialSalt)) >> 24) % mod, width=32)
        if (ret < 0): ret += mod
        return ret
    
    @overload
    @classmethod
    def firstIntIsZero(cls, seed: int, mod: int) -> bool: # rng.mcFirstIntIsZero()
        """Returns whether the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)] is zero, given a chunk seed (see `getChunkSeed()`)."""
        ...

    @overload
    @classmethod
    def firstIntIsZero(cls, seed: int, mod: int, coordinate: Coordinate, initialSalt: int) -> bool:
        """Returns whether the first pseudorandom integer returned by the PRNG in the range [0, min(`mod`, 2^32)] is zero, given a worldseed, a 2D coordinate, and an initial salt."""
        ...

    @classmethod
    def firstIntIsZero(cls, seed: int, mod: int, coordinate: Coordinate | None = None, initialSalt: int | None = None) -> bool:
        return not ((((seed if coordinate is None or initialSalt is None else cls.getChunkSeed(seed, coordinate, initialSalt)) >> 24) % (mod & THIRTY_TWO_BITS)) & THIRTY_TWO_BITS)
    
    @staticmethod
    def stepSeed(chunkSeed: int, startSalt: int, *, width: int = 64) -> int: # mcStepSeed()
        """Advances the PRNG once.
        To initialize the PRNG first, see `firstInt()`."""
        return toUnsigned(chunkSeed * (chunkSeed * 6364136223846793005 + 1442695040888963407) + startSalt, width=width)
    
    # @classmethod
    # def selectOneOfFour(cls, a: int, b: int, c: int, d: int, *, chunkSeed: int, startSalt: int) -> int:
