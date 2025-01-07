#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "../../../../core/common_seedfinding.h"
#include "../../../../Utilities/Spawn.h"

// The seed to start from.
// const uint64_t GLOBAL_START_SEED = 3426926457792;
const uint64_t GLOBAL_START_SEED = 0;
// const uint64_t GLOBAL_START_SEED = 159316728991;
// The number of seeds to iterate over.
// (Note to self: redoing 2481627134962 up to 2612082682250 might be needed worst-case scenario, but I seriously doubt any candidates were missed)
// const uint64_t GLOBAL_SEEDS_TO_CHECK = CHECK_THIS_SEED_AND_FOLLOWING(GLOBAL_START_SEED);
const uint64_t GLOBAL_SEEDS_TO_CHECK = 10000000;
// If running with multithreading/multiprocessing, sets the number of workers to create.
const int GLOBAL_NUMBER_OF_WORKERS = 4;

// The filepath to print results to. If NULL, defaults to stdout.
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;

/* Setting this to false will have the program search for spawns within a particular "ring", given the spawn algorithm initially
	  checks five spaced-out "rings" of points to determine what approximate distance range the spawn will be from the origin.
	  Faster, but allows less precision in terms of distance from the origin.
   Setting this to true will have the program search for spawns with approximate locations beyond a specified radial distance
      from the origin.
	  Slower, but allows more precision in terms of distance from the origin.*/
#define CHECK_DISTANCES true

#if CHECK_DISTANCES
/* Keep in mind that the program does not model the last stage of the spawn algorithm, so results may be off from their true
   radial distance by at most 88sqrt(2) ~ 124.5 blocks.*/
const double MIN_RADIAL_DISTANCE = 1759.01819 - 88*1.41421356237309505; //1634.567398;
// const double MIN_RADIAL_DISTANCE = 2389.66629469 - 88*1.41421356237309505; //1634.567398;

// For the same reason, results may be off from their true larger axial distance by at most 88 blocks.
const double MIN_AXIAL_DISTANCE = 1728 - 88;
// const double MIN_AXIAL_DISTANCE = 2363 - 88;

const double MOST_POSITIVE_CONT = -0.0;

const int CLIMATE_PERCENTILE = FIFTIETH_PERCENTILE;
#else
/* Set to the minimum spawn ring one wishes to search for. (All rings farther than the desired one will also be returned.)
	Ring 1: Spawns [   0, sqrt(1261186) ~ 1123.0] blocks from the origin
	Ring 2: Spawns [ 417, 13sqrt(15794) ~ 1633.8] blocks from the origin
	Ring 3: Spawns [ 929, sqrt(4601474) ~ 2145.1] blocks from the origin
	Ring 4: Spawns [1441, 5sqrt(282322) ~ 2656.7] blocks from the origin (not confirmed to exist yet, though I'd be a bit surprised if they didn't)
	
	There is also a ring 0, with spawns [0, sqrt(377474) ~ 614.4] blocks from the origin, but since every seed has the capability to be a zeroth ring
	spawn, this program does not support that option.*/
const uint8_t MIN_DESIRED_RING = 3;
#endif

// Set to false for normal generation, or true for Large Biomes generation.
const bool POST_1_21_1 = true;
const bool LARGE_BIOMES_FLAG = false;
// const bool LARGE_BIOMES_FLAG = true;

// Set to true to ignore shift for the initial continentalness checks, or false to still sample it.
// (Speeds up the search at the cost of slight accuracy.)
// TODO: Decide whether to keep or not
const bool DELAY_SHIFT = true;

// Set to true to sample temperature, humidity, etc. when emulating the spawn algorithm.
// (Slows down the search with the benefit being far better accuracy.)
const bool SAMPLE_ALL_CLIMATES = true;

// Set to 1 to time the program's runtime, or 0 to disable that feature.
const bool TIME_PROGRAM = false;

#endif