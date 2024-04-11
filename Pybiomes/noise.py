from rng import *
from math import sqrt

class SimplexNoise:
    # TODO: Find range
    """Gradient noise developed by Ken Perlin. Returns values in the range +-...."""
    d: list[int]
    amplitude: float
    lacunarity: float
    def __init__(self, amplitude: float, lacunarity: float, *, prng: Random | Xoroshiro | None = None) -> None:
        """Initializes the Simplex Noise generator, and optionally seeds it if `prng` is not None."""
        self.amplitude = amplitude
        self.lacunarity = lacunarity
        if prng: self.seed(prng)

    def seed(self, prng: Random | Xoroshiro) -> None:
        """Seeds the Simplex Noise generator without disturbing all other initialized constants."""
        prng.skipN(6)
        self.d = [i for i in range(256)]
        for i in range(256):
            j = i + prng.nextInt(256 - i)
            temp = self.d[i]
            self.d[i] = self.d[j]
            self.d[j] = temp
    
    def __grad(self, index: int, x: float, y: float, z: float = 0, d: float = 0.5):
        con = d - x*x - y*y - z*z
        return 0. if con <= 0 else con*con*con*con*indexedLerp(index, x, y, z)

    def sample(self, x: float, y: float) -> float:
        """Samples the Simplex Noise generator."""
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

        # return 70*(self.__grad(self.d[(self.d[hy & EIGHT_BITS] + hx) & EIGHT_BITS] % 12, x, y) + self.__grad(self.d[(self.d[(hy + offy) & EIGHT_BITS] + hx + offx) & EIGHT_BITS] % 12, x - offx + UNSKEW, y - offy + UNSKEW) + self.__grad(self.d[(self.d[(hy + 1) & EIGHT_BITS] + hx + 1) & EIGHT_BITS] % 12, x - 1 + 2*UNSKEW, y - 1 + 2*UNSKEW))
        return 70*(self.__grad(self.d[(self.d[hy & EIGHT_BITS] + hx) & EIGHT_BITS] % 12, x, y) + self.__grad(self.d[(self.d[(hy + offy) & EIGHT_BITS] + hx + offx) & EIGHT_BITS] % 12, x - offx + UNSKEW, y - offy + UNSKEW) + self.__grad(self.d[(self.d[(hy + 1) & EIGHT_BITS] + hx + 1) & EIGHT_BITS] % 12, x - 1/sqrt(3), y - 1/sqrt(3)))


