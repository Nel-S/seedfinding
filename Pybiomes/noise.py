from .base import (
    overload,
    EIGHT_BITS,
    indexedLerp, lerp, doubleLerp, tripleLerp,
    Position
)
from .rng import (
    Random, Xoroshiro
)
from math import sqrt
from typing import Literal

class SimplexNoise:
    # TODO: Find range
    """Gradient noise developed by Ken Perlin. Returns values in the range +-...."""
    d: list[int]
    lacunarity: float
    def __init__(self, lacunarity: float, *, prng: Random | Xoroshiro | None = None) -> None:
        """Initializes the Simplex Noise generator, and optionally seeds it if `prng` is not None."""
        self.lacunarity = lacunarity
        if prng is not None: self.seed(prng)

    def seed(self, prng: Random | Xoroshiro) -> None:
        """Seeds the Simplex Noise generator without disturbing all other initialized constants."""
        prng.skipN(6)
        self.d = [i for i in range(256)]
        for i in range(256):
            j = i + prng.nextInt(256 - i)
            temp = self.d[i]
            self.d[i] = self.d[j]
            self.d[j] = temp
    
    def _getGradient(self, index: int, position: Position, d: float = 0.5) -> float: # noise.simplexGrad()
        """Calculates a gradient at a 3D position using indexed linear interpolation."""
        con = d - position.x*position.x - position.y*position.y - position.z*position.z
        return 0. if con <= 0 else con*con*con*con*indexedLerp(index, position.x, position.y, position.z)

    def sample(self, position: Position) -> float: # noise.sampleSimplex2D()
        """Samples the Simplex Noise generator at a 2D position."""
        # TODO: Clean up y-z mismatch
        x, y = position.x, position.z
        UNSKEW = (3 - sqrt(3))/6
        # hf = (x + y)*(sqrt(3) - 1)/2
        # hx = int((x + hf)//1)
        # hy = int((y + hf)//1)
        hx = int((x*(sqrt(3)+1) + y*(sqrt(3)-1))//2)
        hy = int((x*(sqrt(3)-1) + y*(sqrt(3)+1))//2)
        mhxy = (hx + hy)*UNSKEW
        x -= hx + mhxy
        y -= hy + mhxy
        offx = x > y
        offy = not offx

        # Oddly, the indexed lerp in _getGradient treats z as though it were the y-coordinate. I haven't the slightest idea why.
        return 70*(self._getGradient(self.d[(self.d[hy & EIGHT_BITS] + hx) & EIGHT_BITS] % 12, Position(x, y=y)) + self._getGradient(self.d[(self.d[(hy + offy) & EIGHT_BITS] + hx + offx) & EIGHT_BITS] % 12, Position(x - offx + UNSKEW, y=y - offy + UNSKEW)) + self._getGradient(self.d[(self.d[(hy + 1) & EIGHT_BITS] + hx + 1) & EIGHT_BITS] % 12, Position(x - 1/sqrt(3), y=y - 1/sqrt(3))))


class PerlinNoise: # noise.PerlinNoise
    """Gradient noise developed by Ken Perlin. Returns values in the range +-1.03..., with a large bias towards zero."""
    a: float
    b: float
    c: float
    d: list[int]
    amplitude: float
    lacunarity: float
    yFractionalPartAt0: float = 0
    yIntegerPartAt0: int = 0
    yLerpWeightAt0: float = 0
    def __init__(self, amplitude: float, lacunarity: float, *, prng: Random | Xoroshiro | None = None, precomputeY: bool = True) -> None: # noise.perlinInit(), noise.xPerlinInit()
        """Initializes the Perlin Noise generator, and optionally seeds it if `prng` is not None.
        If the generator is seeded, the intended version is Java Beta 1.8+, and multiple samples with y=0 will be done, `precomputeY` can precompute y-related values to make sampling slightly faster."""
        self.amplitude = amplitude
        self.lacunarity = lacunarity
        if prng is not None: self.seed(prng, precomputeY=precomputeY)


    def seed(self, prng: Random | Xoroshiro, *, precomputeY: bool = True) -> None: # noise.perlinInit(), noise.xPerlinInit()
        """Seeds the Perlin Noise generator without disturbing all other initialized constants.
        If the intended version is Java Beta 1.8+ and multiple samples with y=0 will be done, `precomputeY` can precompute y-related values to make sampling slightly faster."""
        self.a = 256*prng.nextDouble()
        self.b = 256*prng.nextDouble()
        self.c = 256*prng.nextDouble()
        
        self.d = [i for i in range(256)]
        for i in range(256):
            j = i + prng.nextInt(256 - i)
            temp = self.d[i]
            self.d[i] = self.d[j]
            self.d[j] = temp
        self.d.append(self.d[0])

        if precomputeY:
            self.yIntegerPartAt0 = int(self.b//1) & EIGHT_BITS
            self.yFractionalPartAt0 = self.b % 1
            self.yLerpWeightAt0 = self.yFractionalPartAt0*self.yFractionalPartAt0*self.yFractionalPartAt0*(self.yFractionalPartAt0*(self.yFractionalPartAt0*6 - 15) + 10)
    

    def sample(self, position: Position, *, yAmplitude: float = 0, yMinimum: float = 0) -> float: # noise.samplePerlin()
        # TODO: Figure out role of yAmplitude and yMinimum
        """Samples the Perlin Noise generator at a 3D position in Beta 1.8+.
        If `yAmplitude` is nonzero, the fractional part of y will be clamped during the sampling."""
        x, y, z = position.x + self.a, position.y, position.z + self.c
        xIntegerPart, zIntegerPart = int(x//1) & EIGHT_BITS, int(z//1) & EIGHT_BITS
        xFractionalPart, zFractionalPart = x % 1, z % 1

        if not y:
            yIntegerPart = self.yIntegerPartAt0
            yFractionalPart = self.yFractionalPartAt0
            yLerpWeight = self.yLerpWeightAt0
        else:
            y += self.b
            yIntegerPart = int(y//1) & EIGHT_BITS
            yFractionalPart = y % 1
            yLerpWeight = yFractionalPart*yFractionalPart*yFractionalPart*(yFractionalPart*(yFractionalPart*6 - 15) + 10)
        if yAmplitude: yFractionalPart -= (min(yMinimum, yFractionalPart)//yAmplitude)*yAmplitude

        step01 = (self.d[xIntegerPart    ] + yIntegerPart) & EIGHT_BITS
        step02 = (self.d[xIntegerPart + 1] + yIntegerPart) & EIGHT_BITS
        step11 = (self.d[step01    ] + zIntegerPart) & EIGHT_BITS
        step12 = (self.d[step02    ] + zIntegerPart) & EIGHT_BITS
        step13 = (self.d[step01 + 1] + zIntegerPart) & EIGHT_BITS
        step14 = (self.d[step02 + 1] + zIntegerPart) & EIGHT_BITS

        return tripleLerp(
            indexedLerp(self.d[step11    ], xFractionalPart    , yFractionalPart    , zFractionalPart    ),
            indexedLerp(self.d[step12    ], xFractionalPart - 1, yFractionalPart    , zFractionalPart    ),
            indexedLerp(self.d[step13    ], xFractionalPart    , yFractionalPart - 1, zFractionalPart    ),
            indexedLerp(self.d[step14    ], xFractionalPart - 1, yFractionalPart - 1, zFractionalPart    ),
            indexedLerp(self.d[step11 + 1], xFractionalPart    , yFractionalPart    , zFractionalPart - 1),
            indexedLerp(self.d[step12 + 1], xFractionalPart - 1, yFractionalPart    , zFractionalPart - 1),
            indexedLerp(self.d[step13 + 1], xFractionalPart    , yFractionalPart - 1, zFractionalPart - 1),
            indexedLerp(self.d[step14 + 1], xFractionalPart - 1, yFractionalPart - 1, zFractionalPart - 1),
            xFractionalPart*xFractionalPart*xFractionalPart*(xFractionalPart*(xFractionalPart*6 - 15) + 10),
            yLerpWeight,
            zFractionalPart*zFractionalPart*zFractionalPart*(zFractionalPart*(zFractionalPart*6 - 15) + 10)
        )

    def sample_Beta_1_7_Terrain(self, position: Position, yLacAmp: float) -> tuple[float, float]: # noise.samplePerlinBeta17Terrain()
        """Samples the Perlin Noise generator at a 2D position for Beta 1.7 terrain.
        The output will be a tuple of exactly two samples."""
        x, z = position.x + self.a, position.z + self.c
        xIntegerPart, zIntegerPart = int(x//1) & EIGHT_BITS, int(z//1) & EIGHT_BITS
        xFractionalPart, zFractionalPart = x % 1, z % 1
        xLerpWeight = xFractionalPart*xFractionalPart*xFractionalPart*(xFractionalPart*(xFractionalPart*6 - 15) + 10)
        zLerpWeight = zFractionalPart*zFractionalPart*zFractionalPart*(zFractionalPart*(zFractionalPart*6 - 15) + 10)

        genFlag = -1
        for i in range(8):
            yIntegerPart = int((i*self.lacunarity*yLacAmp + self.b)//1) & EIGHT_BITS
            if not i or yIntegerPart != genFlag:
                iLowerBound = i
                genFlagCopy = genFlag
                genFlag = yIntegerPart

        out: tuple[float, float] = tuple()
        lerp1 = lerp2 = lerp3 = lerp4 = 0
        for i in range(iLowerBound, 9):
            y = i*self.lacunarity*yLacAmp + self.b
            yIntegerPart = int(y//1) & EIGHT_BITS
            yFractionalPart = y % 1
            yLerpWeight = yFractionalPart*yFractionalPart*yFractionalPart*(yFractionalPart*(yFractionalPart*6 - 15) + 10)
            if not i or yIntegerPart != genFlagCopy:
                step01 = (self.d[xIntegerPart    ] + yIntegerPart) & EIGHT_BITS
                step02 = (self.d[xIntegerPart + 1] + yIntegerPart) & EIGHT_BITS
                step11 = (self.d[step01    ] + zIntegerPart) & EIGHT_BITS
                step12 = (self.d[step02    ] + zIntegerPart) & EIGHT_BITS
                step13 = (self.d[step01 + 1] + zIntegerPart) & EIGHT_BITS
                step14 = (self.d[step02 + 1] + zIntegerPart) & EIGHT_BITS

                lerp1 = lerp(indexedLerp(self.d[step11    ], xFractionalPart, yFractionalPart    , zFractionalPart    ), indexedLerp(self.d[step12    ], xFractionalPart - 1, yFractionalPart    , zFractionalPart    ), xLerpWeight)
                lerp2 = lerp(indexedLerp(self.d[step13    ], xFractionalPart, yFractionalPart - 1, zFractionalPart    ), indexedLerp(self.d[step14    ], xFractionalPart - 1, yFractionalPart - 1, zFractionalPart    ), xLerpWeight)
                lerp3 = lerp(indexedLerp(self.d[step11 + 1], xFractionalPart, yFractionalPart    , zFractionalPart - 1), indexedLerp(self.d[step12 + 1], xFractionalPart - 1, yFractionalPart    , zFractionalPart - 1), xLerpWeight)
                lerp4 = lerp(indexedLerp(self.d[step13 + 1], xFractionalPart, yFractionalPart - 1, zFractionalPart - 1), indexedLerp(self.d[step14 + 1], xFractionalPart - 1, yFractionalPart - 1, zFractionalPart - 1), xLerpWeight)

                genFlag = yIntegerPart
            if 7 <= i: out += (self.amplitude*doubleLerp(lerp1, lerp2, lerp3, lerp4, yLerpWeight, zLerpWeight),) #type: ignore # iLowerBound must be <= 7, so the starting value of i must be <= 7, meaning this is guaranteed to execute twice and create a tuple of exactly two floats
        return out


# TODO: Untested
class PerlinNoiseOctave: # noise.OctaveNoise
    """An octave containing multiple Perlin Noise generators."""
    octaves: tuple[PerlinNoise, ...] = tuple()
    initializedOctaves: int = 0
    @overload
    def __init__(self, lengths: int, firstOctave: int, octavesToInitialize: int | None = None, *, prng: Random | None = None, beta: Literal[False] = False) -> None: ... # noise.octaveInit()

    @overload
    def __init__(self, lengths: int, firstOctave: int, octavesToInitialize: int | None = None, *, prng: Random | None = None, beta: Literal[True] = True) -> None: ...

    @overload
    def __init__(self, lengths: int, firstOctave: Literal[0] = 0, octavesToInitialize: int | None = None, *, initialAmplitude: float, amplitudeFactor: float, initialLacunarity: float, lacunarityFactor: float, prng: Random | None = None, beta: Literal[True] = True) -> None: ... # noise.octaveInitBeta()

    @overload
    def __init__(self, lengths: tuple[float, ...], firstOctave: int, octavesToInitialize: int | None = None, *, prng: Xoroshiro | None = None) -> None: ... # noise.xOctaveInit()

    def __init__(self, lengths: int | tuple[float, ...], firstOctave: int = 0, octavesToInitialize: int | None = None, *, initialAmplitude: float | None = None, amplitudeFactor: float = 2, initialLacunarity: float | None = None, lacunarityFactor: float = 0.5, prng: Random | Xoroshiro | None = None, beta: bool = False) -> None:
        """Initializes the Perlin Noise octave, and optionally seeds it if `prng` is not None.
        Fewer than the total number of octaves can be initialized if `octavesToInitialize` is specified.
        Set `beta` to true if the emulated version is Beta 1.8-."""
        if isinstance(lengths, int):
            if initialAmplitude is None: initialAmplitude = 1/((1 << lengths) - 1)
            if initialLacunarity is None: initialLacunarity = 2.0**(firstOctave + lengths - 1)
        else:
            if initialAmplitude is None: initialAmplitude = (1 << (len(lengths) - 1))/((1 << len(lengths)) - 1)
            if initialLacunarity is None: initialLacunarity = 2.0**firstOctave

        for amp in (range(lengths) if isinstance(lengths, int) else lengths):
            if octavesToInitialize and self.initializedOctaves >= octavesToInitialize: break
            if isinstance(lengths, tuple) and not amp: continue
            self.octaves += (PerlinNoise((amp if isinstance(lengths, tuple) else 1) * initialAmplitude, initialLacunarity),)
            initialAmplitude *= amplitudeFactor
            initialLacunarity *= lacunarityFactor
            self.initializedOctaves += 1

        if prng is not None:
            if isinstance(prng, Xoroshiro):
                assert isinstance(lengths, tuple)
                self.seed(prng, firstOctave, lengths)
            elif not beta: self.seed(prng, firstOctave, beta=False)
            else: self.seed(prng, beta=True)

    @overload
    def seed(self, prng: Random, firstOctave: int, *, beta: Literal[False] = False) -> None: ... # noise.octaveInit()

    @overload
    def seed(self, prng: Random, *, beta: Literal[True] = True) -> None: ... # noise.octaveInitBeta()

    @overload
    def seed(self, prng: Xoroshiro, firstOctave: int, amplitudes: tuple[float, ...]) -> None: ... # noise.xOctaveInit()

    def seed(self, prng: Random | Xoroshiro, firstOctave: int = 0, amplitudes: tuple[float, ...] | None = None, *, beta: bool = False) -> None:
        """Seeds the Perlin Noise octave using java.util.Random without disturbing all other initialized constants.
        Set `beta` to true if the emulated version is Beta 1.8-."""
        if isinstance(prng, Random):
            if not beta:
                end = firstOctave + self.initializedOctaves - 1
                if end: prng.skipN(-262*end)
            for i in range(self.initializedOctaves): self.octaves[i].seed(prng)
        else:
            MD5_OCTAVE_N = (
                (0xb198de63a8012672, 0x7b84cad43ef7b5a8), # md5 "octave_-12"
                (0x0fd787bfbc403ec3, 0x74a4a31ca21b48b8), # md5 "octave_-11"
                (0x36d326eed40efeb2, 0x5be9ce18223c636a), # md5 "octave_-10"
                (0x082fe255f8be6631, 0x4e96119e22dedc81), # md5 "octave_-9"
                (0x0ef68ec68504005e, 0x48b6bf93a2789640), # md5 "octave_-8"
                (0xf11268128982754f, 0x257a1d670430b0aa), # md5 "octave_-7"
                (0xe51c98ce7d1de664, 0x5f9478a733040c45), # md5 "octave_-6"
                (0x6d7b49e7e429850a, 0x2e3063c622a24777), # md5 "octave_-5"
                (0xbd90d5377ba1b762, 0xc07317d419a7548d), # md5 "octave_-4"
                (0x53d39c6752dac858, 0xbcd1c5a80ab65b3e), # md5 "octave_-3"
                (0xb4a24d7a84e7677b, 0x023ff9668e89b5c4), # md5 "octave_-2"
                (0xdffa22b534c5f608, 0xb9b67517d3665ca9), # md5 "octave_-1"
                (0xd50708086cef4d7c, 0x6e1651ecc7f43309), # md5 "octave_0"
            )
            xlo = prng.nextLong()
            xhi = prng.nextLong()
            for i in range(self.initializedOctaves):
                if amplitudes is not None and amplitudes[i]: self.octaves[i].seed(Xoroshiro(xlo ^ MD5_OCTAVE_N[12 + firstOctave + i][0], xhi ^ MD5_OCTAVE_N[12 + firstOctave + i][1]))

    def sample(self, position: Position, yAmplitude: float = 0, yMinimum: float = 0, *, ignoreY: bool = False) -> float: # noise.sampleOctave(), noise.sampleOctaveAmp()
        """Samples the Perlin Noise octave at a 3D position, nullifying the contribution of the `y` value if `ignoreY` is true.
        If `yAmplitude` is nonzero, the fractional part of y will be clamped during the sampling."""
        return sum(octave.amplitude*octave.sample(Position(position.x*octave.lacunarity, position.z*octave.lacunarity, y=-octave.b if ignoreY else position.y*octave.lacunarity), yAmplitude=yAmplitude*octave.lacunarity, yMinimum=yMinimum*octave.lacunarity) for octave in self.octaves[:self.initializedOctaves])
    
    def sample_Beta_1_7_Biomes(self, position: Position) -> float: # noise.sampleOctaveBeta17Biome()
        """Samples the Perlin Noise octave at a 2D position for Beta 1.7 biomes."""
        # return sum(octave.amplitude*octave.sample(x*octave.lacunarity + octave.a, z*octave.lacunarity + octave.b) for octave in self.octaves[:self.initializedOctaves])
        sample = 0.
        for octave in self.octaves[:self.initializedOctaves]:
            # sn = SimplexNoise(octave.amplitude, octave.lacunarity)
            sn = SimplexNoise(octave.lacunarity)
            sn.d = octave.d
            sample += sn.sample(Position(position.x*octave.lacunarity + octave.a, position.z*octave.lacunarity + octave.b))
        return sample

    def sample_Beta_1_7_Terrain(self, position: Position, minimumLacunarity: float = 0, *, yLacunarityFlag: bool) -> tuple[float, float]: # noise.sampleOctaveBeta17Terrain()
        """Samples the Perlin Noise octave at a 2D position for Beta 1.7 terrain.
        The output will be a tuple of exactly two samples."""
        samples = (0., 0.)
        for octave in self.octaves[:self.initializedOctaves]:
            if minimumLacunarity and octave.lacunarity > minimumLacunarity: continue
            newSample = octave.sample_Beta_1_7_Terrain(Position(position.x*octave.lacunarity, position.z*octave.lacunarity), 1 - yLacunarityFlag/2)
            samples = (samples[0] + newSample[0], samples[1] + newSample[1])
            # samples: tuple[float, float] = tuple(s + n for s, n in zip(samples, newSample)) #type: ignore
        return samples


class Climate: # DoublePerlinNoise
    """A climate composed of two Perlin Noise octaves."""
    octaveA: PerlinNoiseOctave
    octaveB: PerlinNoiseOctave
    amplitude: float
    @overload
    def __init__(self, firstOctave: int, amplitudes: int, octavesToInitialize: int | None = None, *, prng: Random | None = None) -> None: ... # noise.doublePerlinInit()
    
    @overload
    def __init__(self, firstOctave: int, amplitudes: tuple[float, ...], octavesToInitialize: int | None = None, *, prng: Xoroshiro | None = None) -> None: ... # noise.xDoublePerlinInit()

    def __init__(self, firstOctave: int, amplitudes: int | tuple[float, ...], octavesToInitialize: int | None = None, *, prng: Random | Xoroshiro | None = None) -> None:
        """Initializes the climate, and optionally seeds it if `prng` is not None.
        If a length (a single integer) is provided for `amplitudes`, the climate will be treated as 1.17-; otherwise if a tuple of amplitudes is provided, the climate will be treated as 1.18+.
        Fewer than the total number of octaves can be initialized if `octavesToInitialize` is specified."""
        if octavesToInitialize: # None and 0 have the same behavior, so no need for "is not None"
            AOctavesToInitialize = (octavesToInitialize + 1) >> 1
            BOctavesToInitialize = octavesToInitialize - AOctavesToInitialize
        if isinstance(amplitudes, tuple):
            assert not isinstance(prng, Random)
            self.octaveA.__init__(amplitudes, firstOctave, AOctavesToInitialize if octavesToInitialize else None, prng=prng)
            self.octaveB.__init__(amplitudes, firstOctave, BOctavesToInitialize if octavesToInitialize else None, prng=prng)
            amplitudes = max(i for i in range(len(amplitudes)) if amplitudes[i]) - min(i for i in range(len(amplitudes)) if amplitudes[i]) + 1
        else: # isinstance(amplitudes, int)
            assert not isinstance(prng, Xoroshiro)
            self.octaveA.__init__(amplitudes, firstOctave, AOctavesToInitialize if octavesToInitialize else None, prng=prng)
            self.octaveB.__init__(amplitudes, firstOctave, BOctavesToInitialize if octavesToInitialize else None, prng=prng)
        self.amplitude = 5/3 * amplitudes/(amplitudes + 1)

    @overload
    def seed(self, prng: Random, firstOctave: int, *, beta: bool = False) -> None: ... # noise.doublePerlinInit()

    @overload
    def seed(self, prng: Xoroshiro, firstOctave: int, amplitudes: tuple[float, ...]) -> None: ... # noise.xDoublePerlinInit()

    def seed(self, prng: Random | Xoroshiro, firstOctave: int, amplitudes: tuple[float, ...] | None = None, *, beta: bool = False) -> None:
        """Seeds the climate using xoroshiro128++ without disturbing all other initialized constants."""
        if isinstance(prng, Xoroshiro):
            assert isinstance(amplitudes, tuple)
            self.octaveA.seed(prng, firstOctave, amplitudes)
            self.octaveB.seed(prng, firstOctave, amplitudes)
        elif not beta:
            self.octaveA.seed(prng, firstOctave, beta=False)
            self.octaveB.seed(prng, firstOctave, beta=False)
        else:
            self.octaveA.seed(prng, beta=True)
            self.octaveB.seed(prng, beta=True)

    def sample(self, position: Position) -> float: # noise.sampleDoublePerlin()
        """Samples the Perlin Noise octave at a 3D position."""
        return self.amplitude*(self.octaveA.sample(position) + self.octaveB.sample(Position(position.x*337/331, position.z*337/331, y=position.y*337/331)))