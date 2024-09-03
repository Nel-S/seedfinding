#ifndef _COMMON_H
#define _COMMON_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: Add non-CUDA support

// These are constants that must be set in the settings for each program.
extern const uint64_t GLOBAL_START_SEED, GLOBAL_SEEDS_TO_CHECK;
extern const int GLOBAL_NUMBER_OF_WORKERS;
extern const char *INPUT_FILEPATH, *OUTPUT_FILEPATH;
extern const bool TIME_PROGRAM;

// These are global variables that will by default be set to their constant equivalents, but can be modified as needed.
extern uint64_t localStartSeed, localSeedsToCheck;
extern int localNumberOfWorkers;
// Macro for GLOBAL_SEEDS_TO_CHECK/localSeedsToCheck
#define CHECK_THIS_SEED_AND_FOLLOWING(startSeed) UINT64_MAX - startSeed + (!!startSeed)

#ifdef __cplusplus
extern "C" {
#endif

// This is defined in the filtering program.
// It must initialize all global variables prior to any workers being created.
void initGlobals();
// This is defined in the filtering program.
// It must initialize a worker; it is also recommended it
// - call `getNextSeed(workerIndex, &seed)` once to fetch the first seed (or abort if the function returns false),
// - have a do-while loop check each seed against some conditions, calling `outputValues()` if the conditions are met; and
// - have the condition for the do-while loop call `getNextSeed(NULL, &seed)` and abort if the function returns false,
// though custom implementations are still allowed if one wishes.
void *runWorker(void *workerIndex);

// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_GET_NEXT_SEED and providing your own implementation.
// If workerIndex is not NULL, it returns the first seed for the specified worker.
// Otherwise if workerIndex is NULL, it returns the next seed in the sequence.
// Returns false when the end of the sequence is reached.
bool getNextSeed(const void* workerIndex, uint64_t *seed);
// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_OUTPUT_VALUES and providing your own implementation.
// It prints the information for a single result given a setup identical to `printf()`.
void outputValues(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif