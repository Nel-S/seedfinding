#ifndef _GLOBALS_H
#define _GLOBALS_H

// Imports settings
#include "settings.h"

#include "Utilities/Spawn.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

// Struct for interpreting the parameters sent to each thread's *checkSeed() function
typedef struct {
	uint64_t id;
} ThreadData;

#if CHECK_DISTANCES
// Calculates the corresponding MIN_DESIRED_RING if only a distance is provided by the user
// (Should be 0 if MIN_RADIAL_DISTANCE <= 512., but 0 is not supported) 
// TODO: Redo in terms of fitness lookup table using U_SPAWN_FIRST_STAGE_VALS[][4]
const uint8_t MIN_DESIRED_RING = (MIN_RADIAL_DISTANCE <= 1024.) ? 1 :
								 (MIN_RADIAL_DISTANCE <= 1536.) ? 2 :
								 (MIN_RADIAL_DISTANCE <= 2048.) ? 3 : 4;
#endif

/* Index at which each ring begins. Given the U_SPAWN_FIRST_STAGE_VALS[] coordinates are sorted by ascending distance, this is also the index of the smallest distance in each ring, while this minus one
   is the index of the largest distance in the previous ring.*/
uint8_t ringStartingIndex;
					                  						   
#if CHECK_DISTANCES
const double MIN_DIST = (MIN_RADIAL_DISTANCE < MIN_AXIAL_DISTANCE ? MIN_RADIAL_DISTANCE : MIN_AXIAL_DISTANCE)/25;
const double CONT_FITNESS_SQRT = MOST_POSITIVE_CONT < -0.11 ? 10000*MOST_POSITIVE_CONT + 1100 : 0;
const double NON_RING_FITNESS = MIN_DIST * MIN_DIST * MIN_DIST * MIN_DIST + CONT_FITNESS_SQRT * CONT_FITNESS_SQRT;
#endif

// TODO: Fix
#if CHECK_DISTANCES
// const double FITNESS = U_SPAWN_FIRST_STAGE_VALS[RING_STARTING_INDEX][4] > NON_RING_FITNESS ? U_SPAWN_FIRST_STAGE_VALS[RING_STARTING_INDEX][4] : NON_RING_FITNESS;
const double FITNESS = NON_RING_FITNESS;
#else
const double FITNESS = (RING_STARTING_INDEX <= 1 ) ? 0        /* U_SPAWN_FIRST_STAGE_VALS[0 ][4] */ :
					   (RING_STARTING_INDEX <= 8 ) ? 174491   /* U_SPAWN_FIRST_STAGE_VALS[1 ][4] */ :
					   (RING_STARTING_INDEX <= 21) ? 2802142  /* U_SPAWN_FIRST_STAGE_VALS[8 ][4] */ :
					   (RING_STARTING_INDEX <= 40) ? 14209992 /* U_SPAWN_FIRST_STAGE_VALS[21][4] */ :
					   								 44935571 /* U_SPAWN_FIRST_STAGE_VALS[40][4] */ ;
#endif

/* Variable length (and thus runtime-initialized) lookup table. The ith position's jth octave's continental sample must be less than Cdouble[i][j] to still potentially be an 
	nth-ring spawn (assuming no other climates have values extreme enough to influence the spawn fitness calculations, which is true most of the time).
   Derivable by the formula CDOUBLE[i][j] = U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS[j] - (1100 + sqrt(FITNESS - U_SPAWN_FIRST_STAGE_VALS[i][4]))/10000/(3./2)).*/
double Cdouble[sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS)][sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];

#endif