#include "common.h"
#include "Utilities/Climates.h"

const uint64_t GLOBAL_START_SEED = 110546880051;
const uint64_t GLOBAL_SEEDS_TO_CHECK = -1;
const int      GLOBAL_NUMBER_OF_WORKERS = 4;
const char    *INPUT_FILEPATH  = NULL;
const char    *OUTPUT_FILEPATH = NULL;

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



localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK, localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;

double Cdouble[sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];

void initGlobals() {
	U_initClimateBoundsArray(CLIMATE, AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] * THRESHOLD : (THRESHOLD >= 0 ? min(THRESHOLD, U_MAX_CLIMATE_AMPLITUDES[CLIMATE]) : max(THRESHOLD, U_MIN_CLIMATE_AMPLITUDES[CLIMATE])), ONE_HUNDREDTH_PERCENTILE, Cdouble, U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]);
}

void *runWorker(void *workerIndex) {
	PerlinNoise oct[U_NUMBER_OF_OCTAVES];

	double origPx = COORD.x, origPz = COORD.z;
	if (!CHECK_PX_FLAG) {
		origPx = floor(origPx/4);
		origPz = floor(origPz/4);
	}

	uint64_t seed;
	if (!getNextSeed(workerIndex, &seed)) return NULL;
	do {
		double px = origPx, pz = origPz, npC;
		if (CHECK_PX_FLAG) {
			U_initClimate(NP_SHIFT, oct, seed);
			U_sampleClimate(NP_SHIFT, oct, &px, &pz);
		}

		if (U_initAndSampleClimateBounded(CLIMATE, oct, &px, &pz, THRESHOLD >= 0 ? Cdouble : NULL, THRESHOLD >= 0 ? NULL : Cdouble, &seed, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]) continue;
		/*Prints the seed.*/
		outputValue("%" PRId64 "\t%f\n", seed, npC);
	} while (getNextSeed(NULL, &seed));
	return NULL;
}