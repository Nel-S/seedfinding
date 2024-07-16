#include "../common.h"
#include "../utilities/cubiomes/finders.h"
#include <pthread.h>

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = CHECK_THIS_SEED_AND_FOLLOWING(GLOBAL_START_SEED);
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH = NULL;
const char *OUTPUT_FILEPATH = NULL;

const int VERSION = MC_NEWEST;
const int RADIUS_TO_CHECK = 2000;
const int INITIAL_COUNT_THRESHOLD = INT_MAX;
const uint64_t INITIAL_SCORE_THRESHOLD = UINT64_MAX;
//Flags: 1 for Large Biomes generation, 0 for normal generation.
const bool LARGE_BIOMES_FLAG    = false;
const bool FILTER_BY_COUNT_FLAG = false;
const bool FILTER_BY_SCORE_FLAG = true;
const bool CHECK_TERRAIN_FLAG   = true;
const bool TIME_PROGRAM         = false;

uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;

typedef struct {
	int structure;
	uint64_t weight;
} StructureEntry;

const StructureEntry STRUCTURES_TO_CHECK[] = {
	{Swamp_Hut, 3},
	{Igloo, 3},
	{Village, 5},
	{Ocean_Ruin, 1},
	{Monument, 1},
	{Mansion, 4},
	{Outpost, 4},
	{Ancient_City, 1}
};

// TODO: Add support for other versions
Pos getStructureBoundsRange(const int structure) {
	switch (structure) {
		case Treasure:
		case Mineshaft:
		case End_Gateway:
			return (Pos){-RADIUS_TO_CHECK >> 4, RADIUS_TO_CHECK >> 4};
		case Desert_Pyramid:
		case Jungle_Temple:
		case Swamp_Hut:
		case Igloo:
		case Monument:
		case Outpost:
			return (Pos){-RADIUS_TO_CHECK >> 9, RADIUS_TO_CHECK >> 9};
		case Ocean_Ruin:
		case End_City:
			return (Pos){-RADIUS_TO_CHECK/320 - 1, RADIUS_TO_CHECK/320};
		case Shipwreck:
		case Ancient_City:
			return (Pos){-RADIUS_TO_CHECK/384 - 1, RADIUS_TO_CHECK/384};
		case Fortress:
		case Bastion:
			return (Pos){-RADIUS_TO_CHECK/432 - 1, RADIUS_TO_CHECK/432};
		case Village:
			return (Pos){-RADIUS_TO_CHECK/544 - 1, RADIUS_TO_CHECK/544};
		case Ruined_Portal:
			return (Pos){-RADIUS_TO_CHECK/640 - 1, RADIUS_TO_CHECK/640};
		case Mansion:
			return (Pos){-RADIUS_TO_CHECK/1280 - 1, RADIUS_TO_CHECK/1280};
	};
	return (Pos){INT32_MIN, INT32_MAX}; 
}

const uint64_t MAXIMUM_RADIUS = (RADIUS_TO_CHECK - 1) * (uint64_t)(RADIUS_TO_CHECK - 1) + 1;
int minCount;
uint64_t minScore;
pthread_mutex_t mutex;

void initGlobals() {
	minCount = INITIAL_COUNT_THRESHOLD;
	minScore = INITIAL_SCORE_THRESHOLD;
	pthread_mutex_init(&mutex, NULL);
}

void *runWorker(void *workerIndex) {
	Generator g;
	setupGenerator(&g, VERSION, LARGE_BIOMES_FLAG);
	Pos structurePos;
	uint64_t seed;

	Pos structureBounds[sizeof(STRUCTURES_TO_CHECK)/sizeof(*STRUCTURES_TO_CHECK)];
	for (size_t i = 0; i < sizeof(structureBounds)/sizeof(*structureBounds); ++i) structureBounds[i] = getStructureBoundsRange(STRUCTURES_TO_CHECK[i].structure);

	if (!getNextSeed(workerIndex, &seed)) return NULL;
	applySeed(&g, DIM_OVERWORLD, seed);
	do {
		int count = 0;
		uint64_t score = 0;
		for (size_t i = 0; i < sizeof(STRUCTURES_TO_CHECK)/sizeof(*STRUCTURES_TO_CHECK); ++i) {
			if (!STRUCTURES_TO_CHECK[i].weight) continue;
			for (int rx = structureBounds[i].x; rx <= structureBounds[i].z; ++rx) {
				for (int rz = structureBounds[i].x; rz <= structureBounds[i].z; ++rz) {
					if (!getStructurePos(STRUCTURES_TO_CHECK[i].structure, VERSION, seed, rx, rz, &structurePos)) continue;
					uint64_t squaredRadialDist = structurePos.x * (uint64_t)structurePos.x + structurePos.z * (uint64_t)structurePos.z;
					if (MAXIMUM_RADIUS < squaredRadialDist) continue;
					// TODO: Add support for non-overworld structures
					if (g.seed != seed || g.dim != DIM_OVERWORLD) applySeed(&g, DIM_OVERWORLD, seed);
					if (!isViableStructurePos(STRUCTURES_TO_CHECK[i].structure, &g, structurePos.x, structurePos.z, 0)) continue;
					if (CHECK_TERRAIN_FLAG && !isViableStructureTerrain(STRUCTURES_TO_CHECK[i].structure, &g, structurePos.x, structurePos.z)) continue;
					if (FILTER_BY_COUNT_FLAG) {
						++count;
						if (count > minCount) goto skipSeed;
					}
					if (FILTER_BY_SCORE_FLAG) {
						score += STRUCTURES_TO_CHECK[i].weight * (RADIUS_TO_CHECK * RADIUS_TO_CHECK - squaredRadialDist);
						if ((!FILTER_BY_COUNT_FLAG || count == minCount) && score > minScore) goto skipSeed;
					}
				}
			}
		}
		pthread_mutex_lock(&mutex);
		if ((FILTER_BY_COUNT_FLAG && count > minCount) || (FILTER_BY_SCORE_FLAG && (!FILTER_BY_COUNT_FLAG || count == minCount) && score > minScore)) {
			pthread_mutex_unlock(&mutex);
			continue;
		}
		if (FILTER_BY_SCORE_FLAG) {
			if (FILTER_BY_COUNT_FLAG) outputValue("%" PRId64 "\t%d\t%" PRIu64 "\n", seed, count, score);
			else outputValue("%" PRId64 "\t%" PRIu64 "\n", seed, score);
		} else outputValue("%" PRId64 "\t%d\n", seed, count);
		if (count < minCount || ((!FILTER_BY_COUNT_FLAG || count == minCount) && score < minScore)) {
			minCount = count;
			minScore = score;
		}
		pthread_mutex_unlock(&mutex);

		skipSeed: continue;
	} while (getNextSeed(NULL, &seed));
	return NULL;
}