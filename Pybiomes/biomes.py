from enum import IntEnum

class Version(IntEnum):
	V_B1_7 = V_BETA_1_7 = 0
	V_B1_8 = V_BETA_1_8 = 1
	V1_0  = V1_0_0  = 2
	V1_1  = V1_1_0  = 3
	V1_2  = V1_2_5  = 4
	V1_3  = V1_3_2  = 5
	V1_4  = V1_4_7  = 6
	V1_5  = V1_5_2  = 7
	V1_6  = V1_6_4  = 8
	V1_7  = V1_7_10 = 9
	V1_8  = V1_8_9  = 10
	V1_9  = V1_9_4  = 11
	V1_10 = V1_10_2 = 12
	V1_11 = V1_11_2 = 13
	V1_12 = V1_12_2 = 14
	V1_13 = V1_13_2 = 15
	V1_14 = V1_14_4 = 16
	V1_15 = V1_15_2 = 17
	V1_16_1 = 18
	V1_16 = V1_16_5 = 19
	V1_17 = V1_17_1 = 20
	V1_18 = V1_18_2 = 21
	V1_19_2 = 22
	V1_19 = V1_19_4 = 23
	V1_20 = 24
	NEWEST = V1_20

class Dimension(IntEnum):
	NETHER    = -1
	OVERWORLD =  0
	END       =  1

