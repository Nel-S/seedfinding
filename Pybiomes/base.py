from enum import IntEnum
from math import trunc
from typing import overload, Sequence

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

def rotateLeft(n: int, bits: int, *, width: int = 64) -> int: # rng.rotl64(), rng.rotr32()
	"""Returns the unsigned left [circular rotation](https://en.wikipedia.org/wiki/Circular_shift) of `n`, as a `width`-bit integer, by `bits` bits.
	A right circular rotation can be done by calling this with a negative value for `bits` instead.
	`width` must be positive."""
	window = (1 << width) - 1
	n &= window
	bits %= width
	return ((n << bits) | (n >> (width - bits))) & window

def multiplicativeInverse(x: int, modulo: int, *, width=64) -> int: # rng.mulInv()
	"""Returns the multiplicative inverse `x`^-1 (mod `modulo`)--equivalently the value p such that `x`*p (mod `modulo`) = 1--or 0 if no such inverse exists."""
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

def lerp(lowerBound: float, upperBound: float, weight: float, *, clamp: bool = True) -> float: # rng.lerp(), rng.clampedLerp()
	"""Returns the result of one round of [linear interpolation](https://en.wikipedia.org/wiki/Linear_interpolation).
	If `weight` is in the range [0,1], or if `clamp` is true, the returned value will be in the range [`lowerBound`, `upperBound`]."""
	if clamp:
		if weight <= 0: return lowerBound
		if weight >= 1: return upperBound
	return lowerBound + weight*(upperBound - lowerBound)

def doubleLerp(lowerBound1: float, upperBound1: float, lowerBound2: float, upperBound2: float, weight1: float, weight2: float, *, clamp: bool = True) -> float: # rng.lerp2()
	"""Returns the result of two nested rounds of linear interpolation."""
	# TODO: Figure out range
	return lerp(lerp(lowerBound1, upperBound1, weight1, clamp=clamp), lerp(lowerBound2, upperBound2, weight1, clamp=clamp), weight2, clamp=clamp)

def tripleLerp(lowerBound1: float, upperBound1: float, lowerBound2: float, upperBound2: float, lowerBound3: float, upperBound3: float, lowerBound4: float, upperBound4: float, weight1: float, weight2: float, weight3: float, *, clamp: bool = True) -> float: # rng.lerp3()
	"""Returns the result of three nested rounds of linear interpolation."""
	# TODO: Figure out range
	return lerp(doubleLerp(lowerBound1, upperBound1, lowerBound2, upperBound2, weight1, weight2, clamp=clamp), doubleLerp(lowerBound3, upperBound3, lowerBound4, upperBound4, weight1, weight2, clamp=clamp), weight3, clamp=clamp)

def indexedLerp(index: int, a: float, b: float, c: float) -> float: # noise.indexedLerp()
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


class Position:
	"""A two-dimensional or three-dimensional position in space. Unlike `Coordinate`, its values can be decimals.
	`y` defaults to 0 unless explicitly specified.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D point (meaning y will be ignored)."""
	x: int | float
	y: int | float
	z: int | float
	@overload
	def __init__(self, x: int | float = 0, z: int | float = 0, *, y: int | float = 0) -> None: ...

	@overload
	def __init__(self, x: Sequence[int | float]) -> None: ...

	def __init__(self, x: int | float | Sequence[int | float] = 0, z: int | float = 0, *, y: int | float = 0) -> None:
		self.x, self.y, self.z = ((x[0], 0, x[1]) if len(x) < 3 else (x[0], x[1], x[2])) if isinstance(x, Sequence) else (x, y, z)

class Coordinate: # finders.Pos
	"""A two-dimensional or three-dimensional coordinate. Unlike `Position`, its values must be integers.
	`y` defaults to 0 unless explicitly specified.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D point (meaning y will be ignored)."""
	x: int
	y: int
	z: int
	@overload
	def __init__(self, x: int = 0, z: int = 0, *, y: int = 0) -> None: ...

	@overload
	def __init__(self, x: Sequence[int]) -> None: ...

	def __init__(self, x: int | Sequence[int] = 0, z: int = 0, *, y: int = 0) -> None:
		self.x, self.y, self.z = ((x[0], 0, x[1]) if len(x) < 3 else (x[0], x[1], x[2])) if isinstance(x, Sequence) else (x, y, z)

