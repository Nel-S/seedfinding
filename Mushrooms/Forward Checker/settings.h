#ifndef _SETTINGS_H
#define _SETTINGS_H
#include <inttypes.h>
#include <stdbool.h>

// Sets the seed to start at and the number of seeds to iterate over.
const uint64_t GLOBAL_START_SEED = 108584794827;
const uint64_t GLOBAL_SEEDS_TO_CHECK = 10000000;

const double MOST_POSITIVE_CONT = -0.4;
// The radius (half-edge-length) of the bounding box surrounding the chunk-centered second-stage spawnpoint to search within.
// (The real spawn algorithm uses a radius of 88.)
const int RADIUS = 106;

// (If one's main() file prints to a file) The filepath to print results to.
const char *FILEPATH = "mushroomSeeds.txt";

// Set to false for normal generation, or true for Large Biomes generation.
const bool LARGE_BIOMES_FLAG = false;

// Set to true to ignore shift for the initial continentalness checks, or false to still sample it.
// (Speeds up the search at the cost of slight accuracy.)
const bool DELAY_SHIFT = true;

// Set to true to sample temperature, humidity, etc. when emulating the spawn algorithm.
// (Slows down the search with the benefit being far better accuracy.)
const bool SAMPLE_ALL_CLIMATES = true;

// If running with multithreading, sets the number of threads to create.
const uint8_t NUMBER_OF_THREADS = 4;

// Set to 1 to time the program's runtime, or 0 to disable that feature.
const bool TIME_PROGRAM = true;

#endif