class Biome(IntEnum):
	OCEAN = 0
	PLAINS = 1
	DESERT = 2
	MOUNTAINS = EXTREME_HILLS = WINDSWEPT_HILLS = 3
	FOREST = 4
	TAIGA = 5
	SWAMP = SWAMPLAND = 6
	RIVER = 7
	NETHER_WASTES = HELL = 8
	THE_END = SKY = 9
	FROZEN_OCEAN = 10
	FROZEN_RIVER = 11
	SNOWY_TUNDRA = ICE_PLAINS = SNOWY_PLAINS = 12
	SNOWY_MOUNTAINS = ICE_MOUNTAINS = 13
	MUSHROOM_FIELDS = MUSHROOM_ISLAND = 14
	MUSHROOM_FIELD_SHORE = MUSHROOM_ISLAND_SHORE = 15
	BEACH = 16
	DESERT_HILLS = 17
	WOODED_HILLS = FOREST_HILLS = 18
	TAIGA_HILLS = 19
	MOUNTAIN_EDGE = EXTREME_HILLS_EDGE = 20
	JUNGLE = 21
	JUNGLE_HILLS = 22
	JUNGLE_EDGE = SPARSE_JUNGLE = 23
	DEEP_OCEAN = 24
	STONE_SHORE = STONE_BEACH = STONY_SHORE = 25
	SNOWY_BEACH = COLD_BEACH = 26
	BIRCH_FOREST = 27
	BIRCH_FOREST_HILLS = 28
	DARK_FOREST = ROOFED_FOREST = 29
	SNOWY_TAIGA = COLD_TAIGA = 30
	SNOWY_TAIGA_HILLS = COLD_TAIGA_HILLS = 31
	GIANT_TREE_TAIGA = MEGA_TAIGA = OLD_GROWTH_PINE_TAIGA = 32
	GIANT_TREE_TAIGA_HILLS = MEGA_TAIGA_HILLS = 33
	WOODED_MOUNTAINS = EXTREME_HILLS_PLUS = WINDSWEPT_FOREST = 34
	SAVANNA = 35
	SAVANNA_PLATEAU = 36
	BADLANDS = MESA = 37
	WOODED_BADLANDS = WOODED_BADLANDS_PLATEAU = MESA_PLATEAU_F = 38
	BADLANDS_PLATEAU = MESA_PLATEAU = 39
	SMALL_END_ISLANDS = 40
	END_MIDLANDS = 41
	END_HIGHLANDS = 42
	END_BARRENS = 43
	WARM_OCEAN = 44
	LUKEWARM_OCEAN = 45
	COLD_OCEAN = 46
	DEEP_WARM_OCEAN = 47
	DEEP_LUKEWARM_OCEAN = 48
	DEEP_COLD_OCEAN = 49
	DEEP_FROZEN_OCEAN = 50
	SEASONAL_FOREST = 51
	RAINFOREST = 52
	SHRUBLAND = 53
	THE_VOID = 127
	SUNFLOWER_PLAINS = PLAINS + 128
	DESERT_LAKES = DESERT + 128
	GRAVELLY_MOUNTAINS = WINDSWEPT_GRAVELLY_HILLS = MOUNTAINS + 128
	FLOWER_FOREST = FOREST + 128
	TAIGA_MOUNTAINS = TAIGA + 128
	SWAMP_HILLS = SWAMP + 128
	ICE_SPIKES = SNOWY_TUNDRA + 128
	MODIFIED_JUNGLE = JUNGLE + 128
	MODIFIED_JUNGLE_EDGE = JUNGLE_EDGE + 128
	TALL_BIRCH_FOREST = OLD_GROWTH_BIRCH_FOREST = BIRCH_FOREST + 128
	TALL_BIRCH_HILLS = BIRCH_FOREST_HILLS + 128
	DARK_FOREST_HILLS = DARK_FOREST + 128
	SNOWY_TAIGA_MOUNTAINS = SNOWY_TAIGA + 128
	GIANT_SPRUCE_TAIGA = OLD_GROWTH_SPRUCE_TAIGA = GIANT_TREE_TAIGA + 128
	GIANT_SPRUCE_TAIGA_HILLS = GIANT_TREE_TAIGA_HILLS + 128
	MODIFIED_GRAVELLY_MOUNTAINS = WOODED_MOUNTAINS + 128
	SHATTERED_SAVANNA = WINDSWEPT_SAVANNA = SAVANNA + 128
	SHATTERED_SAVANNA_PLATEAU = SAVANNA_PLATEAU + 128
	ERODED_BADLANDS = BADLANDS + 128
	MODIFIED_WOODED_BADLANDS_PLATEAU = WOODED_BADLANDS + 128
	MODIFIED_BADLANDS_PLATEAU = BADLANDS_PLATEAU + 128
	BAMBOO_JUNGLE = 168
	BAMBOO_JUNGLE_HILLS = 169
	SOUL_SAND_VALLEY = 170
	CRIMSON_FOREST = 171
	WARPED_FOREST = 172
	BASALT_DELTAS = 173
	DRIPSTONE_CAVES = 174
	LUSH_CAVES = 175
	MEADOW = 177
	GROVE = 178
	SNOWY_SLOPES = 179
	JAGGED_PEAKS = 180
	FROZEN_PEAKS = 181
	STONY_PEAKS = 182
	DEEP_DARK = 183
	MANGROVE_SWAMP = 184
	CHERRY_GROVE = 185
	
	@classmethod
	def existsInVersion(cls, biome: "Biome", version: Version) -> bool:
		"""Returns whether a specified `biome` existed in a specified `version`."""
		match biome:
			# Beta 1.7
			case cls.RAINFOREST | cls.SEASONAL_FOREST | cls.SHRUBLAND: return Version.V_BETA_1_7 == version
			case cls.DESERT | cls.FOREST | cls.OCEAN | cls.PLAINS | cls.TAIGA: return Version.V_BETA_1_7 <= version
			case cls.FROZEN_OCEAN | cls.SNOWY_TUNDRA: return Version.V_BETA_1_7 == version or Version.V1_0 <= version
			case cls.SAVANNA: return Version.V_BETA_1_7 == version or Version.V1_7 <= version
			# Beta 1.8
			case cls.MOUNTAINS | cls.NETHER_WASTES | cls.RIVER | cls.SWAMP: return Version.V_BETA_1_8 <= version
			# 1.0
			case cls.FROZEN_RIVER | cls.MUSHROOM_FIELDS | cls.THE_END: return Version.V1_0 <= version
			case cls.MUSHROOM_FIELD_SHORE: return Version.V1_0 <= version <= Version.V1_17
			# 1.1
			case cls.BEACH: return Version.V1_1 <= version
			case cls.DESERT_HILLS | cls.MOUNTAIN_EDGE | cls.SNOWY_MOUNTAINS | cls.TAIGA_HILLS | cls.WOODED_HILLS: return Version.V1_1 <= version <= Version.V1_17
			# 1.2
			case cls.JUNGLE: return Version.V1_2 <= version
			case cls.JUNGLE_HILLS: return Version.V1_2 <= version <= Version.V1_17
			# 1.7
			case cls.BADLANDS | cls.BIRCH_FOREST | cls.DARK_FOREST | cls.DEEP_OCEAN | cls.ERODED_BADLANDS | cls.FLOWER_FOREST | cls.GIANT_SPRUCE_TAIGA | cls.GIANT_TREE_TAIGA | cls.GRAVELLY_MOUNTAINS | cls.ICE_SPIKES | cls.JUNGLE_EDGE | cls.SAVANNA_PLATEAU | cls.SHATTERED_SAVANNA | cls.SNOWY_BEACH | cls.SNOWY_TAIGA | cls.STONE_SHORE | cls.SUNFLOWER_PLAINS | cls.TALL_BIRCH_FOREST | cls.WOODED_BADLANDS | cls.WOODED_MOUNTAINS: return Version.V1_7 <= version
			case cls.BADLANDS_PLATEAU | cls.BIRCH_FOREST_HILLS | cls.DARK_FOREST_HILLS | cls.DESERT_LAKES | cls.GIANT_SPRUCE_TAIGA_HILLS | cls.GIANT_TREE_TAIGA_HILLS | cls.MODIFIED_BADLANDS_PLATEAU | cls.MODIFIED_GRAVELLY_MOUNTAINS | cls.MODIFIED_JUNGLE | cls.MODIFIED_JUNGLE_EDGE | cls.MODIFIED_WOODED_BADLANDS_PLATEAU | cls.SHATTERED_SAVANNA_PLATEAU | cls.SNOWY_TAIGA_HILLS | cls.SNOWY_TAIGA_MOUNTAINS | cls.SWAMP_HILLS | cls.TAIGA_MOUNTAINS | cls.TALL_BIRCH_HILLS: return Version.V1_7 <= version <= Version.V1_17
			# 1.9
			case cls.END_BARRENS | cls.END_HIGHLANDS | cls.END_MIDLANDS | cls.SMALL_END_ISLANDS: return Version.V1_9 <= version
			case cls.THE_VOID: return Version.V1_9 <= version <= Version.V1_17
			# 1.13
			case cls.COLD_OCEAN | cls.DEEP_COLD_OCEAN | cls.DEEP_FROZEN_OCEAN | cls.DEEP_LUKEWARM_OCEAN | cls.DEEP_WARM_OCEAN | cls.LUKEWARM_OCEAN | cls.WARM_OCEAN: return Version.V1_13 <= version
			# 1.14
			case cls.BAMBOO_JUNGLE: return Version.V1_14 <= version
			case cls.BAMBOO_JUNGLE_HILLS: return Version.V1_14 <= version <= Version.V1_17
			# 1.16
			case cls.BASALT_DELTAS | cls.CRIMSON_FOREST | cls.SOUL_SAND_VALLEY | cls.WARPED_FOREST: return version >= Version.V1_16_1
			# 1.17
			case cls.DRIPSTONE_CAVES | cls.LUSH_CAVES: return Version.V1_17 <= version
			# 1.18
			case cls.FROZEN_PEAKS | cls.GROVE | cls.JAGGED_PEAKS | cls.MEADOW | cls.SNOWY_SLOPES | cls.STONY_PEAKS: return version >= Version.V1_18
			# 1.19
			case cls.DEEP_DARK | cls.MANGROVE_SWAMP: return version >= Version.V1_19_2
			# 1.20
			case cls.CHERRY_GROVE: return version >= Version.V1_20
		return False

	@classmethod
	def isOverworld(cls, biome: "Biome", version: Version) -> bool:
		# TODO: Verify description
		"""Returns whether a specified `version` classified a specified `biome` as existing in the Overworld. Note that this is from the viewpoint of processes such as version-specific stronghold and mineshaft generation, so this does not return the same results as `getDimension(biome)`."""
		if not cls.existsInVersion(biome, version): return False
		match biome:
			case cls.BASALT_DELTAS | cls.CRIMSON_FOREST | cls.DEEP_WARM_OCEAN | cls.END_BARRENS | cls.END_HIGHLANDS | cls.END_MIDLANDS | cls.NETHER_WASTES | cls.SOUL_SAND_VALLEY | cls.SMALL_END_ISLANDS | cls.THE_END | cls.THE_VOID | cls.WARPED_FOREST: return False
			case cls.MOUNTAIN_EDGE: return version <= Version.V1_6
			case cls.FROZEN_OCEAN: return version <= Version.V1_6 or Version.V1_13 <= version
			case cls.TALL_BIRCH_FOREST: return version <= Version.V1_8 or Version.V1_11 <= version
			case cls.DRIPSTONE_CAVES | cls.LUSH_CAVES: return Version.V1_18 <= version
		return True

	@classmethod
	def getDimension(cls, biome: "Biome") -> Dimension | None:
		"""Returns the dimension a specified `biome` exists within."""
		# match biome:
		# 	# TODO: Create pull request for Cubiomes for UNDEFINED case
		# 	case None: return None
		# 	case cls.BASALT_DELTAS | cls.CRIMSON_FOREST | cls.NETHER_WASTES | cls.SOUL_SAND_VALLEY | cls.WARPED_FOREST: return Dimension.NETHER
		# 	case cls.END_BARRENS | cls.END_HIGHLANDS | cls.END_MIDLANDS | cls.SMALL_END_ISLANDS | cls.THE_END: return Dimension.END
		# 	case _: return Dimension.OVERWORLD
		match cls.getCategory(biome):
			case None: return None
			case cls.NETHER_WASTES: return Dimension.NETHER
			case cls.THE_END: return Dimension.END
		return Dimension.OVERWORLD

	@classmethod
	def getMutatedBiome(cls, biome: "Biome", version: Version | None = None) -> "Biome | None":
		"""Returns a `biome`'s corresponding mutation. Note that this feature only existed between versions 1.7-1.17 (inclusive)."""
		match biome:
			case cls.BADLANDS: return cls.ERODED_BADLANDS
			case cls.BADLANDS_PLATEAU: return cls.MODIFIED_BADLANDS_PLATEAU
			# Emulation of [MC-98995](https://bugs.mojang.com/browse/MC-98995)
			case cls.BIRCH_FOREST: return cls.TALL_BIRCH_HILLS if version is not None and Version.V1_9 <= version <= Version.V1_10 else cls.TALL_BIRCH_FOREST
			case cls.BIRCH_FOREST_HILLS: return None if version is not None and Version.V1_9 <= version <= Version.V1_10 else cls.TALL_BIRCH_HILLS
			case cls.DARK_FOREST: return cls.DARK_FOREST_HILLS
			case cls.DESERT: return cls.DESERT_LAKES
			case cls.FOREST: return cls.FLOWER_FOREST
			case cls.GIANT_TREE_TAIGA: return cls.GIANT_SPRUCE_TAIGA
			case cls.GIANT_TREE_TAIGA_HILLS: return cls.GIANT_SPRUCE_TAIGA_HILLS
			case cls.JUNGLE: return cls.MODIFIED_JUNGLE
			case cls.JUNGLE_EDGE: return cls.MODIFIED_JUNGLE_EDGE
			case cls.MOUNTAINS: return cls.GRAVELLY_MOUNTAINS
			case cls.PLAINS: return cls.SUNFLOWER_PLAINS
			case cls.SAVANNA: return cls.SHATTERED_SAVANNA
			case cls.SAVANNA_PLATEAU: return cls.SHATTERED_SAVANNA_PLATEAU
			case cls.SNOWY_TAIGA: return cls.SNOWY_TAIGA_MOUNTAINS
			case cls.SNOWY_TUNDRA: return cls.ICE_SPIKES
			case cls.SWAMP: return cls.SWAMP_HILLS
			case cls.TAIGA: return cls.TAIGA_MOUNTAINS
			case cls.WOODED_BADLANDS: return cls.MODIFIED_WOODED_BADLANDS_PLATEAU
			case cls.WOODED_MOUNTAINS: return cls.MODIFIED_GRAVELLY_MOUNTAINS
		return None

	# TODO: Unfinished
	@classmethod
	def getCategory(cls, biome: "Biome", version: Version | None = None) -> "Biome | None":
		"""Returns a `biome`'s general biome category."""
		match biome:
			case cls.BADLANDS | cls.ERODED_BADLANDS | cls.MODIFIED_BADLANDS_PLATEAU | cls.MODIFIED_WOODED_BADLANDS_PLATEAU: return cls.MESA
			case cls.BADLANDS_PLATEAU | cls.WOODED_BADLANDS: return cls.MESA if version is not None and version <= Version.V1_15 else cls.BADLANDS_PLATEAU
			case cls.BAMBOO_JUNGLE | cls.BAMBOO_JUNGLE_HILLS | cls.JUNGLE | cls.JUNGLE_EDGE | cls.JUNGLE_HILLS | cls.MODIFIED_JUNGLE | cls.MODIFIED_JUNGLE_EDGE: return cls.JUNGLE
			case cls.BEACH | cls.SNOWY_BEACH: return cls.BEACH
			case cls.BASALT_DELTAS | cls.CRIMSON_FOREST | cls.NETHER_WASTES | cls.SOUL_SAND_VALLEY | cls.WARPED_FOREST: return cls.NETHER_WASTES
			case cls.BIRCH_FOREST | cls.BIRCH_FOREST_HILLS | cls.DARK_FOREST | cls.DARK_FOREST_HILLS | cls.FLOWER_FOREST | cls.FOREST | cls.TALL_BIRCH_FOREST | cls.TALL_BIRCH_HILLS | cls.WOODED_HILLS: return cls.FOREST
			case cls.COLD_OCEAN | cls.DEEP_COLD_OCEAN | cls.DEEP_FROZEN_OCEAN | cls.DEEP_LUKEWARM_OCEAN | cls.DEEP_OCEAN | cls.DEEP_WARM_OCEAN | cls.FROZEN_OCEAN | cls.LUKEWARM_OCEAN | cls.OCEAN | cls.WARM_OCEAN: return cls.OCEAN
			case cls.DESERT | cls.DESERT_HILLS | cls.DESERT_LAKES: return cls.DESERT
			# TODO: Create pull request for Cubiomes for THE_END case
			case cls.END_BARRENS | cls.END_HIGHLANDS | cls.END_MIDLANDS | cls.SMALL_END_ISLANDS | cls.THE_END: return cls.THE_END
			case cls.FROZEN_RIVER | cls.RIVER: return cls.RIVER
			case cls.GIANT_SPRUCE_TAIGA | cls.GIANT_SPRUCE_TAIGA_HILLS | cls.GIANT_TREE_TAIGA | cls.GIANT_TREE_TAIGA_HILLS | cls.SNOWY_TAIGA | cls.SNOWY_TAIGA_HILLS | cls.SNOWY_TAIGA_MOUNTAINS | cls.TAIGA | cls.TAIGA_HILLS | cls.TAIGA_MOUNTAINS: return cls.TAIGA
			case cls.GRAVELLY_MOUNTAINS | cls.MODIFIED_GRAVELLY_MOUNTAINS | cls.MOUNTAIN_EDGE | cls.MOUNTAINS | cls.WOODED_MOUNTAINS: return cls.MOUNTAINS
			case cls.MUSHROOM_FIELDS | cls.MUSHROOM_FIELD_SHORE: return cls.MUSHROOM_FIELDS
			case cls.PLAINS | cls.SUNFLOWER_PLAINS: return cls.PLAINS
			case cls.SAVANNA | cls.SAVANNA_PLATEAU | cls.SHATTERED_SAVANNA | cls.SHATTERED_SAVANNA_PLATEAU: return cls.SAVANNA
			case cls.SNOWY_TUNDRA | cls.SNOWY_MOUNTAINS | cls.ICE_SPIKES: return cls.SNOWY_TUNDRA
			case cls.SWAMP | cls.SWAMP_HILLS: return cls.SWAMP
			case cls.STONE_SHORE: return cls.STONE_SHORE
		return None

	@classmethod
	def areSimilar(cls, biome1: "Biome", biome2: "Biome") -> bool:
		"""Returns whether `biome1` and `biome2` are considered similar in terms of their categories."""
		# Badlands Plateau and Wooded Badlands are only similar to each other, which is the behavior exhibited by any 1.16+ version.
		return cls.getCategory(biome1, Version.NEWEST) == cls.getCategory(biome2, Version.NEWEST)
	
	@classmethod
	def isMesaBiome(cls, biome: "Biome") -> bool:
		"""Returns whether `biome` qualifies as a mesa biome."""
		# Badlands Plateau and Wooded Badlands qualify, which is the behavior exhibited by any 1.15- or an unspecified version.
		return cls.getCategory(biome) == cls.MESA
	
	@classmethod
	def isShallowOceanBiome(cls, biome: "Biome") -> bool:
		"""Returns whether `biome` qualifies as a shallow ocean biome."""
		return biome in {cls.COLD_OCEAN, cls.FROZEN_OCEAN, cls.LUKEWARM_OCEAN, cls.OCEAN, cls.WARM_OCEAN}
	
	@classmethod
	def isDeepOceanBiome(cls, biome: "Biome") -> bool:
		"""Returns whether `biome` qualifies as a deep ocean biome."""
		return biome in {cls.DEEP_COLD_OCEAN, cls.DEEP_FROZEN_OCEAN, cls.DEEP_LUKEWARM_OCEAN, cls.DEEP_OCEAN, cls.DEEP_WARM_OCEAN}
	
	@classmethod
	def isOceanBiome(cls, biome: "Biome") -> bool:
		"""Returns whether `biome` qualifies as an ocean biome."""
		return cls.getCategory(biome) == cls.OCEAN
	
	@classmethod
	def isSnowyBiome(cls, biome: "Biome") -> bool:
		"""Returns whether `biome` qualifies as a snowy biome."""
		return biome in {cls.FROZEN_OCEAN, cls.FROZEN_RIVER, cls.ICE_SPIKES, cls.SNOWY_BEACH, cls.SNOWY_MOUNTAINS, cls.SNOWY_TAIGA, cls.SNOWY_TAIGA_HILLS, cls.SNOWY_TAIGA_MOUNTAINS, cls.SNOWY_TUNDRA}