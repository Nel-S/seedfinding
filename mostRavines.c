#include "common.h"
#include "utilities/Chunkbiomes/Bfinders.c"
#include <string.h>

const int MAX_RADIUS = 3;
const int MIN_Y_LEVEL = 60;

typedef struct {
    Pos3 min, max;
    uint64_t count;
} BoundingBox;

void initGlobals() {}

void *runWorker(void *workerIndex) {
    BoundingBox bb;
    // In 1.18+ chunks can generate at most one ravine
    StructureVariant *sv;
    uint64_t bestCount = 1, bestScore = UINT64_MAX;

    uint64_t seed;
    if (!getNextSeed(workerIndex, &seed)) return NULL;
    do {
        uint64_t currentScore = UINT64_MAX;
        memset(&bb, 0, sizeof(bb));
        for (int chunkX = -MAX_RADIUS; chunkX <= MAX_RADIUS; ++chunkX) {
            for (int chunkZ = -MAX_RADIUS; chunkZ <= MAX_RADIUS; ++chunkZ) {
                if (!getRavinePos(MC_NEWEST, seed, chunkX, chunkZ, NULL, sv, NULL) || sv->y < MIN_Y_LEVEL) continue;
                ++bb.count;
                switch (bb.count) {
                    case 1:
                        bb.min.x = sv->x;
                        bb.min.y = sv->y;
                        bb.min.z = sv->z;
                        break;
                    case 2:
                        if (sv->x < bb.min.x) {
                            bb.max.x = bb.min.x; bb.min.x = sv->x;
                        } else bb.max.x = sv->x;
                        if (sv->y < bb.min.y) {
                            bb.max.y = bb.min.y; bb.min.y = sv->y;
                        } else bb.max.y = sv->y;
                        if (sv->z < bb.min.z) {
                            bb.max.z = bb.min.z; bb.min.z = sv->z;
                        } else bb.max.z = sv->z;
                        // currentScore = (bb.max.x - bb.min.x + 1) * (bb.max.y - bb.min.y + 1) * (bb.max.z - bb.min.z + 1) / (bb.count * bb.count);
                        // if (currentScore > bestScore) goto nextSeed;
                        break;
                    default:
                        if (sv->x < bb.min.x) bb.min.x = sv->x;
                        else if (sv->x > bb.max.x) bb.max.x = sv->x;
                        if (sv->y < bb.min.y) bb.min.y = sv->y;
                        else if (sv->y > bb.max.y) bb.max.y = sv->y;
                        if (sv->z < bb.min.z) bb.min.z = sv->z;
                        else if (sv->z > bb.max.z) bb.max.z = sv->z;
                        // currentScore = (bb.max.x - bb.min.x + 1) * (bb.max.y - bb.min.y + 1) * (bb.max.z - bb.min.z + 1) / (bb.count * bb.count);
                        // if (currentScore > bestScore) goto nextSeed;
                        break;
                }
            }
        }
        if (bb.count < bestCount) continue;
        if (bb.count == bestCount) {
            currentScore = (bb.max.x - bb.min.x + 1) * (bb.max.y - bb.min.y + 1) * (bb.max.z - bb.min.z + 1) / (bb.count * bb.count);
            if (currentScore > bestScore) continue;
            bestScore = currentScore;
        }
        outputValues("%" PRIu64 "\t%" PRIu64 "\t(%d, %d, %d) - (%d, %d, %d)\t%" PRIu64 "\n", seed, bb.count, bb.min.x, bb.min.y, bb.min.z, bb.max.x, bb.max.y, bb.max.z, currentScore);
        bestCount = bb.count;
        // nextSeed: continue;
    } while (getNextSeed(NULL, &seed));
    return NULL;
}