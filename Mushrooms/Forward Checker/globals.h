#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "settings.h"
#include "utilities/Spawn.h"

DEFAULT_LOCALS_INITIALIZATION

const double CONT_FITNESS_SQRT = MOST_POSITIVE_CONT < -0.11 ? 10000*(MOST_POSITIVE_CONT + 0.11) : 0;
const double FITNESS = CONT_FITNESS_SQRT * CONT_FITNESS_SQRT;

/* Runtime-initialized lookup table. The ith position's jth octave's continental sample must be less than Cdouble[i][j] to still potentially be an 
	nth-ring spawn (assuming no other climates have values extreme enough to influence the spawn fitness calculations, which is true most of the time).*/
double Cdouble[sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS)][sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];
uint_fast8_t ringStartingIndex;

const size_t BIOME_CACHE_SIZE = (size_t)RADIUS * RADIUS + 3*(((RADIUS + 3ULL) >> 2) + 2)*(((RADIUS + 3ULL) >> 2) + 2);
// const size_t BIOME_CACHE_SIZE = (size_t)3*(((2*RADIUS + 3) >> 2) + 2)*(((2*RADIUS + 3) >> 2) + 2);

#endif