from .base import (
    IntEnum, Sequence,
    toSigned,
    THIRTY_TWO_BITS,
    Version
)
from .base import *
from .rng import Random
from .rng import *
from .noise import *
from .biomes import *
from .layers import *
from .generator import IntFlag
from .generator import *

def isSlimeChunk(seed: int, chunkCoord: Sequence) -> bool: # finders.isSlimeChunk
    """"""
    rng = Random((seed + ((chunkCoord[0]*(0x5ac0db + chunkCoord[0]*0x4c1906) + chunkCoord[1]*0x5f24f) & THIRTY_TWO_BITS) + ((chunkCoord[1]*chunkCoord[1]) & THIRTY_TWO_BITS)*0x4307a7) ^ 0x3ad8025f)
    return not rng.nextInt(10)

def getShadowSeed(seed: int, version: Version | None = None) -> int:
    """"""
    if version is not None and (version < Version.V1_0 or Version.V1_17 < version): raise RuntimeError("Shadow seeds only exist between versions 1.0 and 1.17 inclusive.")
    return toSigned(-7379792620528906219 - seed)

class StructureGenerator(IntEnum):
    GENERIC_FEATURE = 0
    DESERT_PYRAMID = 1
    JUNGLE_PYRAMID = JUNGLE_TEMPLE = 2
    SWAMP_HUT = 3
    IGLOO = 4
    VILLAGE = 5
    OCEAN_RUIN = 6
    SHIPWRECK = 7
    MONUMENT = 8
    MANSION = 9
    OUTPOST = 10
    RUINED_PORTAL = OVERWORLD_RUINED_PORTAL = 11
    NETHER_RUINED_PORTAL = 12
    ANCIENT_CITY = 13
    BURIED_TREASURE = 14
    MINESHAFT = 15
    DESERT_WELL = 16
    GEODE = 17
    TRAIL_RUIN = 18
    NETHER_FORTRESS = 19
    BASTION = 20
    END_CITY = 21
    END_GATEWAY = 22

    class Attributes(IntFlag):
        TRIANGULAR_DISTRIBUTION = 1
        NETHER_STRUCTURE = 2
        END_STRUCTURE = 4
    
    @staticmethod
    def transposeStructure(baseSeed: int, regionDisplacement: Sequence[int]) -> int: # finders.moveStructure
        """"""
        return (baseSeed - regionDisplacement[0]*341873128712 - regionDisplacement[1]*132897987541) & FORTY_EIGHT_BITS
    
    ...