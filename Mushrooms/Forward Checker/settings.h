#ifndef _SETTINGS_H
#define _SETTINGS_H
#include "../../core/common_seedfinding.h"

// Sets the seed to start at and the number of seeds to iterate over.
const uint64_t GLOBAL_START_SEED = 4000000000300814;
const uint64_t GLOBAL_SEEDS_TO_CHECK = 1000000;
// (If one's main() file uses multithreading), sets the number of threads to create.
const int GLOBAL_NUMBER_OF_WORKERS = 4;

// The most positive effective continentalness to allow during the spawn algorithm.
const double MOST_POSITIVE_CONT = -0.4;
// The radius (half-edge-length) of the bounding box surrounding the chunk-centered second-stage spawnpoint to search within.
// (For context, the real spawn algorithm uses a radius of 88.)
const int RADIUS = 105;
// const int RADIUS = 400;

// (If one's main() file prints to a file) The filepath to print results to.
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;

// Set to false for normal generation, or true for Large Biomes generation.
const bool LARGE_BIOMES_FLAG = false;

// Set to true to ignore shift for the initial continentalness checks, or false to still sample it.
// (Speeds up the search at the cost of slight accuracy.)
const bool DELAY_SHIFT = true;

// Set to true to sample temperature, humidity, etc. when emulating the spawn algorithm, or false to sample continentalness only.
// (Slows down the search with the benefit being better accuracy.)
const bool SAMPLE_ALL_CLIMATES = true;

// Set to true to time the program's runtime, or false otherwise.
const bool TIME_PROGRAM = true;

#endif