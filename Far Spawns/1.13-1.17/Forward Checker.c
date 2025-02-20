#include "utilities/cubiomes/finders.h"
#include "core/bruteforce.h"
#include "Utilities/U_Math.h"
#include <pthread.h>

// const uint64_t GLOBAL_START_INTEGER = 160091761;
const uint64_t GLOBAL_START_INTEGER = INT64_MIN;
const uint64_t GLOBAL_NUMBER_OF_INTEGERS = CHECK_THIS_INTEGER_AND_FOLLOWING(GLOBAL_START_INTEGER, 64);
// const uint64_t GLOBAL_NUMBER_OF_INTEGERS = 1000000;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH = NULL;
const char *OUTPUT_FILEPATH = NULL;
const bool LARGE_BIOMES_FLAG = false;
const bool TIME_PROGRAM = false;

const int VERSION = MC_1_16_5;
// const uint64_t INITIAL_SQUARED_RADIAL_THRESHOLD = 398 * 398;
// const uint64_t INITIAL_MAX_AXIAL_THRESHOLD = 285;
// const uint64_t INITIAL_MIN_AXIAL_THRESHOLD = 280;
// const uint64_t INITIAL_SQUARED_RADIAL_THRESHOLD = 288*288 + 303*303;
// const uint64_t INITIAL_MAX_AXIAL_THRESHOLD = 300;
// const uint64_t INITIAL_MIN_AXIAL_THRESHOLD = 288;
const uint64_t INITIAL_SQUARED_RADIAL_THRESHOLD = 256*256 + 257*257;
const uint64_t INITIAL_MAX_AXIAL_THRESHOLD = 257;
const uint64_t INITIAL_MIN_AXIAL_THRESHOLD = 257;
const bool UPDATE_THRESHOLD = true;


DEFAULT_LOCALS_INITIALIZATION

// valid spawn biomes up to 1.17
const uint64_t VALID_SPAWN_BIOMES = (1ULL << forest) |
									(1ULL << plains) |
									(1ULL << taiga) |
									(1ULL << taiga_hills) |
									(1ULL << wooded_hills) |
									(1ULL << jungle) |
									(1ULL << jungle_hills);
const int BIOMES_MATCH_ANY_256[] = {
	plains, forest, taiga, wooded_hills, taiga_hills, jungle, jungle_hills
};
const int BIOMES_TO_EXCLUDE_256[] = {
	ocean, desert, mountains, swamp, river, nether_wastes, the_end, frozen_ocean, frozen_river, snowy_tundra, snowy_mountains, mushroom_fields, mushroom_field_shore, beach, 
	desert_hills, mountain_edge, jungle_edge, deep_ocean, stone_shore, snowy_beach, birch_forest, birch_forest_hills, dark_forest, snowy_taiga, snowy_taiga_hills, 
	giant_tree_taiga, giant_tree_taiga_hills, wooded_mountains, savanna, savanna_plateau, badlands, wooded_badlands_plateau, badlands_plateau, small_end_islands, end_midlands, 
	end_highlands, end_barrens, warm_ocean, lukewarm_ocean, cold_ocean, deep_warm_ocean, deep_lukewarm_ocean, deep_cold_ocean, deep_frozen_ocean, seasonal_forest, rainforest, 
	shrubland, sunflower_plains, desert_lakes, gravelly_mountains, flower_forest, taiga_mountains, swamp_hills, ice_spikes, modified_jungle, modified_jungle_edge, 
	tall_birch_forest, tall_birch_hills, dark_forest_hills, snowy_taiga_mountains, giant_spruce_taiga, giant_spruce_taiga_hills, modified_gravelly_mountains, shattered_savanna, 
	shattered_savanna_plateau, eroded_badlands, modified_wooded_badlands_plateau, modified_badlands_plateau, bamboo_jungle, bamboo_jungle_hills, soul_sand_valley, crimson_forest, 
	warped_forest, basalt_deltas, dripstone_caves, lush_caves, meadow, grove, snowy_slopes, jagged_peaks, frozen_peaks, stony_peaks, old_growth_birch_forest, 
	old_growth_pine_taiga, old_growth_spruce_taiga, snowy_plains, sparse_jungle, stony_shore, windswept_hills, windswept_forest, windswept_gravelly_hills, windswept_savanna, 
	wooded_badlands, deep_dark, mangrove_swamp, cherry_grove, 
};

volatile uint64_t farthestSquaredRadialDist;
volatile int farthestMinAxialDist, farthestMaxAxialDist;
pthread_mutex_t mutex;

void initializeGlobals() {
	farthestSquaredRadialDist = INITIAL_SQUARED_RADIAL_THRESHOLD;
	farthestMinAxialDist = INITIAL_MIN_AXIAL_THRESHOLD;
	farthestMaxAxialDist = INITIAL_MAX_AXIAL_THRESHOLD;
	pthread_mutex_init(&mutex, NULL);
}

