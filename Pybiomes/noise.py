from rng import *

class PerlinNoise:
    a: float
    b: float
    c: float
    d: list[int]
    amplitude: float = 1
    lacunarity: float = 1
    yFractionalPartAt0: float = 0
    yIntegerPartAt0: int = 0
    yLerpWeightAt0: float = 0
    def __init__(self, prng: Random | Xoroshiro, *, precomputeY: bool = True) -> None:
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
            self.yFractionalPartAt0 = self.b - self.yIntegerPartAt0
            self.yLerpWeightAt0 = self.yFractionalPartAt0*self.yFractionalPartAt0*self.yFractionalPartAt0*(self.yFractionalPartAt0*(self.yFractionalPartAt0*6 - 15) + 10)

        # self.amplitude = self.lacunarity = 1

    def sample(self, x: float, y: float, z: float, *, yamp: float = 0, ymin: float = 0) -> float:
        x += self.a
        z += self.c
        xIntegerPart, zIntegerPart = int(x//1) & EIGHT_BITS, int(z//1) & EIGHT_BITS
        aFractionalPart, cFractionalPart = x - xIntegerPart, z - zIntegerPart
        aLerpWeight = aFractionalPart*aFractionalPart*aFractionalPart*(aFractionalPart*(aFractionalPart*6 - 15) + 10)
        cLerpWeight = cFractionalPart*cFractionalPart*cFractionalPart*(cFractionalPart*(cFractionalPart*6 - 15) + 10)

        if not y:
            yIntegerPart = self.yIntegerPartAt0
            bFractionalPart = self.yFractionalPartAt0
            bLerpWeight = self.yLerpWeightAt0
        else:
            y += self.b
            yIntegerPart = int(y//1) & EIGHT_BITS
            bFractionalPart = y - yIntegerPart
            bLerpWeight = bFractionalPart*bFractionalPart*bFractionalPart*(bFractionalPart*(bFractionalPart*6 - 15) + 10)
        if yamp: bFractionalPart -= (min(ymin, bFractionalPart)//yamp)*yamp

        step01 = (self.d[xIntegerPart    ] + yIntegerPart) & EIGHT_BITS
        step02 = (self.d[xIntegerPart + 1] + yIntegerPart) & EIGHT_BITS
        step11 = (self.d[step01    ] + zIntegerPart) & EIGHT_BITS
        step12 = (self.d[step02    ] + zIntegerPart) & EIGHT_BITS
        step13 = (self.d[step01 + 1] + zIntegerPart) & EIGHT_BITS
        step14 = (self.d[step02 + 1] + zIntegerPart) & EIGHT_BITS

        return tripleLerp(indexedLerp(self.d[step11], aFractionalPart, bFractionalPart, cFractionalPart), indexedLerp(self.d[step12], aFractionalPart - 1, bFractionalPart, cFractionalPart), indexedLerp(self.d[step13], aFractionalPart, bFractionalPart - 1, cFractionalPart), indexedLerp(self.d[step14], aFractionalPart - 1, bFractionalPart - 1, cFractionalPart), indexedLerp(self.d[step11 + 1], aFractionalPart, bFractionalPart, cFractionalPart - 1), indexedLerp(self.d[step12 + 1], aFractionalPart - 1, bFractionalPart, cFractionalPart - 1), indexedLerp(self.d[step13 + 1], aFractionalPart, bFractionalPart - 1, cFractionalPart - 1), indexedLerp(self.d[step14 + 1], aFractionalPart - 1, bFractionalPart - 1, cFractionalPart - 1), aLerpWeight, bLerpWeight, cLerpWeight)

    # TODO: Unfinished
    def sample_Beta_1_7(self, x: float, z: float, v: float, yLacAmp: float) -> float:
        pass

class SimplexNoise:
    d: list[int]
    amplitude: float = 1
    lacunarity: float = 1
    def __init__(self, prng: Random | Xoroshiro) -> None:
        prng.skipN(6)
        self.d = [i for i in range(256)]
        for i in range(256):
            j = i + prng.nextInt(256 - i)
            temp = self.d[i]
            self.d[i] = self.d[j]
            self.d[j] = temp
        self.d.append(self.d[0])
    
    # TODO: Unfinished
    def _grad(index: int, x: float, y: float, z: float, d: float):
        pass

    # TODO: Unfinished
    def sample(x: float, y: float) -> float:
        pass


# TODO: Unfinished
class PerlinOctaveNoise:
    length: int
    octaves: list[PerlinNoise]


# TODO: Unfinished
class DoublePerlinOctaveNoise:
    octaveA: PerlinOctaveNoise
    octaveB: PerlinOctaveNoise
    amplitude: float