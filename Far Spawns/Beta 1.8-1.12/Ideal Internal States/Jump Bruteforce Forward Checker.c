#include "cubiomes/finders.h"
#include <math.h>
#include <stdbool.h>

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = (1ULL << 48) - GLOBAL_START_SEED;
const int GLOBAL_NUMBER_OF_WORKERS = 4;

const char *FILEPATH = NULL;
const bool TIME_PROGRAM = false;

uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
extern void outputValues(const uint64_t *seeds, const void *otherValues, const size_t count);

uint64_t farthestDist = 0;
// 138728061      (3890, -2996) = 24108116 (4910.001629)

void *checkSeed(void *workerIndex) {
	uint64_t rng;
	for (uint64_t count = *(int *)workerIndex; count < localSeedsToCheck; count += localNumberOfWorkers) {
		uint64_t seed = localStartSeed + count;
		// setSeed(&rng, seed);
		rng = seed;
		Pos currentPos = {0, 0};
		// Pos bestPossiblePos = {63000, 63000};
		for (int i = 0; i < 1000; ++i) {
			int dispX = nextInt(&rng, 64) - nextInt(&rng, 64);
			int dispZ = nextInt(&rng, 64) - nextInt(&rng, 64);
			currentPos.x += dispX;
			currentPos.z += dispZ;
			// bestPossiblePos.x += abs(dispX);
			// bestPossiblePos.z += abs(dispZ);
			// if (bestPossiblePos.x * bestPossiblePos.x + bestPossiblePos.z * bestPossiblePos.z < farthestDist) goto skipSeed;
		}
		uint64_t currentDist = currentPos.x * currentPos.x + currentPos.z * currentPos.z;
		if (currentDist < farthestDist) continue;
		char *bestDemarker = "\t";
		if (farthestDist < currentDist) {
			farthestDist = currentDist;
			bestDemarker = "*";
		}
		// TODO: Port to outputValues
		printf("%s%" PRIu64 "\t(%d, %d) = %" PRIu64 " (%f)\n", bestDemarker, seed, currentPos.x, currentPos.z, currentDist, sqrt(currentDist));
		// skipSeed: continue;
	}
	return NULL;
}