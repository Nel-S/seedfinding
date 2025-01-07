#include "../core/common_seedfinding.h"
#include "../Utilities/Climates.h"
#include <pthread.h>

// const uint64_t GLOBAL_START_SEED = 110546880051;
const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = CHECK_THIS_SEED_AND_FOLLOWING(GLOBAL_START_SEED);
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;

const int CLIMATE = NP_TEMPERATURE;
const Pos COORD = {0, 0};
const int PERCENTILE = FIFTIETH_PERCENTILE;
/* The sign of this indicates whether to look for minimum or maximum continentalnesses.
   The value of this specifies, if AS_PERCENT_ is true, what percentage of the maximum possible climate amplitude (between 0 and 1) to use as a threshold.
   If AS_PERCENT is false, it instead specifies the direct 
   E.g. -0.2 if AS_PERCENT = true will look for seeds with continentalnesses 20% that of the minimum, while if AS_PERCENT is false it will look for seeds with continentalnesses of -0.2 or less.*/
const double INITIAL_THRESHOLD = 0.59;
const bool AS_PERCENT = true;
const bool UPDATE_THRESHOLD = true;

const bool LARGE_BIOMES_FLAG = false;
const bool CHECK_PX_FLAG = false;
const bool TIME_PROGRAM = false;



DEFAULT_LOCALS_INITIALIZATION
double climateBounds[sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];
double currentThreshold;
pthread_mutex_t mutex;

void initGlobals() {
	currentThreshold = INITIAL_THRESHOLD;
	U_initClimateBoundsArray(CLIMATE, AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] * INITIAL_THRESHOLD : (INITIAL_THRESHOLD >= 0 ? min(INITIAL_THRESHOLD, U_MAX_CLIMATE_AMPLITUDES[CLIMATE]) : max(INITIAL_THRESHOLD, U_MIN_CLIMATE_AMPLITUDES[CLIMATE])), PERCENTILE, climateBounds, U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]);
}

void *runWorker(void *workerIndex) {
	PerlinNoise octaves[U_NUMBER_OF_OCTAVES];

	double origPx = COORD.x, origPz = COORD.z;
	if (!CHECK_PX_FLAG) {
		origPx = floor(origPx/4.);
		origPz = floor(origPz/4.);
	}

	uint64_t seed;
	if (!getNextSeed(workerIndex, &seed)) return NULL;
	do {
		double px = origPx, pz = origPz, sample;
		if (CHECK_PX_FLAG) {
			U_initClimate(NP_SHIFT, octaves, seed, LARGE_BIOMES_FLAG);
			U_sampleClimate(NP_SHIFT, octaves, &px, &pz, LARGE_BIOMES_FLAG);
		}

		if (U_initAndSampleClimateBounded(CLIMATE, octaves, &px, &pz, INITIAL_THRESHOLD >= 0 ? climateBounds : NULL, INITIAL_THRESHOLD >= 0 ? NULL : climateBounds, &seed, LARGE_BIOMES_FLAG, &sample) < U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]) continue;
		/*Prints the seed.*/
		outputValues("%" PRId64 "\t%f\t%f\n", seed, sample, sample/U_MAX_CLIMATE_AMPLITUDES[CLIMATE]);
		if (UPDATE_THRESHOLD) {
			pthread_mutex_lock(&mutex);
			if (fabs(currentThreshold) < fabs(sample/(AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] : 1))) {
				currentThreshold = sample/(AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] : 1);
				U_initClimateBoundsArray(CLIMATE, AS_PERCENT ? U_MAX_CLIMATE_AMPLITUDES[CLIMATE] * currentThreshold : (currentThreshold >= 0 ? min(currentThreshold, U_MAX_CLIMATE_AMPLITUDES[CLIMATE]) : max(currentThreshold, U_MIN_CLIMATE_AMPLITUDES[CLIMATE])), PERCENTILE, climateBounds, U_CLIMATE_NUMBER_OF_OCTAVES[CLIMATE]);
			}
			pthread_mutex_unlock(&mutex);
		}
	} while (getNextSeed(NULL, &seed));
	return NULL;
}