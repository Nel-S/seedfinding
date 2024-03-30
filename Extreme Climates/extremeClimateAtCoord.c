#include "Utilities/Climates.h"
#include <stdbool.h>

const uint64_t GLOBAL_START_SEED = 110546880051;
const uint64_t GLOBAL_SEEDS_TO_CHECK = -1;
const int      GLOBAL_NUMBER_OF_WORKERS = 4;
const char    *FILEPATH = NULL;

const int CLIMATE = NP_TEMPERATURE;
const Pos COORD = {0, 0};
/* The sign of this indicates whether to look for minimum or maximum continentalnesses.
   The value of this specifies, if AS_PERCENT_ is true, what percentage of the maximum possible climate amplitude (between 0 and 1) to use as a threshold.
   If AS_PERCENT is false, it instead specifices the direct 
   E.g. -0.2 will look for seeds with continentalnesses 20% that of the minimum.*/
const double THRESHOLD = 0.83;
const bool   AS_PERCENT = true;

const bool LARGE_BIOMES_FLAG = false;
const bool CHECK_PX_FLAG     = false;
const bool TIME_PROGRAM      = false;



uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
extern void outputValues(const uint64_t *seeds, const void *otherValues, const size_t count);

double Cdouble[sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];

void initGlobals() {
	U_initClimateBoundsArray(CLIMATE, AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] * THRESHOLD : (THRESHOLD >= 0 ? min(THRESHOLD, U_MAX_CLIMATE_AMPLITUDES[CLIMATE]) : max(THRESHOLD, U_MIN_CLIMATE_AMPLITUDES[CLIMATE])), Cdouble, U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]);
}

void *checkSeeds(void *workerIndex) {
	PerlinNoise oct[U_NUMBER_OF_OCTAVES];
	for (uint64_t count = *(int *)workerIndex; count < localSeedsToCheck; count += localNumberOfWorkers) {
		uint64_t seed = localStartSeed + count;

		double px = COORD.x, pz = COORD.z, npC;
		if (CHECK_PX_FLAG) {
			U_initClimate(NP_SHIFT, oct, seed);
			U_sampleClimate(NP_SHIFT, oct, &px, &pz);
		} else {
			px = floor(px/4);
			pz = floor(pz/4);
		}

		if (U_initAndSampleClimateBounded(CLIMATE, oct, &px, &pz, THRESHOLD >= 0 ? Cdouble : NULL, THRESHOLD >= 0 ? NULL : Cdouble, &seed, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]) continue;
		/*Prints the seed.*/
		int out = 10000 * npC;
		outputValues(&seed, &out, 1);
	}
	return NULL;
}