from noise import *
from biomes import *

class BiomeTemperatureCategory(IntEnum):
    OCEANIC = 0
    WARM = 1
    LUSH = 2
    COLD = 3
    FREEZING = 4
    SPECIAL = 5

class LayerID(IntEnum):
    CONTINENT_4096 = ISLAND_4096 = 0
    ZOOM_4096 = 1
    LAND_4096 = 2
    ZOOM_2048 = 3
    LAND_2048 = ADD_ISLAND_2048 = 4
    ZOOM_1024 = 5
    LAND_1024_A = ADD_ISLAND_1024_A = 6
    LAND_1024_B = ADD_ISLAND_1024_B = 7
    LAND_1024_C = ADD_ISLAND_1024_C = 8
    ISLAND_1024 = REMOVE_OCEAN_1024 = 9
    SNOW_1024 = ADD_SNOW_1024 = 10
    LAND_1024_D = ADD_ISLAND_1024_D = 11
    COOL_1024 = COOL_WARM_1024 = 12
    HEAT_1024 = HEAT_ICE_1024 = 13
    SPECIAL_1024 = 14
    ZOOM_512 = 15
    LAND_512 = 16
    ZOOM_256 = 17
    LAND_256 = ADD_ISLAND_256 = 18
    MUSHROOM_256 = ADD_MUSHROOM_256 = 19
    DEEP_OCEAN_256 = 20
    BIOME_256 = 21
    BAMBOO_256 = 22
    ZOOM_128 = 23
    ZOOM_64 = 24
    BIOME_EDGE_64 = 25
    NOISE_256 = RIVER_INIT_256 = 26
    ZOOM_128_HILLS = 27
    ZOOM_64_HILLS = 28
    HILLS_64 = 29
    SUNFLOWER_64 = RARE_BIOME_64 = 30
    ZOOM_32 = 31
    LAND_32 = ADD_ISLAND_32 = 32
    ZOOM_16 = 33
    SHORE_16 = 34
    SWAMP_RIVER_16 = 35
    ZOOM_8 = 36
    ZOOM_4 = 37
    SMOOTH_4 = 38
    ZOOM_128_RIVER = 39
    ZOOM_64_RIVER = 40
    ZOOM_32_RIVER = 41
    ZOOM_16_RIVER = 42
    ZOOM_8_RIVER = 43
    ZOOM_4_RIVER = 44
    RIVER_4 = 45
    SMOOTH_4_RIVER = 46
    RIVER_MIX_4 = 47
    OCEAN_TEMP_256 = 48
    ZOOM_128_OCEAN = 49
    ZOOM_64_OCEAN = 50
    ZOOM_32_OCEAN = 51
    ZOOM_16_OCEAN = 52
    ZOOM_8_OCEAN = 53
    ZOOM_4_OCEAN = 54
    OCEAN_MIX_4 = 55
    VORONOI_1 = VORONOI_ZOOM_1 = 56
    ZOOM_LARGE_A = 57
    ZOOM_LARGE_B = 58
    ZOOM_L_RIVER_A = 59
    ZOOM_L_RIVER_B = 60
    NUMBER_OF_LAYERS = 61

class Layer:
    version: Version
    zoomFactor: int
    requiredEdge: int
    scale: int
    layerSalt: int
    startSalt: int
    startSeed: int
    noise: PerlinNoise | None = None
    data: None
    parents: tuple["Layer | None", "Layer | None"]
    INITIAL_SHA = 2**64 - 1
    def getMap(self, coordRange: Range) -> list: return []

    def seed(self, worldseed: int) -> None:
        for parent in reversed(self.parents):
            if parent is not None: parent.seed(worldseed)
        if self.noise is not None: self.noise.seed(Random(worldseed))
        
        match self.layerSalt:
            case 0: self.startSalt = self.startSeed = 0
            case self.INITIAL_SHA:
                self.startSalt = getVoronoiSHA(worldseed)
                self.startSeed = 0
            case _:
                self.startSalt = SeedHelper.getStartSalt(worldseed, self.layerSalt)
                self.startSeed = SeedHelper.getStartSeed(worldseed, self.layerSalt)
    
    @staticmethod
    def isAny4(id: int, a: int, b: int, c: int, d: int) -> bool:
        return id in {a, b, c, d}

    def mapContinent(self, coordRange: Range) -> list[bool]:
        # 2D range
        out: list[bool] = [SeedHelper.firstIntIsZero(SeedHelper.getChunkSeed(self.startSeed, Coordinate(x, z)), 10) for z in range(coordRange.width) for x in range(coordRange.length)]
        if -coordRange.length < coordRange.x <= 0 and -coordRange.width < coordRange.z <= 0: out[-coordRange.z * coordRange.length - coordRange.x] = True
        return out
    
    #TODO: Unfinished
    def mapZoomFuzzy(self, coordRange: Range) -> list[int]:
        # 2D range
        reducedCoordRange = Range(coordRange.x >> 1, coordRange.z >> 1, ((coordRange.x + coordRange.length) >> 1) - (coordRange.x >> 1) + 1, ((coordRange.z + coordRange.width) >> 1) - (coordRange.z >> 1) + 1)
        if self.parents[0] is None: raise ValueError("Layer has no parent, meaning parent's getMap() method cannot be accessed.")
        out = self.parents[0].getMap(reducedCoordRange) + [0 for _ in range(reducedCoordRange.length * reducedCoordRange.width)]
        newW = reducedCoordRange.length * 2
        for j in range(reducedCoordRange.width):
            idx = 2*j*newW
        return []
    



class LayerStack:
    layers: list[Layer]
    entry1: Layer
    entry4: Layer
    entry16: Layer
    entry64: Layer
    entry256: Layer
    OceanRNG: PerlinNoise

# TODO: Unimplemented
def getVoronoiSHA(seed: int) -> int:
    return 0