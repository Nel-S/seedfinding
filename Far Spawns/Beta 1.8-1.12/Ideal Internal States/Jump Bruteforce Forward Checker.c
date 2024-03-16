#include "cubiomes/rng.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>

const uint64_t START_SEED = 0;
const uint64_t SEEDS_TO_CHECK = (1ULL << 48) - START_SEED;
const uint8_t NUMBER_OF_THREADS = 4;

typedef struct {
	int x, z;
} Pos;

uint64_t farthestDist = 0;
// 138728061      (3890, -2996) = 24108116 (4910.001629)
// pthread_mutex_t mutex;

void *checkSeed(void *dat) {
	uint64_t rng;
	for (uint64_t count = *(uint8_t *)dat; count < SEEDS_TO_CHECK; count += NUMBER_OF_THREADS) {
		uint64_t seed = START_SEED + count;
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
		// pthread_mutex_lock(&mutex);
		if (currentDist < farthestDist) {
			// pthread_mutex_unlock(&mutex);
			continue;
		}
		char *bestDemarker = "\t";
		if (farthestDist < currentDist) {
			farthestDist = currentDist;
			bestDemarker = "*";
		}
		// pthread_mutex_unlock(&mutex);
		printf("%s%" PRIu64 "\t(%d, %d) = %" PRIu64 " (%f)\n", bestDemarker, seed, currentPos.x, currentPos.z, currentDist, sqrt(currentDist));
		// skipSeed: continue;
	}
	return NULL;
}

int main() {
	pthread_t threads[NUMBER_OF_THREADS];
	for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) pthread_create(&threads[i], NULL, checkSeed, &i);
	for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) pthread_join(threads[i], NULL);
	return 0;
}