class Range(Coordinate): # biomenoise.Range
	"""A three-dimensional range of coordinates.
	(x, y, z) is the lower-Northwest corner of the region, and the region extends to (x + length - 1, y + height - 1, z + width - 1).
	`y` and `height` default to 0 unless explicitly specified.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D area (meaning y and height will be ignored)."""
	length: int
	width: int
	height: int
	@overload
	def __init__(self, x: int = 0, z: int = 0, length: int = 0, width: int = 0, *, y: int = 0, height: int = 0) -> None: ...

	@overload
	def __init__(self, x: Sequence[int]) -> None: ...

	def __init__(self, x: int | Sequence[int] = 0, z: int = 0, length: int = 0, width: int = 0, *, y: int = 0, height: int = 0) -> None:
		if isinstance(x, Sequence):
			super().__init__(x[0], x[1], y=0 if len(x) < 5 else x[4])
			self.length, self.width, self.height = x[2], x[3], 0 if len(x) < 6 else x[5]
		else:
			super().__init__(x, z, y=y)
			self.length, self.width, self.height = length, width, height

class Version(IntEnum): # biomes.MCVersion
	V_B1_7  = V_BETA_1_7 = 0
	V_B1_8  = V_BETA_1_8 = 1
	V1_0    = V1_0_0     = 2
	V1_1    = V1_1_0     = 3
	V1_2    = V1_2_5     = 4
	V1_3    = V1_3_2     = 5
	V1_4    = V1_4_7     = 6
	V1_5    = V1_5_2     = 7
	V1_6    = V1_6_4     = 8
	V1_7    = V1_7_10    = 9
	V1_8    = V1_8_9     = 10
	V1_9    = V1_9_4     = 11
	V1_10   = V1_10_2    = 12
	V1_11   = V1_11_2    = 13
	V1_12   = V1_12_2    = 14
	V1_13   = V1_13_2    = 15
	V1_14   = V1_14_4    = 16
	V1_15   = V1_15_2    = 17
	V1_16_1 = 18
	V1_16   = V1_16_5    = 19
	V1_17   = V1_17_1    = 20
	V1_18   = V1_18_2    = 21
	V1_19_2 = 22
	V1_19   = V1_19_4    = 23
	V1_20   = V1_20_5    = 24
	NEWEST  = V1_20
	def __format__(self) -> str: # util.mc2str()
		match self:
			case self.V_B1_7: return "Beta 1.7"
			case self.V_B1_8: return "Beta 1.8"
			case self.V1_0:   return "1.0"
			case self.V1_1:   return "1.1"
			case self.V1_2:   return "1.2"
			case self.V1_3:   return "1.3"
			case self.V1_4:   return "1.4"
			case self.V1_5:   return "1.5"
			case self.V1_6:   return "1.6"
			case self.V1_7:   return "1.7"
			case self.V1_8:   return "1.8"
			case self.V1_9:   return "1.9"
			case self.V1_10:  return "1.10"
			case self.V1_11:  return "1.11"
			case self.V1_12:  return "1.12"
			case self.V1_13:  return "1.13"
			case self.V1_14:  return "1.14"
			case self.V1_15:  return "1.15"
			case self.V1_16:  return "1.16"
			case self.V1_17:  return "1.17"
			case self.V1_18:  return "1.18"
			case self.V1_19:  return "1.19"
			case self.V1_20 | self.NEWEST: return "1.20"
		return ""

class Dimension(IntEnum): # biomes.Dimension
	OVERWORLD =  0
	NETHER    = -1
	END       =  1
	def __format__(self) -> str:
		match self:
			case self.OVERWORLD: return "The Overworld"
			case self.NETHER:    return "The Nether"
			case self.END:       return "The End"
		return ""