class PerlinNoise:
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
    def __init__(self, amplitude: float, lacunarity: float, *, prng: Random | Xoroshiro | None = None, precomputeY: bool = True) -> None:
        """Initializes the Perlin Noise generator, and optionally seeds it if `prng` is not None.
        If the generator is seeded, the intended version is Java Beta 1.8+, and multiple samples with y=0 will be done, `precomputeY` can precompute y-related values to make sampling slightly faster."""
        self.amplitude = amplitude
        self.lacunarity = lacunarity
        if prng: self.seed(prng, precomputeY=precomputeY)


    def seed(self, prng: Random | Xoroshiro, *, precomputeY: bool = True) -> None:
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
    

    def sample(self, x: float, y: float, z: float, *, yAmplitude: float = 0, yMinimum: float = 0) -> float:
        # TODO: Figure out role of yAmplitude and yMinimum
        """Samples the Perlin Noise generator in Beta 1.8 or later versions.
        If `yAmplitude` is nonzero, the fractional part of y will be clamped during the sampling."""
        x += self.a
        z += self.c
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

    def sample_Beta_1_7_Terrain(self, x: float, z: float, yLacAmp: float) -> tuple[float, float]:
        """Samples the Perlin Noise generator for Beta 1.7 terrain.
        The output will be a tuple of exactly two samples."""
        x += self.a
        z += self.c
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
class PerlinNoiseOctave:
    octaves: tuple[PerlinNoise, ...] = tuple()
    initializedOctaves: int

    @multimethod
    def __init__(self, length: int, firstOctave: int = 0, octavesToInitialize: int | None = None, *, initialAmplitude: float | None = None, amplitudeFactor: float = 2, initialLacunarity: float | None = None, lacunarityFactor: float = 0.5, prng: Random | None = None, beta: bool = False) -> None:
        """Initializes the Perlin Noise octave for Java 1.17-, and optionally seeds it if `prng` is not None."""
        lastOctave = firstOctave + length - 1
        if not initialAmplitude: initialAmplitude = 1/((1 << length) - 1)
        if not initialLacunarity: initialLacunarity = 2.0**lastOctave
        self.initializedOctaves = 0
        for _ in range(length):
            if octavesToInitialize and self.initializedOctaves >= octavesToInitialize: break
            self.octaves += (PerlinNoise(initialAmplitude, initialLacunarity),)
            initialAmplitude *= amplitudeFactor
            initialLacunarity *= lacunarityFactor
            self.initializedOctaves += 1
        
        if prng: self.seed(prng, firstOctave, beta=beta)

    @multimethod
    def __init__(self, amplitudes: tuple[float, ...], firstOctave: int, octavesToInitialize: int | None = None, *, prng: Xoroshiro | None = None) -> None:
        """Initializes the Perlin Noise octave for Java 1.18+, and optionally seeds it if `prng` is not None."""
        initialAmplitude = (1 << (len(amplitudes) - 1))/((1 << len(amplitudes)) - 1)
        initialLacunarity = 2.0**firstOctave
        self.initializedOctaves = 0
        for amp in amplitudes:
            if octavesToInitialize and self.initializedOctaves >= octavesToInitialize: break
            if not amp: continue
            self.octaves += (PerlinNoise(amp * initialAmplitude, initialLacunarity),)
            initialAmplitude *= 2
            initialLacunarity /= 2
            self.initializedOctaves += 1
        
        if prng: self.seed(prng, firstOctave, amplitudes)

    @multimethod
    def seed(self, prng: Random, minOctave: int, *, beta: bool = False) -> None:
        """Seeds the Simplex Noise generator using java.util.Random without disturbing all other initialized constants."""
        if not beta:
            end = minOctave + self.initializedOctaves - 1
            if end: prng.skipN(-262*end)
        for i in range(self.initializedOctaves): self.octaves[i].seed(prng)
    
    @multimethod
    def seed(self, prng: Xoroshiro, amplitudes: tuple[float, ...], minOctave: int) -> None:
        """Seeds the Simplex Noise generator using xoroshiro128++ without disturbing all other initialized constants."""
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
            if amplitudes[i]: self.octaves[i].seed(Xoroshiro(xlo ^ MD5_OCTAVE_N[12 + minOctave + i][0], xhi ^ MD5_OCTAVE_N[12 + minOctave + i][1]))

    def sample(self, x: float, y: float, z: float, yAmplitude: float = 0, yMinimum: float = 0, *, ignoreY: bool = False) -> float:
        return sum(octave.amplitude*octave.sample(x*octave.lacunarity, -octave.b if ignoreY else y*octave.lacunarity, z*octave.lacunarity, yAmplitude=yAmplitude*octave.lacunarity, yMinimum=yMinimum*octave.lacunarity) for octave in self.octaves[:self.initializedOctaves])
    
    def sample_Beta_1_7_Biomes(self, x: float, z: float) -> float:
        # return sum(octave.amplitude*octave.sample(x*octave.lacunarity + octave.a, z*octave.lacunarity + octave.b) for octave in self.octaves[:self.initializedOctaves])
        sample = 0
        for octave in self.octaves[:self.initializedOctaves]:
            sn = SimplexNoise(octave.amplitude, octave.lacunarity)
            sn.d = octave.d
            sample += sn.sample(x*octave.lacunarity + octave.a, z*octave.lacunarity + octave.b)
        return sample

    def sample_Beta_1_7_Terrain(self, x: float, z: float, minimumLacunarity: float = 0, *, yLacunarityFlag: bool) -> tuple[float, float]:
        samples = (0., 0.)
        for octave in self.octaves[:self.initializedOctaves]:
            if minimumLacunarity and octave.lacunarity > minimumLacunarity: continue
            newSample = octave.sample_Beta_1_7_Terrain(x*octave.lacunarity, z*octave.lacunarity, 1 - yLacunarityFlag/2)
            samples = (samples[0] + newSample[0], samples[1] + newSample[1])
            # samples: tuple[float, float] = tuple(s + n for s, n in zip(samples, newSample)) #type: ignore
        return samples


# TODO: Unfinished
class Climate:
    octaveA: PerlinNoiseOctave
    octaveB: PerlinNoiseOctave
    amplitude: float
    @multimethod
    def __init__(self, length: int, firstOctave: int, *, prng: Random | None = None) -> None:
        self.octaveA.__init__(length, firstOctave, prng=prng)
        self.octaveB.__init__(length, firstOctave, prng=prng)
        self.amplitude = 5/3 * length/(length + 1)
