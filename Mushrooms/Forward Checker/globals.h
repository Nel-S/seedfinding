#ifndef _GLOBALS_H
#define _GLOBALS_H
// Imports settings
#include "settings.h"

#include "Utilities/Fast Climates.h"
#include <limits.h>
#include <time.h>

// Struct for interpreting the parameters sent to each thread's *checkSeed() function
typedef struct {
	uint64_t index;
} Data;

const double CONT_FITNESS_SQRT = MOST_POSITIVE_CONT < -0.11 ? 10000*MOST_POSITIVE_CONT + 1100 : 0;
const double FITNESS = CONT_FITNESS_SQRT * CONT_FITNESS_SQRT;

/* Variable length (and thus runtime-initialized) lookup table. The ith position's jth octave's continental sample must be less than Cdouble[i][j] to still potentially be an 
	nth-ring spawn (assuming no other climates have values extreme enough to influence the spawn fitness calculations, which is true most of the time).
   Derivable by the formula CDOUBLE[i][j] = U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS[j] - (1100 + sqrt(FITNESS - U_SPAWN_FIRST_STAGE_VALS[i][4]))/10000/(3./2)).*/
double Cdouble[sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS)][sizeof(U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS)];
uint_fast8_t ringStartingIndex;

const size_t biomeCacheSize = (size_t)RADIUS * RADIUS + 3*(((RADIUS + 3) >> 2) + 2)*(((RADIUS + 3) >> 2) + 2);
// const size_t biomeCacheSize = (size_t)3*(((2*RADIUS + 3) >> 2) + 2)*(((2*RADIUS + 3) >> 2) + 2);

#endif