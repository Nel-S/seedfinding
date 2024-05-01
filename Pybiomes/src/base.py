from enum import IntEnum
from math import trunc
from typing import overload, Sequence

# Constants that, when ANDed with a value, reduce the value to the listed number of bits.
FOUR_BITS                     = 0xf
EIGHT_BITS                    = 0xff
SIXTEEN_BITS                  = 0xffff
THIRTY_TWO_BITS               = 0xffffffff
FORTY_EIGHT_BITS              = 0xffffffffffff
SIXTY_FOUR_BITS               = 0xffffffffffffffff
ONE_HUNDRED_TWENTY_EIGHT_BITS = 0xffffffffffffffffffffffffffffffff
FLOAT_LENGTH                  = 0x1000000
DOUBLE_LENGTH                 = 0x20000000000000

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
	Providing a negative value for `bits` causes a right circular rotation to be done instead.
	`width` must be positive."""
	window = (1 << width) - 1
	n &= window
	bits %= width
	return ((n << bits) | (n >> (width - bits))) & window

def rotateRight(n: int, bits: int, *, width: int = 64) -> int:
	"""Returns the unsigned right [circular rotation](https://en.wikipedia.org/wiki/Circular_shift) of `n`, as a `width`-bit integer, by `bits` bits.
	Providing a negative value for `bits` causes a left circular rotation to be done instead.
	`width` must be positive."""
	return rotateLeft(n, -bits, width=width)

def multiplicativeInverse(x: int, modulo: int, *, width: int = 64) -> int | None: # rng.mulInv()
	"""Returns the unsigned multiplicative inverse `x`^-1 (mod `modulo`)--equivalently the value p such that `x`*p (mod `modulo`) = 1--or None if no such inverse exists."""
	if toSigned(modulo, width=width) <= 1: return None
	originalModulo, a, b = modulo, 0, 1
	x = toUnsigned(x, width=width)
	while toSigned(x, width=width) > 1:
		if not modulo: return None
		temp = a
		a = b - a*trunc(x/modulo)
		b = temp
		temp = modulo
		modulo = x % modulo
		x = temp
	if toSigned(b, width=width) < 0: b += originalModulo
	return toUnsigned(b, width=width)


@overload
def lerp(leftBound: float, rightBound: float, weight: float, *, clamp: bool = True) -> float:
	"""Returns the result of one round of [linear interpolation](https://en.wikipedia.org/wiki/Linear_interpolation), averaging `leftBound` and `rightBound`.
	If `weight` is in the range [0,1], or if `clamp` is true, the returned value will be in the range [min(`leftBound`, `rightBound`), max(`leftBound`, `rightBound`)].
	
	Note that the order of arguments differs from Cubiomes' implementation."""
	...

@overload
def lerp(leftBound: Sequence[float], rightBound: Sequence[float], weight: Sequence[float], *, clamp: bool = True) -> float:
	"""Returns the result of potentially multiple rounds of [linear interpolation](https://en.wikipedia.org/wiki/Linear_interpolation), averaging all values in `leftBound` and `rightBound`.
	If all values in `weight` are in the range [0,1], or if `clamp` is true, the returned value will be in the range [min(min(`leftBound`), min(`rightBound`)), max(max(`leftBound`), max(`rightBound`))].
	Note that n rounds of linear interpolation requires n weights, 2^(n - 1) left bounds, and 2^(n - 1) right bounds."""
	...

def lerp(leftBound: float | Sequence[float], rightBound: float | Sequence[float], weight: float | Sequence[float], *, clamp: bool = True) -> float: # rng.lerp(), rng.clampedLerp()
	if isinstance(leftBound, (float, int)):
		if not isinstance(rightBound, (float, int)) or not isinstance(weight, (float, int)): raise TypeError("Sequences and singular values cannot be mixed.")
		if clamp:
			if weight <= 0: return leftBound
			if weight >= 1: return rightBound
		return leftBound + weight*(rightBound - leftBound)
	else:
		if not isinstance(rightBound, Sequence) or not isinstance(weight, Sequence): raise TypeError("Sequences and singular values cannot be mixed.")
		if len(leftBound) != len(rightBound): raise ValueError("Left and right bounds must have equal lengths.")
		if not len(leftBound) or (len(leftBound) & (len(leftBound) - 1)): raise ValueError("Left and right bounds must have a nonzero length that is a power of two.")
		if len(leftBound) != (1 << (len(weight) - 1)): raise ValueError("Linearly interpolating 2^k left/right bounds requires exactly k + 1 weights.")

		if clamp: weight = [min(max(w, 0), 1) for w in weight]
		combined = [x for tup in zip(leftBound, rightBound, strict=True) for x in tup]
		for w in weight: combined = [combined[i] + w*(combined[i + 1] - combined[i]) for i in range(0, len(combined), 2)]
		return combined[0]


def doubleLerp(leftBound1: float, rightBound1: float, leftBound2: float, rightBound2: float, weight1: float, weight2: float, *, clamp: bool = True) -> float: # rng.lerp2()
	"""Returns the result of two nested rounds of linear interpolation.
	Equivalent to lerp((`leftBound1`, `leftBound2`), (`rightBound1`, `rightBound2`), (`weight1`, `weight2`), clamp=`clamp`)."""
	# TODO: Figure out range
	return lerp(lerp(leftBound1, rightBound1, weight1, clamp=clamp), lerp(leftBound2, rightBound2, weight1, clamp=clamp), weight2, clamp=clamp)

def tripleLerp(leftBound1: float, rightBound1: float, leftBound2: float, rightBound2: float, leftBound3: float, rightBound3: float, leftBound4: float, rightBound4: float, weight1: float, weight2: float, weight3: float, *, clamp: bool = True) -> float: # rng.lerp3()
	"""Returns the result of three nested rounds of linear interpolation.
	Equivalent to lerp((`leftBound1`, ..., `leftBound4`), (`rightBound1`, ..., `rightBound4`), (`weight1`, ..., `weight3`), clamp=`clamp`)."""
	# TODO: Figure out range
	return lerp(doubleLerp(leftBound1, rightBound1, leftBound2, rightBound2, weight1, weight2, clamp=clamp), doubleLerp(leftBound3, rightBound3, leftBound4, rightBound4, weight1, weight2, clamp=clamp), weight3, clamp=clamp)

def indexedLerp(index: int, a: float, b: float, c: float) -> float: # noise.indexedLerp()
	"""Returns two of the three values `a`, `b`, or `c` added to or subtracted from one another, depending on the value of `index`."""
	match index & FOUR_BITS:
		case 0  | 12: return  a + b
		case 1  | 14: return -a + b
		case 2      : return  a - b
		case 3      : return -a - b
		case 4      : return  a + c
		case 5      : return -a + c
		case 6      : return  a - c
		case 7      : return -a - c
		case 8      : return  b + c
		case 9  | 13: return -b + c
		case 10     : return  b - c
		case 11 | 15: return -b - c
	return 0


class Coordinate(Sequence): # finders.Pos
	"""A two- or three-dimensional coordinate. Unlike `Position`, its values must be integers.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D point (meaning y will be ignored)."""
	x: int
	y: int
	z: int
	@overload
	def __init__(self, x: int = 0, z: int = 0, /) -> None:
		"""Initializes a two-dimensional coordinate from the provided parameters."""
		...

	@overload
	def __init__(self, x: int = 0, y: int = 0, z: int = 0, /) -> None:
		"""Initializes a three-dimensional coordinate from the provided parameters."""
		...

	@overload
	def __init__(self, positions: Sequence[int], /) -> None:
		"""Initializes a two- (if `positions` is of length two) or three-dimensional coordinate from the provided sequence's values.
		The values will be evaluated in the order (`x`, `z`) if `positions` is of length two or (`x`, `y`, `z`) otherwise."""
		...

	def __init__(self, arg1: int | Sequence[int] = 0, arg2: int = 0, arg3: int | None = None) -> None:
		if isinstance(arg1, int):
			self.x, self.y, self.z = arg1, 0 if arg3 is None else arg3, arg2
			self.dimension = 2 + (arg3 is not None)
		else: # isinstance(arg1, Sequence)
			if len(arg1) < 2: raise ValueError(f"Sequence has too few values to initialize a range (needs 2, found {len(arg1)}).")
			self.x, self.y, self.z = (arg1[0],) + ((0, arg1[1]) if len(arg1) < 3 else (arg1[1], arg1[2]))
			self.dimension = 2 + (len(arg1) >= 3)
		self.__iteratorIndex = 0

	def __format__(self) -> str:
		"""Pretty-print the coordinate."""
		return f"({self.x}, {str(self.y) + ', ' if self.dimension == 3 else ''}{self.z}"

	def __getitem__(self, index: int) -> int:
		match index:
			case 0: return self.x
			case 1: return self.y if self.dimension == 2 else self.z
			case 2 if self.dimension == 3: return self.z
			case -3 if self.dimension == 3: return self.x
			case -2: return self.x if self.dimension == 2 else self.y
			case -1: return self.z
		raise IndexError

	def __iter__(self):
		return self

	def __len__(self) -> int:
		return self.dimension

	def __next__(self) -> int | float:
		try:
			out = self[self.__iteratorIndex]
			self.__iteratorIndex += 1
			return out
		except IndexError:
			self.__iteratorIndex = 0
			raise StopIteration


class Position(Coordinate):
	"""A two- or three-dimensional position in space. Unlike `Coordinate`, its values can be decimals.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D point (meaning y will be ignored)."""
	x: int | float
	y: int | float
	z: int | float
	@overload
	def __init__(self, x: int | float = 0, z: int | float = 0, /) -> None:
		"""Initializes a two-dimensional position in space from the provided parameters."""
		...

	@overload
	def __init__(self, x: int | float = 0, y: int | float = 0, z: int | float = 0, /) -> None:
		"""Initializes a three-dimensional position in space from the provided parameters."""
		...

	@overload
	def __init__(self, coordinates: Sequence[int | float], /) -> None:
		"""Initializes a two- (if `positions` is of length two) or three-dimensional position in space from the provided sequence's values.
		The values will be evaluated in the order (`x`, `z`) if `positions` is of length two or (`x`, `y`, `z`) otherwise."""
		...

	def __init__(self, arg1: int | float | Sequence[int | float] = 0, arg2: int | float = 0, arg3: int | float | None = None) -> None:
		if isinstance(arg1, (int, float)):
			self.x, self.y, self.z = arg1, 0 if arg3 is None else arg3, arg2
			self.dimension = 2 + (arg3 is not None)
		else: # isinstance(arg1, Sequence)
			if len(arg1) < 2: raise ValueError(f"Sequence has too few values to initialize a range (needs 2, found {len(arg1)}).")
			self.x, self.y, self.z = (arg1[0],) + ((0, arg1[1]) if len(arg1) < 3 else (arg1[1], arg1[2]))
			self.dimension = 2 + (len(arg1) >= 3)
		self.__iteratorIndex = 0


class Range(Coordinate): # biomenoise.Range
	"""A two- or three-dimensional range of coordinates.
	(x, y, z) is the lower-Northwest corner of the region, and the region extends to (x + length - 1, y + height - 1, z + width - 1).
	`length`, `width`, and `height` must be positive.
	All functions in this library using this will explicitly specify in their description if all three dimensions will be used, or if the object will be taken as a 2D area (meaning y and height will be ignored)."""
	length: int
	height: int
	width: int
	scale: int
	__iteratorIndex: int
	@overload
	def __init__(self, x: int = 0, z: int = 0, length: int = 1, width: int = 1, /, *, scale: int = 1) -> None:
		"""Initializes a two-dimensional range from the provided parameters.
		The range will extend from (`x`, `z`) to (`x` + `length` - 1, `z` + `width` - 1) inclusive."""
		...

	@overload
	def __init__(self, x: int = 0, y: int = 0, z: int = 0, length: int = 1, height: int = 1, width: int = 1, /, *, scale: int = 1) -> None:
		"""Initializes a three-dimensional range from the provided parameters.
		The range will extend from (`x`, `y`, `z`) to (`x` + `length` - 1, `y` + `height` - 1, `z` + `width` - 1) inclusive."""
		...

	@overload
	def __init__(self, x: Sequence[int], /) -> None:
		"""Initializes a two- (if `y` and `height` are omitted) or three-dimensional range from the provided sequence.
		The range will extend from (`x`, `y`, `z`) to (`x` + `length` - 1, `y` + `height` - 1, `z` + `width` - 1) inclusive.
		Note that due to `y`'s and `height`'s optionalness, the values will be evaluated in the order `x`, `z`, `y=y`, *not* `x`, `y`, `z` or `x`, `y=y`, `z`."""
		...

	@overload
	def __init__(self, coord1: Coordinate, coord2: Coordinate, /, *, scale: int = 1) -> None:
		"""Initializes a two- (if the coordinates' `y`s are unspecified) or three-dimensional range extending from the coordinate `coord1` to the coordinate `coord2`, inclusive."""
		...

	def __init__(self, arg1: int | Sequence[int] = 0, arg2: int | Coordinate = 0, arg3: int = 0, arg4: int = 0, arg5: int | None = None, arg6: int | None = None, *, scale: int = 1) -> None:
		if isinstance(arg1, int):
			if not isinstance(arg2, int): raise TypeError("Coordinates and singular values cannot be mixed.")
			super().__init__(arg1, arg2) if arg6 is None else super().__init__(arg1, arg2, arg3)
			self.length, self.width, self.height = (arg3, arg4, 1) if arg5 is None or arg6 is None else (arg4, arg5, arg6)
			self.scale = scale
		elif isinstance(arg1, Coordinate):
			if not isinstance(arg2, Coordinate): raise TypeError("Coordinates and singular values cannot be mixed.")
			super().__init__(max(arg1.x, arg2.x), max(arg1.z, arg2.z)) if max(arg1.dimension, arg2.dimension) == 2 else super().__init__(max(arg1.x, arg2.x), max(arg1.y, arg2.y), max(arg1.z, arg2.z))
			self.length, self.height, self.width = abs(arg1.x - arg2.x) + 1, abs(arg1.y - arg2.y) + 1, abs(arg1.z - arg2.z) + 1
			self.scale = scale
		else: # isinstance(arg1, Sequence)
			if len(arg1) < 4: raise ValueError("Sequence has too few values to initialize a range.")
			super().__init__(arg1[0], arg1[1]) if len(arg1) <= 5 else super().__init__(arg1[0], arg1[1], arg1[2])
			self.length, self.height, self.width = (arg1[2], 1, arg1[3]) if len(arg1) <= 5 else (arg1[3], arg1[5], arg1[4])
			self.scale = 1 if len(arg1) in (4, 6) else arg1[4] if len(arg1) == 5 else arg1[6]
		
		if min(self.length, self.height, self.width, self.scale) <= 0: raise ValueError(f"length ({self.length}), width ({self.width}), height ({self.height}), and scale ({self.scale}) must all be positive.")

	def __format__(self) -> str:
		"""Pretty-print the range."""
		px = self.x + self.length - 1
		pz = self.z + self.width - 1
		return f"[{Coordinate(self.x, self.z)} - {Coordinate(px, pz)}]" if self.dimension == 2 else f"[{Coordinate(self.x, self.y, self.z)} - {Coordinate(px, self.y + self.height - 1, pz)}]"

	def __getitem__(self, index: int) -> int:
		raise NotImplementedError

	def __iter__(self):
		return self
	
	def __len__(self) -> int:
		raise NotImplementedError

	def __next__(self) -> Coordinate:
		"""Returns the next coordinate in the range, iterating over first the z-axis, then the x-axis, then the y-axis."""
		if self.__iteratorIndex < self.length*self.width*self.height:
			x = self.x + ((self.__iteratorIndex//self.width) % self.length)
			z = self.z + (self.__iteratorIndex % self.width)
			out = Coordinate(x, z) if self.dimension == 2 else Coordinate(x, self.y + (self.__iteratorIndex//(self.length*self.width)), z)
			self.__iteratorIndex += 1
			return out
		else:
			self.__iteratorIndex = 0
			raise StopIteration

		

class FixedSpline: # biomenoise.FixSpline
	class IDs(IntEnum):
		CONTINENTALNESS = 0 # biomenoise.SP_CONTINENTALNESS
		EROSION = 1 # biomenoise.SP_EROSION
		RIDGES = 2 # biomenoise.SP_RIDGES
		WEIRDNESS = 3 # biomenoise.SP_WEIRDNESS
	length: int # FixSpline.len
	values: float # FixSpline.val
	...

class Spline(FixedSpline): # biomenoise.Spline
	# length: int # Spline.len
	id: FixedSpline.IDs # Spline.typ
	loc: list[float]
	der: list[float]
	values: list["Spline"] # Spline.val
	...

class SplineStack: # biomenoise.SplineStack
	splines: list[Spline] # SplineStack.stack
	fixedSplines: list[FixedSpline] # SplineStack.fstack
	...


class Platform(IntEnum):
	"""A list of Minecraft platforms."""
	JAVA    = 0
	BEDROCK = 1

class Version(IntEnum): # biomes.MCVersion
	"""A list of supported Minecraft: Java Edition versions."""
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
	"""A list of Minecraft dimensions."""
	OVERWORLD =  0
	NETHER    = -1
	END       =  1
	def __format__(self) -> str:
		match self:
			case self.OVERWORLD: return "The Overworld"
			case self.NETHER:    return "The Nether"
			case self.END:       return "The End"
		return ""