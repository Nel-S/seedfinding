#ifndef _CLIMATES_H
#define _CLIMATES_H

#include "cubiomes/finders.h"
#include <stdbool.h>

enum {TENTH_PERCENTILE, TWENTIETH_PERCENTILE, THIRTIETH_PERCENTILE, FOURTIETH_PERCENTILE, FIFTIETH_PERCENTILE, SIXTIETH_PERCENTILE, SEVENTIETH_PERCENTILE, EIGHTIETH_PERCENTILE, NINETIETH_PERCENTILE, ONE_HUNDREDTH_PERCENTILE, PERCENTILES};
extern const bool LARGE_BIOMES_FLAG;
extern const double U_PI, U_SQRT_2, U_SQRT_3, U_TWO_PI,
    U_MAX_PERLIN_VALUE, U_MIN_PERLIN_VALUE, U_PERLIN_BENCHMARKS[PERCENTILES],
    U_MAX_CLIMATE_AMPLITUDES[NP_MAX], U_MIN_CLIMATE_AMPLITUDES[NP_MAX], U_SET_CLIMATE_AMPLITUDES[NP_MAX],
    U_MAX_TEMP_OCTAVE_AMPLITUDE_SUMS[4],  U_MIN_TEMP_OCTAVE_AMPLITUDE_SUMS[4],  U_MAX_HUMID_OCTAVE_AMPLITUDE_SUMS[4], U_MIN_HUMID_OCTAVE_AMPLITUDE_SUMS[4],
    U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS[18], U_MIN_CONT_OCTAVE_AMPLITUDE_SUMS[18], U_MAX_EROS_OCTAVE_AMPLITUDE_SUMS[8],  U_MIN_EROS_OCTAVE_AMPLITUDE_SUMS[8], 
    U_MAX_SHIFT_OCTAVE_AMPLITUDE_SUMS[6], U_MIN_SHIFT_OCTAVE_AMPLITUDE_SUMS[6], U_MAX_WEIRD_OCTAVE_AMPLITUDE_SUMS[6], U_MIN_WEIRD_OCTAVE_AMPLITUDE_SUMS[6];
extern const int U_NUMBER_OF_OCTAVES, U_CLIMATE_NUMBER_OF_OCTAVES[NP_MAX];

#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define IF_EXISTS(X, Y) ((X) ? (Y) : 0)

// Fills an array with the bounds necessary for a particular climate sample to occur. Note that the sign of `desiredClimateSample` is also taken into account.
// Returns the number of array elements set.
// If `climate == NP_SHIFT` and space is available in the array, the bounds will be filled twice over--the first set for px, the second set for pz--for usage in e.g. `U_sampleClimateBounded()` later on.
int U_initClimateBoundsArray(const int climate, const double desiredClimateSample, const int percentile, double *array, const size_t arraySize);

// Initializes several 1.18+ BiomeNoise constants that Cubiomes chooses to regenerate each time instead of hardcoding (https://github.com/Cubitect/cubiomes/issues/82).
// Note that Cubiomes' `xPerlinInit()` will reset the octave amplitudes and lacunarities initialized by this; use `U_initPerlin()` instead.
void U_manualBNinit(BiomeNoise *bn);

// Clone of Cubiomes' xPerlinInit() that doesn't disrupt the octave's amplitudes/lacunarities.
void U_initPerlin(PerlinNoise *octave, Xoroshiro *xoroshiro);

// Initializes a climate using `U_initPerlin()`s.
void U_initClimate(const int climate, PerlinNoise *oct, const uint64_t seed);

// Initializes all climates using `U_initPerlin()`s.
void U_initAllClimates(PerlinNoise *oct, const uint64_t seed);

// Samples a climate without regenerating the octaves' amplitudes, lacunarities, or climate configurations.
// If climate is `NP_SHIFT`, the shift is stored in px and pz; otherwise (for `NP_TEMPERATURE`, `NP_HUMIDITY`, `NP_CONTINENTALNESS`, `NP_EROSION`, and `NP_WEIRDNESS`) the sample is directly returned.
// If a non-`SHIFT` climate is sampled directly without sampling `NP_SHIFT` first, `*px` must be set to `floor(*px/4.)` and `*pz` to `floor(*pz/4.)` beforehand.
// Depth sampling is not supported due to that using splines in addition to Perlin octaves.
double U_sampleClimate(const int climate, PerlinNoise *oct, double *px, double *pz);

// Same as `U_sampleClimate()`, but this breaks early if the sum of the first `n` Perlin octaves' samples are less than `lowerBounds[n]` or greater than `upperBounds[n]`. The number of sampled octaves is returned.
// If climate is `NP_SHIFT`, the shift is stored in px and pz, and `lowerBounds[i]`/`upperBounds[i]` must be double the length--the first half being px's bounds, the second half being pz's bounds. Otherwise (for `NP_TEMPERATURE`, `NP_HUMIDITY`, `NP_CONTINENTALNESS`, `NP_EROSION`, and `NP_WEIRDNESS`), the sample is stored in climateSample.
// If a non-`SHIFT` climate is sampled directly without sampling `NP_SHIFT` first, `*px` must be set to `floor(*px/4.)` and `*pz` to `floor(*pz/4.)` beforehand.
// Depth sampling is not supported due to that using splines in addition to Perlin octaves.
int U_sampleClimateBounded(const int climate, PerlinNoise *oct, double *px, double *pz, const double *lowerBounds, const double *upperBounds, double *climateSample);

// Same as `U_initClimate()` and `U_sampleClimateBounded()`, but only initializes as much as is necessary to check each octave, and aborts the initialization whenever `U_sampleClimateBounded()` fails. The number of initialized octaves is returned.
// Note that seedIfInitializingClimate is a pointer, and can be nulled (which simply reverts the function to `U_sampleClimateBounded`).
// If a non-`SHIFT` climate is sampled directly without sampling `NP_SHIFT` first, `*px` must be set to `floor(*px/4.)` and `*pz` to `floor(*pz/4.)` beforehand.
// If climate is `NP_SHIFT`, the shift is stored in px and pz, and `lowerBounds[i]`/`upperBounds[i]` must be double the length--the first half being px's bounds, the second half being pz's bounds. Otherwise (for `NP_TEMPERATURE`, `NP_HUMIDITY`, `NP_CONTINENTALNESS`, `NP_EROSION`, and `NP_WEIRDNESS`), the sample is stored in climateSample.
// Depth sampling is not supported due to that using splines in addition to Perlin octaves.
int U_initAndSampleClimateBounded(const int climate, PerlinNoise *oct, double *px, double *pz, const double *lowerBounds, const double *upperBounds, const uint64_t *seedIfInitializingClimate, double *climateSample);

#endif