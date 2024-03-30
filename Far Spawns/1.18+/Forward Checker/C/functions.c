#ifndef _FUNCTIONS_C
#define _FUNCTIONS_C

#include "globals.h"

void initGlobals() {
	for (ringStartingIndex = 0; ringStartingIndex < sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS) && U_SPAWN_FIRST_STAGE_VALS[ringStartingIndex][U_spawn_table_fitness] <= FITNESS; ++ringStartingIndex);

	for (uint8_t i = 0; i < ringStartingIndex; ++i) U_initClimateBoundsArray(NP_CONTINENTALNESS, U_getEffectiveContinentalness(FITNESS - U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_fitness]), Cdouble[i], sizeof(*Cdouble)/sizeof(**Cdouble));
}

void *checkSeeds(void *workerIndex) {
	PerlinNoise oct[U_NUMBER_OF_OCTAVES];
	// Iterates over N seeds, beginning at the one originally specified by the user
	for (uint64_t count = *(int *)workerIndex; count < localSeedsToCheck; count += localNumberOfWorkers) {
		uint64_t seed = count + localStartSeed;
		
		double px = U_SPAWN_FIRST_STAGE_VALS[0][U_spawn_table_x], pz = U_SPAWN_FIRST_STAGE_VALS[0][U_spawn_table_z], npC;
		if (!DELAY_SHIFT) U_initAndSampleClimateBounded(NP_SHIFT, oct, &px, &pz, NULL, NULL, &seed, NULL);
		else {
			px = floor(px/4);
			pz = floor(pz/4);
		}
		if (U_initAndSampleClimateBounded(NP_CONTINENTALNESS, oct, &px, &pz, NULL, Cdouble[0], &seed, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

		// If a 1st-ring spawn is all that is needed, we can stop here; otherwise, we repeat the process again for each position in each ring below the desired one.
		for (uint_fast8_t i = 1; i < ringStartingIndex; ++i) {
			px = U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_x], pz = U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_z];
			if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz);
			else {
				px = floor(px/4);
				pz = floor(pz/4);
			}
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
		if (!U_firstStageSpawnBounded(&oct, FITNESS, &firstStageIndex, &fitness)) continue;

		#if CHECK_DISTANCES
		/* At this point, we know the seed is an nth ring spawn, so we now just need to ensure the second stage of the spawn algorithm places its spawn beyond
		   (the square root of) MIN_DISTANCE_SQUARED blocks.*/
		if (!U_secondStageSpawnBounded(&oct, firstStageIndex, fitness, FITNESS, &secondStageIndex, &fitness)) continue;
		Pos approxSpawn = {U_SPAWN_SECOND_STAGE_VALS[firstStageIndex][secondStageIndex][U_spawn_table_x], U_SPAWN_SECOND_STAGE_VALS[firstStageIndex][secondStageIndex][U_spawn_table_z]};
		Pos centeredSpawn = {(approxSpawn.x & -16) + 8, (approxSpawn.z & -16) + 8};
		uint32_t distSquared = centeredSpawn.x * centeredSpawn.x + centeredSpawn.z * centeredSpawn.z;
		/* Prints the seed if its squared second-stage approximate distance is further than MIN_RADIAL_DISTANCE blocks away, or either axis is further than MIN_AXIAL_DISTANCE 
		blocks away.*/
		if (distSquared < MIN_RADIAL_DISTANCE * MIN_RADIAL_DISTANCE && abs(approxSpawn.x) < MIN_AXIAL_DISTANCE && abs(approxSpawn.z) < MIN_AXIAL_DISTANCE) continue;
		double dist = sqrt(distSquared);
		outputValues(&seed, &dist, 1);
		#else
		outputValues(&seed, &firstStageIndex, 1);
		#endif
		skip: continue;
	}
	return NULL;
}

#endif