#include "../../../Utilities/core/common_seedfinding.h"
#include "../../../Utilities/Climates.h"
#include <math.h>

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = (1ULL << 48) - GLOBAL_START_SEED;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;
const bool TIME_PROGRAM = false;

DEFAULT_LOCALS_INITIALIZATION

uint64_t farthestDist = 0;
// 138728061      (3890, -2996) = 24108116 (4910.001629)

void initGlobals() {}

void *runWorker(void *workerIndex) {
	uint64_t seed;
	if (!getNextSeed(workerIndex, &seed)) return NULL;
	do {
		// setSeed(&rng, seed);
		uint64_t rng = seed;
		Pos localBestPos = {0, 0}, currentPos = {0, 0};
		int bestI = 0;
		uint64_t localFarthestDist = 0, currentDist = 0;
		// Pos bestPossiblePos = {63000, 63000};
		for (int i = 0; i < 1000; ++i) {
		// for (int i = 0; i < 1000 && sqrt(currentDist) + (1000 - i)*63*U_SQRT_2 >= sqrt(farthestDist); ++i) {
			int dispX = nextInt(&rng, 64) - nextInt(&rng, 64);
			int dispZ = nextInt(&rng, 64) - nextInt(&rng, 64);
			currentPos.x += dispX;
			currentPos.z += dispZ;
			// bestPossiblePos.x -= abs(dispX);
			// bestPossiblePos.z -= abs(dispZ);
			// if (bestPossiblePos.x * bestPossiblePos.x + bestPossiblePos.z * bestPossiblePos.z < farthestDist) goto skipSeed;
			currentDist = currentPos.x * currentPos.x + currentPos.z * currentPos.z;
			if (localFarthestDist < currentDist) {
				localFarthestDist = currentDist;
				bestI = i;
				localBestPos.x = currentPos.x, localBestPos.z = currentPos.z;
			}
		}
		if (localFarthestDist < farthestDist) continue;
		if (farthestDist < localFarthestDist) farthestDist = localFarthestDist;
		outputValues("%" PRId64 "\t%d\t%d\t%d\t%f\n", seed, bestI, localBestPos.x, localBestPos.z, sqrt(localFarthestDist));
		// skipSeed: continue;
	} while (getNextSeed(NULL, &seed));
	return NULL;
}