void *runWorker(void *workerIndex) {
	Generator g;
	setupGenerator(&g, VERSION, LARGE_BIOMES_FLAG);
	BiomeFilter bf_256;
	// setupBiomeFilter(&bf_256, VERSION, 0, NULL, 0, BIOMES_TO_EXCLUDE_256, sizeof(BIOMES_TO_EXCLUDE_256)/sizeof(*BIOMES_TO_EXCLUDE_256), NULL, 0);
	setupBiomeFilter(&bf_256, VERSION, 0, NULL, 0, NULL, 0, BIOMES_MATCH_ANY_256, sizeof(BIOMES_MATCH_ANY_256)/sizeof(*BIOMES_MATCH_ANY_256));
	// setupBiomeFilter(&bf_256, VERSION, 0, NULL, 0, BIOMES_TO_EXCLUDE_256, sizeof(BIOMES_TO_EXCLUDE_256)/sizeof(*BIOMES_TO_EXCLUDE_256), BIOMES_MATCH_ANY_256, sizeof(BIOMES_MATCH_ANY_256)/sizeof(*BIOMES_MATCH_ANY_256));
	SurfaceNoise sn;
	Pos spawn;
	float y[16];
	int ids[16];
	uint64_t random, seed;
	if (!getNextInteger(workerIndex, &seed)) return NULL;
	// printf("(%" PRId64 ")", seed);
	do {
		/* The farthest possible 1.13-1.17 spawnpoint can only occur if the initial 513x513 biome search around the origin picks (256, 256).
		   For that to happen, the 1:4 biome at (256, 256) must be one of the biomes in VALID_SPAWN_BIOMES.*/
		int status = checkForBiomesAtLayer(&g.ls, g.ls.entry_256, NULL, seed, 256 >> 8, 256 >> 8, 1, 1, &bf_256);
		if (!status) continue;
		if (status == 1) applySeed(&g, DIM_OVERWORLD, seed);

		/* Otherwise perform the biome search and breaks if a different coordinate would be picked. */
		setSeed(&random, seed);
		Pos estimatedSpawn = locateBiome(&g, 0, 63, 0, 256, VALID_SPAWN_BIOMES, 0, &random, NULL);
		if (estimatedSpawn.x != 256 || estimatedSpawn.z != 256) continue;

		/* Then check if (256, 256) would NOT (be likely to) have a grass block. If not, break.*/
		initSurfaceNoise(&sn, DIM_OVERWORLD, seed);
		mapApproxHeight(y, ids, &g, &sn, 256 >> 2, 256 >> 2, 1, 1);
		int grassStartsAt = 0;
		getBiomeDepthAndScale(ids[0], NULL, NULL, &grassStartsAt);
		if (grassStartsAt && y[0] >= grassStartsAt) continue;

		/* Then check if (527, 527) would (be likely to) have a grass block. If not, break.*/
		mapApproxHeight(y, ids, &g, &sn, 527 >> 2, 527 >> 2, 1, 1);
		grassStartsAt = 0;
		getBiomeDepthAndScale(ids[0], NULL, NULL, &grassStartsAt);
		if (!grassStartsAt || y[0] < grassStartsAt) continue;

		/* Otherwise emulate the rest of the spawnpoint algorithm.
		   (From Cubiomes' getSpawn())*/
		int j = 0, k = 0, u = 0, v = -1;
		for (int i = 0; i < 1024; ++i) {
			// if (j > -16 && j <= 16 && k > -16 && k <= 16) {
				// find server spawn point in chunk
				int cx0 = 256 + j * 16; // start of chunk
				int cz0 = 256 + k * 16;
				mapApproxHeight(y, ids, &g, &sn, cx0 >> 2, cz0 >> 2, 4, 4);
				for (int ii = 0; ii < 4; ++ii) {
					for (int jj = 0; jj < 4; ++jj) {
						getBiomeDepthAndScale(ids[jj*4+ii], NULL, NULL, &grassStartsAt);
						if (grassStartsAt <= 0 || y[jj*4+ii] < grassStartsAt) continue;
						spawn.x = cx0 + ii * 4;
						spawn.z = cz0 + jj * 4;
						goto validSpawn;
					}
				}
			// }
			if (j == k || (j < 0 && j == -k) || (j > 0 && j == 1 - k)) {
				int tmp = u;
				u = -v;
				v = tmp;
			}
			j += u;
			k += v;
		}
		continue;
		validSpawn: ;
		uint64_t currentSquaredRadialDist = (uint64_t)spawn.x * spawn.x + (uint64_t)spawn.z * spawn.z;
		int currentMinAxialDist = min(abs(spawn.x), abs(spawn.z));
		int currentMaxAxialDist = max(abs(spawn.x), abs(spawn.z));
		if (UPDATE_THRESHOLD) pthread_mutex_lock(&mutex);
		int records = ((currentSquaredRadialDist >= farthestSquaredRadialDist) << 2) | ((currentMinAxialDist >= farthestMinAxialDist) << 1) | (currentMaxAxialDist >= farthestMaxAxialDist);
		if (!records) {
			if (UPDATE_THRESHOLD) pthread_mutex_unlock(&mutex);
			continue;
		}
		outputString("%" PRId64 "\t%d\t%d\t%f\t%d\n", seed, spawn.x, spawn.z, sqrt(currentSquaredRadialDist), records);
		if (UPDATE_THRESHOLD) {
			if (records & 4) farthestSquaredRadialDist = currentSquaredRadialDist;
			if (records & 2) farthestMinAxialDist = currentMinAxialDist;
			if (records & 1) farthestMaxAxialDist = currentMaxAxialDist;
			pthread_mutex_unlock(&mutex);
		}
	} while (getNextInteger(NULL, &seed));
	return NULL;
}