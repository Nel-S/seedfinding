#ifndef _FUNCTIONS_C
#define _FUNCTIONS_C

#include "globals.h"

/*
	Custom main() files MUST
		- #include "functions.c";
		- call initGlobals() before starting the search;
		- create a struct "Data <name>" in each thread/process/etc. such that <name>.index = <index of the thread/process/etc.>;
		- heave each thread/process/etc. call checkSeeds(&<name>) to start its search; and
		- implement outputValues().
	Anything else can be implemented to your liking. (Also, localStartSeed, localSeedsToCheck, and localNumberOfProcesses can be overridden if desired.)
*/
uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
extern void outputValues(const uint64_t *seeds, const void *otherValues, size_t count);


void initGlobals() {
	for (ringStartingIndex = 0; ringStartingIndex < sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS) && U_SPAWN_FIRST_STAGE_VALS[ringStartingIndex][U_spawn_table_fitness] <= FITNESS; ++ringStartingIndex);

	for (uint_fast8_t i = 0; i < ringStartingIndex; ++i) {
		U_initClimateBoundsArray(NP_CONTINENTALNESS, U_getEffectiveContinentalness(FITNESS - U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_fitness]), Cdouble[i], sizeof(*Cdouble)/sizeof(**Cdouble));
	}
}

void *checkSeeds(void *workerIndex) {
	BiomeNoise bn;
	initBiomeNoise(&bn, MC_NEWEST);
	U_manualBNinit(&bn);

	int ids[BIOME_CACHE_SIZE];
	PerlinNoise *oct = bn.oct;
	// Iterates over N seeds, beginning at the one originally specified by the user
	for (uint64_t count = *(int *)workerIndex; count < localSeedsToCheck; count += localNumberOfWorkers) {
		uint64_t seed = count + localStartSeed;
		double px = 0, pz = 0, npC;
		if (!DELAY_SHIFT) U_initAndSampleClimateBounded(NP_SHIFT, oct, &px, &pz, NULL, NULL, &seed, NULL);

		if (U_initAndSampleClimateBounded(NP_CONTINENTALNESS, oct, &px, &pz, NULL, Cdouble[0], &seed, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

		// If a 1st-ring spawn is all that is needed, we can stop here; otherwise, we repeat the process again for each position in each ring below the desired one.
		for (uint_fast8_t i = 1; i < ringStartingIndex; ++i) {
			px = U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_x] >> 2, pz = U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_z] >> 2;
			if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz);
			if (U_sampleClimateBounded(NP_CONTINENTALNESS, oct, &px, &pz, NULL, Cdouble[i], &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) goto skip;
		}
		/*At this point, a seed *could* be an Nth-ring spawn, but we need to verify that it truly is.
			This does require initializing the other 4 climates (and consequently 22 more octaves).*/
		// TODO: Only initialize a climate when actually necessary
		if (DELAY_SHIFT) U_initClimate(NP_SHIFT, oct, seed);
		if (SAMPLE_ALL_CLIMATES) {
			U_initClimate(NP_TEMPERATURE, oct, seed);
			U_initClimate(NP_HUMIDITY, oct, seed);
			U_initClimate(NP_EROSION, oct, seed);
			U_initClimate(NP_WEIRDNESS, oct, seed);
        }
		/* Emulates the first round of the spawn algorithm, updating minI and fitness if the values turn out to be the lowest thus far, or doing nothing otherwise.
  		   Uses samples of all five climates because e.g. 34788113448 is marked as a second-ring spawn instead of a third-ring if only continentalness is measured.*/
		int firstStageIndex, secondStageIndex;
		double fitness;
		if (!U_firstStageSpawnBounded(oct, FITNESS, &firstStageIndex, &fitness)) continue;

		/* At this point, we know the seed is an nth ring spawn, so we now just need to ensure the second stage of the spawn algorithm places its spawn beyond
		   (the square root of) MIN_DISTANCE_SQUARED blocks.*/
		if (!U_secondStageSpawnBounded(oct, firstStageIndex, fitness, FITNESS, &secondStageIndex, &fitness)) continue;
		Pos approxSpawn = {((int)U_SPAWN_SECOND_STAGE_VALS[firstStageIndex][secondStageIndex][U_spawn_table_x] & -16) + 8, ((int)U_SPAWN_SECOND_STAGE_VALS[firstStageIndex][secondStageIndex][U_spawn_table_z] & -16) + 8};
		
		double highestCont = -INFINITY, lowestCont = INFINITY;
		for (int dx = -RADIUS; dx <= RADIUS - 1; dx += 2*RADIUS - 1) {
			for (int dz = -RADIUS; dz <= RADIUS - 1; dz += 2*RADIUS - 1) {
				px = approxSpawn.x + dx;
				pz = approxSpawn.z + dz;
				U_sampleClimate(NP_SHIFT, oct, &px, &pz);
				// Continentalness
				npC = U_sampleClimate(NP_CONTINENTALNESS, oct, &px, &pz);
				if (npC > highestCont) {
					highestCont = npC;
					if (highestCont > -0.2) goto skip;
				}
				if (npC < lowestCont) lowestCont = npC;
			}
		}
		if (lowestCont > -0.95) continue;

		bool mushroomExists = false;
		int mushroomDist = INT_MAX;
		genBiomeNoiseScaled(&bn, ids, (Range){1, approxSpawn.x - RADIUS, approxSpawn.z - RADIUS, 2*RADIUS, 2*RADIUS, 63, 1}, getVoronoiSHA(seed));
		for (size_t i = 0; i < 4*RADIUS*RADIUS; ++i) {
			if (ids[i] == mushroom_fields) {
				mushroomExists = true;
				int x = approxSpawn.x - RADIUS + (i % RADIUS), z = approxSpawn.z - RADIUS + (i/RADIUS);
				int maxDist = abs(x - approxSpawn.x) > abs(z - approxSpawn.z) ? abs(x - approxSpawn.x) : abs(z - approxSpawn.z);
				if (maxDist < mushroomDist) mushroomDist = maxDist;
			}
			else if (!isOceanic(ids[i])) goto skip;
		}
		// Range s = {4, 0, 0, 0, 0, 15, 2};
		// s.x = (approxSpawn.x - RADIUS - 2) >> 2;
		// s.sx = ((approxSpawn.x + RADIUS - 2) >> 2) - ((approxSpawn.x - RADIUS - 2) >> 2) + 2;
		// s.z = (approxSpawn.z - RADIUS - 2) >> 2;
		// s.sz = ((approxSpawn.z + RADIUS - 2) >> 2) - ((approxSpawn.z - RADIUS - 2) >> 2) + 2;
		// int *idP = ids;
		// for (int k = 0; k < 2; ++k) {
		// 	for (int j = 0; j < s.sz; ++j) {
		// 		for (int i = 0; i < s.sx; ++i) {
		// 			*idP = sampleBiomeNoise(&bn, NULL, s.x + i, 15 + k, s.z + j, NULL, 0);
		// 			if (*idP == mushroom_fields) mushroomExists = true;
		// 			idP++;
		// 		}
		// 	}
		// }
		// if (!mushroomExists) continue;

		// mushroomExists = false;
		// uint64_t sha = getVoronoiSHA(seed);
		// for (int j = 0; j < 2*RADIUS; j++) {
		// 	int z = approxSpawn.z - RADIUS + j;
		// 	for (int i = 0; i < 2*RADIUS; i++) {
		// 		int x4, z4, y4, x = approxSpawn.x - RADIUS + i;
		// 		voronoiAccess3D(sha, x, 63, z, &x4, &y4, &z4);
		// 		int biome = ids[((int64_t)(y4 - 15)*s.sz + (z4 - s.z))*s.sx + (x4 - s.x)];
		// 		if (biome == mushroom_fields) {
		// 			mushroomExists = true;
		// 			int maxDist = abs(x - approxSpawn.x) > abs(z - approxSpawn.z) ? abs(x - approxSpawn.x) : abs(z - approxSpawn.z);
		// 			if (maxDist < mushroomDist) mushroomDist = maxDist;
		// 		}
		// 		else if (!isOceanic(biome)) goto skip;
		// 	}
		// }

		if (!mushroomExists) continue;

		outputValues(&seed, &mushroomDist, 1);
		skip: continue;
	}
	return NULL;
}

#endif