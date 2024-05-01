from .base import Version, Dimension
from .base import *
from .rng import *
from .noise import OverworldNoise, NetherNoise, EndNoise
from .noise import *
from .biomes import *
from .layers import LayerStack, Layer
from .layers import *
from enum import IntFlag

class Generator:
    class Flags(IntFlag):
        LARGE_BIOMES         = 1
        NO_BETA_OCEANS       = 2
        FORCE_OCEAN_VARIANTS = 4

    worldseed: int
    version: Version | None = None
    dimension: Dimension | None = None
    flags: int
    sha: int

    # Alpha 1.2 - Beta 1.7, 1.18+
    overworldNoise: OverworldNoise
    # 1.0 - 1.17
    layerStack: LayerStack
    entryLayer: Layer
    customEntryLayerBuffer: tuple[Layer, Layer, Layer, Layer, Layer]
    # 1.9+
    endNoise: EndNoise
    # 1.16+
    netherNoise: NetherNoise
    ...