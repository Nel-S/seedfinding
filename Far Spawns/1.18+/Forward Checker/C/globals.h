#ifndef _GLOBALS_H
#define _GLOBALS_H

// Imports settings
#include "settings.h"

DEFAULT_LOCALS_INITIALIZATION

/* Index at which each ring begins. Given the U_SPAWN_FIRST_STAGE_VALS[] coordinates are sorted by ascending distance, this is also the index of the smallest distance in each ring, while this minus one
   is the index of the largest distance in the previous ring.*/
uint8_t ringStartingIndex;
					                  						   
#if CHECK_DISTANCES
const double MIN_DIST = max(MIN_RADIAL_DISTANCE - 88*1.4*JAVA_SPAWN, 0) < max(MIN_AXIAL_DISTANCE - 88*JAVA_SPAWN, 0) ? max(MIN_RADIAL_DISTANCE - 88*1.4*JAVA_SPAWN, 0) : max(MIN_AXIAL_DISTANCE - 88*JAVA_SPAWN, 0);
const double CONT_FITNESS_SQRT = MOST_POSITIVE_CONT < -0.11 ? (POST_1_21_1 ? 20480000. : 10000.)*(MOST_POSITIVE_CONT + 0.11) : 0;
const double FITNESS = MIN_DIST * MIN_DIST * (POST_1_21_1 ? 1 : MIN_DIST * MIN_DIST/390625.) + CONT_FITNESS_SQRT * CONT_FITNESS_SQRT;
#else
const double FITNESS = (!MIN_DESIRED_RING     ) ? 0        /* U_SPAWN_FIRST_STAGE_VALS[0 ][U_spawn_table_fitness] */ :
					   ( MIN_DESIRED_RING == 1) ? 174491   /* U_SPAWN_FIRST_STAGE_VALS[1 ][U_spawn_table_fitness] */ :
					   ( MIN_DESIRED_RING == 2) ? 2802142  /* U_SPAWN_FIRST_STAGE_VALS[8 ][U_spawn_table_fitness] */ :
					   ( MIN_DESIRED_RING == 3) ? 14209992 /* U_SPAWN_FIRST_STAGE_VALS[21][U_spawn_table_fitness] */ :
					   							  44935571 /* U_SPAWN_FIRST_STAGE_VALS[40][U_spawn_table_fitness] */ ;
#endif

/* Runtime-initialized lookup table. The ith position's jth octave's continental sample must be less than Cdouble[i][j] to still potentially be an 
   nth-ring spawn (assuming no other climates have values extreme enough to influence the spawn fitness calculations, which is true most of the time).*/
double Cdouble[sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS)][sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];

#endif