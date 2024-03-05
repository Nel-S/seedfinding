#include "../cubiomes/finders.h"
#include "Brng.h"

int getBedrockStructureConfig(const int structureType, const int mc, StructureConfig *sconf);

static inline ATTR(const)
Pos mGetFeatureChunkInRegion(const StructureConfig config, uint64_t seed, int regX, int regZ);

static inline Pos getBedrockFeaturePos(StructureConfig config, uint64_t seed, int regX, int regZ);

int getBedrockStructurePos(int structureType, int mc, uint64_t seed, int regX, int regZ, Pos *pos);

/* Returns the number of ravines found.
   For 1.17 and earlier, two ravine checks are performed per coordinate: one "ordinary" one and a second for ocean ravines.
   If the second is desired, `g` must be provided and have been initialized; in all other cases, `g` can be set to NULL.
   If not NULL, the position and size of the ravines is stored in `ravine1` and `ravine2`.*/
int getRavinePos(int mc, uint64_t seed, int x, int z, const Generator *g, StructureVariant *ravine1, StructureVariant *ravine2);

/* Returns if a ravine was found.
   If not NULL, the position and size of the ravine, and whether it is giant, are stored in `ravine`.*/
int getBedrockRavinePos(uint64_t seed, int x, int z, StructureVariant *ravine);