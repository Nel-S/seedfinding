#include "Climates.h"
#include <math.h>

// Mathematical constants.
const double U_PI     = 3.1415926535897932384626433;
const double U_SQRT_2 = 1.4142135623730950488016887;
const double U_SQRT_3 = 1.7320508075688772935274463;
const double U_TWO_PI = 6.2831853071795864769252866;

/*
	TODO: Check if all climate samples are truly truncated after four decimal places, as Cubiomes suggests.
	If so, I have a lot of fixing to do.
*/

// The maximum value that a Perlin sample can return.
const double U_MAX_PERLIN_VALUE = 1.0363538112118025;
// The minimum value that a Perlin sample can return.
const double U_MIN_PERLIN_VALUE = -U_MAX_PERLIN_VALUE;
// The values that, alongside their negative inverses, encompass the middle N% likelihood of an average Perlin sample.
const double U_PERLIN_BENCHMARKS[PERCENTILES] = {0.0295, 0.0612, 0.0948, 0.130, 0.1683, 0.2101, 0.2572, 0.3182, 0.4040, U_MAX_PERLIN_VALUE};

// The maximum value each climate sample can return. 
const double U_MAX_CLIMATE_AMPLITUDES[NP_MAX] = {20./9  * U_MAX_PERLIN_VALUE, 320./189 * U_MAX_PERLIN_VALUE, 267./73 * U_MAX_PERLIN_VALUE,
										   75./31 * U_MAX_PERLIN_VALUE,  28./3   * U_MAX_PERLIN_VALUE,  20./7  * U_MAX_PERLIN_VALUE};
// The minimum value each climate sample can return.
const double U_MIN_CLIMATE_AMPLITUDES[NP_MAX] = {20./9  * U_MIN_PERLIN_VALUE, 320./189 * U_MIN_PERLIN_VALUE, 267./73 * U_MIN_PERLIN_VALUE,
										   75./31 * U_MIN_PERLIN_VALUE,  28./3   * U_MIN_PERLIN_VALUE,  20./7  * U_MIN_PERLIN_VALUE};

// The total number of Perlin octaves across all climates.
const int U_NUMBER_OF_OCTAVES = 46;
// The number of Perlin octaves each climate has.
// If a constant compile-time variant is needed instead, `sizeof(U_<MAX/MIN>_<climate>_OCTAVE_AMPLITUDE_SUMS)/sizeof(*U_<MAX/MIN>_<climate>_OCTAVE_AMPLITUDE_SUMS)`
// will return the same value.
const int U_CLIMATE_NUMBER_OF_OCTAVES[NP_MAX] = {4, 4, 18, 8, 6, 6};
// The BiomeNoise-set amplitude for each climate.
const double U_SET_CLIMATE_AMPLITUDES[NP_MAX] = {5./4, 10./9, 3./2, 25./18, 5./4, 5./4};

// The maximum value that all temperature Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MAX_TEMP_OCTAVE_AMPLITUDE_SUMS[4] = {80./63 * U_MAX_PERLIN_VALUE, 20./63 * U_MAX_PERLIN_VALUE, 10./63 * U_MAX_PERLIN_VALUE, 0};

// The minimum value that all temperature Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MIN_TEMP_OCTAVE_AMPLITUDE_SUMS[4] = {80./63 * U_MIN_PERLIN_VALUE, 20./63 * U_MIN_PERLIN_VALUE, 10./63 * U_MIN_PERLIN_VALUE, 0};

// The maximum value that all temperature Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MAX_HUMID_OCTAVE_AMPLITUDE_SUMS[4] = {640./567 * U_MAX_PERLIN_VALUE, 320./567 * U_MAX_PERLIN_VALUE, 160./567 * U_MAX_PERLIN_VALUE, 0};

// The minimum value that all temperature Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MIN_HUMID_OCTAVE_AMPLITUDE_SUMS[4] = {640./567 * U_MIN_PERLIN_VALUE, 320./567 * U_MIN_PERLIN_VALUE, 160./567 * U_MIN_PERLIN_VALUE, 0};

// The maximum value that all continentalness Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS[18] = {1485./511  * U_MAX_PERLIN_VALUE, 1101./511 * U_MAX_PERLIN_VALUE, 909./511 * U_MAX_PERLIN_VALUE, 717./511 * U_MAX_PERLIN_VALUE,
													   75./73   * U_MAX_PERLIN_VALUE,  333./511 * U_MAX_PERLIN_VALUE, 237./511 * U_MAX_PERLIN_VALUE, 141./511 * U_MAX_PERLIN_VALUE,
													   93./511  * U_MAX_PERLIN_VALUE,   45./511 * U_MAX_PERLIN_VALUE,  33./511 * U_MAX_PERLIN_VALUE,   3./73  * U_MAX_PERLIN_VALUE,
													   15./511  * U_MAX_PERLIN_VALUE,    9./511 * U_MAX_PERLIN_VALUE,   6./511 * U_MAX_PERLIN_VALUE,   3./511 * U_MAX_PERLIN_VALUE,
													    3./1022 * U_MAX_PERLIN_VALUE,    0};
// The minimum value that all continentalness Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MIN_CONT_OCTAVE_AMPLITUDE_SUMS[18] = {1485./511  * U_MIN_PERLIN_VALUE, 1101./511 * U_MIN_PERLIN_VALUE, 909./511 * U_MIN_PERLIN_VALUE, 717./511 * U_MIN_PERLIN_VALUE,
													   75./73   * U_MIN_PERLIN_VALUE,  333./511 * U_MIN_PERLIN_VALUE, 237./511 * U_MIN_PERLIN_VALUE, 141./511 * U_MIN_PERLIN_VALUE,
													   93./511  * U_MIN_PERLIN_VALUE,   45./511 * U_MIN_PERLIN_VALUE,  33./511 * U_MIN_PERLIN_VALUE,   3./73  * U_MIN_PERLIN_VALUE,
													   15./511  * U_MIN_PERLIN_VALUE,    9./511 * U_MIN_PERLIN_VALUE,   6./511 * U_MIN_PERLIN_VALUE,   3./511 * U_MIN_PERLIN_VALUE,
													    3./1022 * U_MIN_PERLIN_VALUE,    0};
// The maximum value that all erosion Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MAX_EROS_OCTAVE_AMPLITUDE_SUMS[8] = {475./279 * U_MAX_PERLIN_VALUE, 275./279 * U_MAX_PERLIN_VALUE, 175./279  * U_MAX_PERLIN_VALUE, 25./93  * U_MAX_PERLIN_VALUE,
													 50./279 * U_MAX_PERLIN_VALUE,  25./279 * U_MAX_PERLIN_VALUE,  25./558 * U_MAX_PERLIN_VALUE,   0};
// The minimum value that all erosion Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MIN_EROS_OCTAVE_AMPLITUDE_SUMS[8] = {475./279 * U_MIN_PERLIN_VALUE, 275./279 * U_MIN_PERLIN_VALUE, 175./279  * U_MIN_PERLIN_VALUE, 25./93  * U_MIN_PERLIN_VALUE,
													 50./279 * U_MIN_PERLIN_VALUE,  25./279 * U_MIN_PERLIN_VALUE,  25./558 * U_MIN_PERLIN_VALUE,   0};
// The maximum value that all shift Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
// Note that this is not the same as the Depth climate, which is not included here.
const double U_MAX_SHIFT_OCTAVE_AMPLITUDE_SUMS[6] = {20./3 * U_MAX_PERLIN_VALUE, 4 * U_MAX_PERLIN_VALUE, 8./3 * U_MAX_PERLIN_VALUE, 4./3 * U_MAX_PERLIN_VALUE,
													  2./3 * U_MAX_PERLIN_VALUE, 0};
// The minimum value that all shift Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
// Note that this is not the same as the Depth climate, which is not included here.
const double U_MIN_SHIFT_OCTAVE_AMPLITUDE_SUMS[6] = {20./3 * U_MIN_PERLIN_VALUE, 4 * U_MIN_PERLIN_VALUE, 8./3 * U_MIN_PERLIN_VALUE, 4./3 * U_MIN_PERLIN_VALUE,
													  2./3 * U_MIN_PERLIN_VALUE, 0};
// The maximum value that all weirdness Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MAX_WEIRD_OCTAVE_AMPLITUDE_SUMS[6] = {20./9  * U_MAX_PERLIN_VALUE, 100./63 * U_MAX_PERLIN_VALUE, 20./21 * U_MAX_PERLIN_VALUE, 20./63 * U_MAX_PERLIN_VALUE,
													 10./63 * U_MAX_PERLIN_VALUE, 0};
// The minimum value that all weirdness Perlin octaves after the `i`th one can return. (This is with the climate amplitude factored in.)
const double U_MIN_WEIRD_OCTAVE_AMPLITUDE_SUMS[6] = {20./9  * U_MIN_PERLIN_VALUE, 100./63 * U_MIN_PERLIN_VALUE, 20./21 * U_MIN_PERLIN_VALUE, 20./63 * U_MIN_PERLIN_VALUE,
													 10./63 * U_MIN_PERLIN_VALUE, 0};



int U_initClimateBoundsArray(const int climate, const double desiredClimateSample, const int percentile, double *array, const size_t arraySize) {
	static const double *CLIMATE_ARRAYS[] = {U_MAX_TEMP_OCTAVE_AMPLITUDE_SUMS,  U_MIN_TEMP_OCTAVE_AMPLITUDE_SUMS,
											 U_MAX_HUMID_OCTAVE_AMPLITUDE_SUMS, U_MIN_HUMID_OCTAVE_AMPLITUDE_SUMS,
											 U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS,  U_MIN_CONT_OCTAVE_AMPLITUDE_SUMS,
											 U_MAX_EROS_OCTAVE_AMPLITUDE_SUMS,  U_MIN_EROS_OCTAVE_AMPLITUDE_SUMS,
											 U_MAX_SHIFT_OCTAVE_AMPLITUDE_SUMS, U_MIN_SHIFT_OCTAVE_AMPLITUDE_SUMS,
											 U_MAX_WEIRD_OCTAVE_AMPLITUDE_SUMS, U_MIN_WEIRD_OCTAVE_AMPLITUDE_SUMS};
	const double *OCTAVE_RANGES_ARRAY = CLIMATE_ARRAYS[2*climate + (desiredClimateSample < 0)];
	int count = 0;
	for (size_t i = 0; i < min(arraySize, (size_t)U_CLIMATE_NUMBER_OF_OCTAVES[climate]); ++i, ++count) {
		array[i] = desiredClimateSample - OCTAVE_RANGES_ARRAY[i]*U_PERLIN_BENCHMARKS[percentile]/U_MAX_PERLIN_VALUE;
		if (climate == NP_SHIFT && U_CLIMATE_NUMBER_OF_OCTAVES[NP_SHIFT] + i < arraySize) {
			array[U_CLIMATE_NUMBER_OF_OCTAVES[NP_SHIFT] + i] = array[i];
			++count;
		}
	}
	return count;
}

void U_manualBNinit(BiomeNoise *bn) {
    if (bn->mc < MC_1_18) return;
	const int LB_MULT = 3*LARGE_BIOMES_FLAG + 1;

    bn->climate[NP_TEMPERATURE].amplitude = bn->climate[NP_SHIFT].amplitude = bn->climate[NP_WEIRDNESS].amplitude = 5./4;
	bn->climate[NP_HUMIDITY].amplitude = 10./9;
	bn->climate[NP_CONTINENTALNESS].amplitude = 3./2;
	bn->climate[NP_EROSION].amplitude = 25./18;
	bn->climate[NP_TEMPERATURE].octA.octaves = &bn->oct[0];
	bn->climate[NP_TEMPERATURE].octB.octaves = &bn->oct[2];
	bn->climate[NP_HUMIDITY].octA.octaves = &bn->oct[4];
	bn->climate[NP_HUMIDITY].octB.octaves = &bn->oct[6];
	bn->climate[NP_CONTINENTALNESS].octA.octaves = &bn->oct[8];
	bn->climate[NP_CONTINENTALNESS].octB.octaves = &bn->oct[17];
	bn->climate[NP_EROSION].octA.octaves = &bn->oct[26];
	bn->climate[NP_EROSION].octB.octaves = &bn->oct[30];
	bn->climate[NP_SHIFT].octA.octaves = &bn->oct[34];
	bn->climate[NP_SHIFT].octB.octaves = &bn->oct[37];
	bn->climate[NP_WEIRDNESS].octA.octaves = &bn->oct[40];
	bn->climate[NP_WEIRDNESS].octB.octaves = &bn->oct[43];
	bn->climate[NP_TEMPERATURE].octA.octcnt = bn->climate[NP_TEMPERATURE].octB.octcnt = bn->climate[NP_HUMIDITY].octA.octcnt = bn->climate[NP_HUMIDITY].octB.octcnt = 2;
	bn->climate[NP_CONTINENTALNESS].octA.octcnt = bn->climate[NP_CONTINENTALNESS].octB.octcnt = 9;
	bn->climate[NP_EROSION].octA.octcnt = bn->climate[NP_EROSION].octB.octcnt = 4;
	bn->climate[NP_SHIFT].octA.octcnt = bn->climate[NP_SHIFT].octB.octcnt = bn->climate[NP_WEIRDNESS].octA.octcnt = bn->climate[NP_WEIRDNESS].octB.octcnt = 3;
	// Temperature
	bn->oct[0].amplitude   = bn->oct[2].amplitude   = 16./21;
	bn->oct[0].lacunarity  = bn->oct[2].lacunarity  = 1./(1024*LB_MULT);
	bn->oct[1].amplitude   = bn->oct[3].amplitude   = 8./63;
	bn->oct[1].lacunarity  = bn->oct[3].lacunarity  = 1./(256*LB_MULT);
	// Humidity
	bn->oct[4].amplitude   = bn->oct[6].amplitude   = 32./63;
	bn->oct[4].lacunarity  = bn->oct[6].lacunarity  = 1./(256*LB_MULT);
	bn->oct[5].amplitude   = bn->oct[7].amplitude   = 16./63;
	bn->oct[5].lacunarity  = bn->oct[7].lacunarity  = 1./(128*LB_MULT);
	// Continentalness
	bn->oct[8].amplitude   = bn->oct[17].amplitude  = 256./511;
	bn->oct[8].lacunarity  = bn->oct[17].lacunarity = 1./(512*LB_MULT);
	bn->oct[9].amplitude   = bn->oct[18].amplitude  = bn->oct[10].amplitude = bn->oct[19].amplitude  = 128./511;
	bn->oct[9].lacunarity  = bn->oct[18].lacunarity = 1./(256*LB_MULT);
	bn->oct[10].lacunarity = bn->oct[19].lacunarity = 1./(128*LB_MULT);
	bn->oct[11].amplitude  = bn->oct[20].amplitude  = 64./511;
	bn->oct[11].lacunarity = bn->oct[20].lacunarity = 1./(64*LB_MULT);
	bn->oct[12].amplitude  = bn->oct[21].amplitude  = 32./511;
	bn->oct[12].lacunarity = bn->oct[21].lacunarity = 1./(32*LB_MULT);
	bn->oct[13].amplitude  = bn->oct[22].amplitude  = 8./511;
	bn->oct[13].lacunarity = bn->oct[22].lacunarity = 1./(16*LB_MULT);
	bn->oct[14].amplitude  = bn->oct[23].amplitude  = 4./511;
	bn->oct[14].lacunarity = bn->oct[23].lacunarity = 1./(8*LB_MULT);
	bn->oct[15].amplitude  = bn->oct[24].amplitude  = 2./511;
	bn->oct[15].lacunarity = bn->oct[24].lacunarity = 1./(4*LB_MULT);
	bn->oct[16].amplitude  = bn->oct[25].amplitude  = 1./511;
	bn->oct[16].lacunarity = bn->oct[25].lacunarity = 1./(2*LB_MULT);
	// Erosion
	bn->oct[26].amplitude  = bn->oct[30].amplitude  = 16./31;
	bn->oct[26].lacunarity = bn->oct[30].lacunarity = 1./(512*LB_MULT);
	bn->oct[27].amplitude  = bn->oct[31].amplitude  = 8./31;
	bn->oct[27].lacunarity = bn->oct[31].lacunarity = 1./(256*LB_MULT);
	bn->oct[28].amplitude  = bn->oct[32].amplitude  = 2./31;
	bn->oct[28].lacunarity = bn->oct[32].lacunarity = 1./(64*LB_MULT);
	bn->oct[29].amplitude  = bn->oct[33].amplitude  = 1./31;
	bn->oct[29].lacunarity = bn->oct[33].lacunarity = 1./(32*LB_MULT);
	// Shift
	bn->oct[34].amplitude  = bn->oct[37].amplitude  = 8./3;
	bn->oct[34].lacunarity = bn->oct[37].lacunarity = 1./8;
	bn->oct[35].amplitude  = bn->oct[38].amplitude  = 4./3;
	bn->oct[35].lacunarity = bn->oct[38].lacunarity = 1./4;
	bn->oct[36].amplitude  = bn->oct[39].amplitude  = 2./3;
	bn->oct[36].lacunarity = bn->oct[39].lacunarity = 1./2;
	// Weirdness
	bn->oct[40].amplitude  = bn->oct[43].amplitude  = bn->oct[41].amplitude = bn->oct[44].amplitude = 32./63;
	bn->oct[40].lacunarity = bn->oct[43].lacunarity = 1./128;
	bn->oct[41].lacunarity = bn->oct[44].lacunarity = 1./64;
	bn->oct[42].amplitude  = bn->oct[45].amplitude  = 8./63;
	bn->oct[42].lacunarity = bn->oct[45].lacunarity = 1./32;
}

void U_initPerlin(PerlinNoise *octave, Xoroshiro *xoroshiro) {
	octave->a = xNextDouble(xoroshiro) * 256.;
	octave->b = xNextDouble(xoroshiro) * 256.;
	octave->c = xNextDouble(xoroshiro) * 256.;
	uint8_t *idx = octave->d;
	for (int i = 0; i < 256; ++i) idx[i] = i;
	for (int i = 0; i < 255; ++i) {
		int j = xNextInt(xoroshiro, 256 - i) + i;
		uint_fast8_t n = idx[i];
		idx[i] = idx[j];
		idx[j] = n;
	}
	idx[256] = idx[0];
	double i2 = floor(octave->b);
	double d2 = octave->b - i2;
	octave->h2 = (int)i2;
	octave->d2 = d2;
	octave->t2 = d2*d2*d2 * (d2 * (d2*6. - 15.) + 10.);
}

void U_initClimate(const int climate, PerlinNoise *oct, const uint64_t seed) {
	double px = 0, pz = 0, climateSample;
	U_initAndSampleClimateBounded(climate, oct, &px, &pz, NULL, NULL, &seed, &climateSample);
}

void U_initAllClimates(PerlinNoise *oct, const uint64_t seed) {
	for (int climate = 0; climate < NP_MAX; ++climate) U_initClimate(climate, oct, seed);
}

// TODO: See if any way exists to specify `oct` as `const` while preserving deferral to `U_initAndSampleClimateBounded`, or see if it doesn't make a difference performance-wise
double U_sampleClimate(const int climate, PerlinNoise *oct, double *px, double *pz) {
	double climateSample;
	U_initAndSampleClimateBounded(climate, oct, px, pz, NULL, NULL, NULL, &climateSample);
	return climateSample;
}

// TODO: See if any way exists to specify `oct` as `const` while preserving deferral to `U_initAndSampleClimateBounded`, or see if it doesn't make a difference performance-wise
int U_sampleClimateBounded(const int climate, PerlinNoise *oct, double *px, double *pz, const double *lowerBounds, const double *upperBounds, double *climateSample) {
	return U_initAndSampleClimateBounded(climate, oct, px, pz, lowerBounds, upperBounds, NULL, climateSample);
}

int U_initAndSampleClimateBounded(const int climate, PerlinNoise *oct, double *px, double *pz, const double *lowerBounds, const double *upperBounds, const uint64_t *seedIfInitializingClimate, double *climateSample) {
	if (!climateSample && climate != NP_SHIFT) return 0;
	const int LB_MULT = 3*LARGE_BIOMES_FLAG + 1;
	const double OFF = 337./331;

	Xoroshiro pxr = {0, 0}, pxr2;
	uint64_t xloo, xhio, xlo = 0, xhi = 0, xlo2 = 0, xhi2 = 0;
	if (seedIfInitializingClimate) {
		xSetSeed(&pxr, *seedIfInitializingClimate);
		xloo = xNextLong(&pxr);
		xhio = xNextLong(&pxr);
	}
	switch (climate) {
		case NP_TEMPERATURE:
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x944b0073edf549db : 0x5c7e6b29735f0d7f);
            pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0x4ff44347e9d22b96 : 0xf7d86f1bbc734988); // md5 "minecraft:temperature_large" or "minecraft:temperature"
            xlo = xNextLong(&pxr);
            xhi = xNextLong(&pxr);
            pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
            U_initPerlin(&oct[0], &pxr2);
		}
		*climateSample = 20./21 * samplePerlin(&oct[0], *px/(1024*LB_MULT), 0, *pz/(1024*LB_MULT), 0, 0);
		if ((lowerBounds && *climateSample < lowerBounds[0]) || (upperBounds && *climateSample < lowerBounds[0])) return 0;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
            xhi2 = xNextLong(&pxr);
            pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
            U_initPerlin(&oct[2], &pxr2);
		}
		*climateSample += 20./21 * samplePerlin(&oct[2], *px/(1024*LB_MULT)*OFF, 0, *pz/(1024*LB_MULT)*OFF, 0, 0);
		if ((lowerBounds && *climateSample < lowerBounds[1]) || (upperBounds && *climateSample > upperBounds[1])) return 1;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[1], &pxr2);
		}
		*climateSample += 10./63 * samplePerlin(&oct[1], *px/(256*LB_MULT), 0, *pz/(256*LB_MULT), 0, 0);
		if ((lowerBounds && *climateSample < lowerBounds[2]) || (upperBounds && *climateSample > upperBounds[2])) return 2;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[3], &pxr2);
		}
		*climateSample += 10./63 * samplePerlin(&oct[3], *px/(256*LB_MULT)*OFF, 0, *pz/(256*LB_MULT)*OFF, 0, 0);
		return 3 + ((!lowerBounds || *climateSample > lowerBounds[3]) && (!upperBounds || *climateSample < upperBounds[3]));

		case NP_HUMIDITY:
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x71b8ab943dbd5301 : 0x81bb4d22e8dc168e);
            pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xbb63ddcf39ff7a2b : 0xf1c8b4bea16303cd); // md5 "minecraft:vegetation_large" or "minecraft:vegetation"
            xlo = xNextLong(&pxr);
            xhi = xNextLong(&pxr);
            pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[4], &pxr2);
		}
		*climateSample = 320./567 * samplePerlin(&oct[4], *px/(256*LB_MULT), 0, *pz/(256*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[0]) || (upperBounds && *climateSample > upperBounds[0])) return 0;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
            xhi2 = xNextLong(&pxr);
            pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[6], &pxr2);
		}
		*climateSample += 320./567 * samplePerlin(&oct[6], *px/(256*LB_MULT)*OFF, 0, *pz/(256*LB_MULT)*OFF, 0, 0);
		if ((lowerBounds && *climateSample < lowerBounds[1]) || (upperBounds && *climateSample > upperBounds[1])) return 1;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
            U_initPerlin(&oct[5], &pxr2);
		}
		*climateSample += 160./567 * samplePerlin(&oct[5], *px/(128*LB_MULT), 0, *pz/(128*LB_MULT), 0, 0);
		if ((lowerBounds && *climateSample < lowerBounds[2]) || (upperBounds && *climateSample > upperBounds[2])) return 2;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
            U_initPerlin(&oct[7], &pxr2);
		}
		*climateSample += 160./567 * samplePerlin(&oct[7], *px/(128*LB_MULT)*OFF, 0, *pz/(128*LB_MULT)*OFF, 0, 0);
		return 3 + ((!lowerBounds || *climateSample > lowerBounds[3]) && (!upperBounds || *climateSample < upperBounds[3]));

		case NP_CONTINENTALNESS:
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x9a3f51a113fce8dc : 0x83886c9d0ae3a662);
			pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xee2dbd157e5dcdad : 0xafa638a61b42e8ad); // md5 "minecraft:continentalness_large" or "minecraft:continentalness"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			U_initPerlin(&oct[8], &pxr2);
		}
		*climateSample = 384./511 * samplePerlin(&oct[8], *px/(512*LB_MULT), 0, *pz/(512*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[0]) || (upperBounds && *climateSample > upperBounds[0])) return 0;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			U_initPerlin(&oct[17], &pxr2);
		}
		*climateSample += 384./511 * samplePerlin(&oct[17], *px/(512*LB_MULT)*OFF, 0, *pz/(512*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[1]) || (upperBounds && *climateSample > upperBounds[1])) return 1;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			U_initPerlin(&oct[9], &pxr2);
		}
		*climateSample += 192./511 * samplePerlin(&oct[9], *px/(256*LB_MULT), 0, *pz/(256*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[2]) || (upperBounds && *climateSample > upperBounds[2])) return 2;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			U_initPerlin(&oct[18], &pxr2);
		}
		*climateSample += 192./511 * samplePerlin(&oct[18], *px/(256*LB_MULT)*OFF, 0, *pz/(256*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[3]) || (upperBounds && *climateSample > upperBounds[3])) return 3;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			U_initPerlin(&oct[10], &pxr2);
		}
		*climateSample += 192./511 * samplePerlin(&oct[10], *px/(128*LB_MULT), 0, *pz/(128*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[4]) || (upperBounds && *climateSample > upperBounds[4])) return 4;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			U_initPerlin(&oct[19], &pxr2);
		}
		*climateSample += 192./511 * samplePerlin(&oct[19], *px/(128*LB_MULT)*OFF, 0, *pz/(128*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[5]) || (upperBounds && *climateSample > upperBounds[5])) return 5;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			U_initPerlin(&oct[11], &pxr2);
		}
		*climateSample += 96./511 * samplePerlin(&oct[11], *px/(64*LB_MULT), 0, *pz/(64*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[6]) || (upperBounds && *climateSample > upperBounds[6])) return 6;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			U_initPerlin(&oct[20], &pxr2);
		}
		*climateSample += 96./511 * samplePerlin(&oct[20], *px/(64*LB_MULT)*OFF, 0, *pz/(64*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[7]) || (upperBounds && *climateSample > upperBounds[7])) return 7;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			U_initPerlin(&oct[12], &pxr2);
		}
		*climateSample += 48./511 * samplePerlin(&oct[12], *px/(32*LB_MULT), 0, *pz/(32*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[8]) || (upperBounds && *climateSample > upperBounds[8])) return 8;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			U_initPerlin(&oct[21], &pxr2);
		}
		*climateSample += 48./511 * samplePerlin(&oct[21], *px/(32*LB_MULT)*OFF, 0, *pz/(32*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[9]) || (upperBounds && *climateSample > upperBounds[9])) return 9;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			U_initPerlin(&oct[13], &pxr2);
		}
		*climateSample += 12./511 * samplePerlin(&oct[13], *px/(16*LB_MULT), 0, *pz/(16*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[10]) || (upperBounds && *climateSample > upperBounds[10])) return 10;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			U_initPerlin(&oct[22], &pxr2);
		}
		*climateSample += 12./511 * samplePerlin(&oct[22], *px/(16*LB_MULT)*OFF, 0, *pz/(16*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[11]) || (upperBounds && *climateSample > upperBounds[11])) return 11;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			U_initPerlin(&oct[14], &pxr2);
		}
		*climateSample += 6./511 * samplePerlin(&oct[14], *px/(8*LB_MULT), 0, *pz/(8*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[12]) || (upperBounds && *climateSample > upperBounds[12])) return 12;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			U_initPerlin(&oct[23], &pxr2);
		}
		*climateSample += 6./511 * samplePerlin(&oct[23], *px/(8*LB_MULT)*OFF, 0, *pz/(8*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[13]) || (upperBounds && *climateSample > upperBounds[13])) return 13;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			U_initPerlin(&oct[15], &pxr2);
		}
		*climateSample += 3./511 * samplePerlin(&oct[15], *px/(4*LB_MULT), 0, *pz/(4*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[14]) || (upperBounds && *climateSample > upperBounds[14])) return 14;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			U_initPerlin(&oct[24], &pxr2);
		}
		*climateSample += 3./511 * samplePerlin(&oct[24], *px/(4*LB_MULT)*OFF, 0, *pz/(4*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[15]) || (upperBounds && *climateSample > upperBounds[15])) return 15;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			U_initPerlin(&oct[16], &pxr2);
		}
		*climateSample += 3./1022 * samplePerlin(&oct[16], *px/(2*LB_MULT), 0, *pz/(2*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[16]) || (upperBounds && *climateSample > upperBounds[16])) return 16;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			U_initPerlin(&oct[25], &pxr2);
		}
		*climateSample += 3./1022 * samplePerlin(&oct[25], *px/(2*LB_MULT)*OFF, 0, *pz/(2*LB_MULT)*OFF, 0, 0);
		return 17 + ((!lowerBounds || *climateSample > lowerBounds[17]) && (!upperBounds || *climateSample < upperBounds[17]));

		case NP_EROSION:
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x8c984b1f8702a951 : 0xd02491e6058f6fd8);
            pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xead7b1f92bae535f : 0x4792512c94c17a80); // md5 "minecraft:erosion_large" or "minecraft:erosion"
            xlo = xNextLong(&pxr);
            xhi = xNextLong(&pxr);
            pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
            U_initPerlin(&oct[26], &pxr2);
		}
		*climateSample = 200./279 * samplePerlin(&oct[26], *px/(512*LB_MULT), 0, *pz/(512*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[0]) || (upperBounds && *climateSample > upperBounds[0])) return 0;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
            xhi2 = xNextLong(&pxr);
            pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
            U_initPerlin(&oct[30], &pxr2);
		}
		*climateSample += 200./279 * samplePerlin(&oct[30], *px/(512*LB_MULT)*OFF, 0, *pz/(512*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[1]) || (upperBounds && *climateSample > upperBounds[1])) return 1;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[27], &pxr2);
		}
		*climateSample += 100./279 * samplePerlin(&oct[27], *px/(256*LB_MULT), 0, *pz/(256*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[2]) || (upperBounds && *climateSample > upperBounds[2])) return 2;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
            U_initPerlin(&oct[31], &pxr2);
		}
		*climateSample += 100./279 * samplePerlin(&oct[31], *px/(256*LB_MULT)*OFF, 0, *pz/(256*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[3]) || (upperBounds && *climateSample > upperBounds[3])) return 3;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
            U_initPerlin(&oct[28], &pxr2);
		}
		*climateSample += 25./279 * samplePerlin(&oct[28], *px/(64*LB_MULT), 0, *pz/(64*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[4]) || (upperBounds && *climateSample > upperBounds[4])) return 4;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
            U_initPerlin(&oct[32], &pxr2);
		}
		*climateSample += 25./279 * samplePerlin(&oct[32], *px/(64*LB_MULT)*OFF, 0, *pz/(64*LB_MULT)*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[5]) || (upperBounds && *climateSample > upperBounds[5])) return 5;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
            pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
            U_initPerlin(&oct[29], &pxr2);
		}
		*climateSample += 25./558 * samplePerlin(&oct[29], *px/(32*LB_MULT), 0, *pz/(32*LB_MULT), 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[6]) || (upperBounds && *climateSample > upperBounds[6])) return 6;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
            pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
            U_initPerlin(&oct[33], &pxr2);
		}
		*climateSample += 25./558 * samplePerlin(&oct[33], *px/(32*LB_MULT)*OFF, 0, *pz/(32*LB_MULT)*OFF, 0, 0);
		return 7 + ((!lowerBounds || *climateSample > lowerBounds[7]) && (!upperBounds || *climateSample < upperBounds[7]));

		case NP_SHIFT: *px = floor(*px/4), *pz = floor(*pz/4);
		const double origPx = *px, origPz = *pz;
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ 0x080518cf6af25384;
			pxr.hi = xhio ^ 0x3f3dfb40a54febd5; // md5 "minecraft:offset"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			pxr2.lo = xlo ^ 0x53d39c6752dac858;
			pxr2.hi = xhi ^ 0xbcd1c5a80ab65b3e; // md5 "octave_-3"
			U_initPerlin(&oct[34], &pxr2);
		}
		*px += 8./3 * samplePerlin(&oct[34], origPx/8., 0, origPz/8., 0, 0);
        if ((lowerBounds && *px < lowerBounds[0]) || (upperBounds && *px > upperBounds[0])) return 0;
		*pz += 8./3 * samplePerlin(&oct[34], origPz/8., origPx/8., 0, 0, 0);
        if ((lowerBounds && *pz < lowerBounds[6]) || (upperBounds && *pz > upperBounds[6])) return 1;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);
			pxr2.lo = xlo2 ^ 0x53d39c6752dac858;
			pxr2.hi = xhi2 ^ 0xbcd1c5a80ab65b3e; // md5 "octave_-3"
			U_initPerlin(&oct[37], &pxr2);
		}
		*px += 8./3 * samplePerlin(&oct[37], origPx/8.*OFF, 0, origPz/8.*OFF, 0, 0);
        if ((lowerBounds && *px < lowerBounds[1]) || (upperBounds && *px > upperBounds[1])) return 2;
		*pz += 8./3 * samplePerlin(&oct[37], origPz/8.*OFF, origPx/8.*OFF, 0, 0, 0);
        if ((lowerBounds && *pz < lowerBounds[7]) || (upperBounds && *pz > upperBounds[7])) return 3;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ 0xb4a24d7a84e7677b;
			pxr2.hi = xhi ^ 0x023ff9668e89b5c4; // md5 "octave_-2"
			U_initPerlin(&oct[35], &pxr2);
		}
		*px += 4./3 * samplePerlin(&oct[35], origPx/4., 0, origPz/4., 0, 0);
        if ((lowerBounds && *px < lowerBounds[2]) || (upperBounds && *px > upperBounds[2])) return 4;
		*pz += 4./3 * samplePerlin(&oct[35], origPz/4., origPx/4., 0, 0, 0);
        if ((lowerBounds && *pz < lowerBounds[8]) || (upperBounds && *pz > upperBounds[8])) return 5;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ 0xb4a24d7a84e7677b;
			pxr2.hi = xhi2 ^ 0x023ff9668e89b5c4; // md5 "octave_-2"
			U_initPerlin(&oct[38], &pxr2);
		}
		*px += 4./3 * samplePerlin(&oct[38], origPx/4.*OFF, 0, origPz/4.*OFF, 0, 0);
        if ((lowerBounds && *px < lowerBounds[3]) || (upperBounds && *px > upperBounds[3])) return 6;
		*pz += 4./3 * samplePerlin(&oct[38], origPz/4.*OFF, origPx/4.*OFF, 0, 0, 0);
        if ((lowerBounds && *pz < lowerBounds[9]) || (upperBounds && *pz > upperBounds[9])) return 7;
		
		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ 0xdffa22b534c5f608;
			pxr2.hi = xhi ^ 0xb9b67517d3665ca9; // md5 "octave_-1"
			U_initPerlin(&oct[36], &pxr2);
		}
		*px += 2./3 * samplePerlin(&oct[36], origPx/2., 0, origPz/2., 0, 0);
        if ((lowerBounds && *px < lowerBounds[4]) || (upperBounds && *px > upperBounds[4])) return 8;
		*pz += 2./3 * samplePerlin(&oct[36], origPz/2., origPx/2., 0, 0, 0);
        if ((lowerBounds && *pz < lowerBounds[10]) || (upperBounds && *pz > upperBounds[10])) return 9;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ 0xdffa22b534c5f608;
			pxr2.hi = xhi2 ^ 0xb9b67517d3665ca9; // md5 "octave_-1"
			U_initPerlin(&oct[39], &pxr2);
		}
		*px += 2./3 * samplePerlin(&oct[39], origPx/2.*OFF, 0, origPz/2.*OFF, 0, 0);
		if ((lowerBounds && *px < lowerBounds[5]) || (upperBounds && *px > upperBounds[5])) return 10;
		*pz += 2./3 * samplePerlin(&oct[39], origPz/2.*OFF, origPx/2.*OFF, 0, 0, 0);
		return 11 + ((!lowerBounds || *pz > lowerBounds[11]) && (!upperBounds || *pz < upperBounds[11]));

		case NP_WEIRDNESS:
		if (seedIfInitializingClimate) {
			pxr.lo = xloo ^ 0xefc8ef4d36102b34;
            pxr.hi = xhio ^ 0x1beeeb324a0f24ea; // md5 "minecraft:ridge"
            xlo = xNextLong(&pxr);
            xhi = xNextLong(&pxr);
            pxr2.lo = xlo ^ 0xf11268128982754f;
            pxr2.hi = xhi ^ 0x257a1d670430b0aa; // md5 "octave_-7"
            U_initPerlin(&oct[40], &pxr2);
		}
		*climateSample = 40./63 * samplePerlin(&oct[40], *px/128., 0, *pz/128., 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[0]) || (upperBounds && *climateSample > upperBounds[0])) return 0;

		if (seedIfInitializingClimate) {
			xlo2 = xNextLong(&pxr);
            xhi2 = xNextLong(&pxr);
            pxr2.lo = xlo2 ^ 0xf11268128982754f;
            pxr2.hi = xhi2 ^ 0x257a1d670430b0aa; // md5 "octave_-7"
            U_initPerlin(&oct[43], &pxr2);
		}
		*climateSample += 40./63 * samplePerlin(&oct[43], *px/128.*OFF, 0, *pz/128.*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[1]) || (upperBounds && *climateSample > upperBounds[1])) return 1;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ 0xe51c98ce7d1de664;
            pxr2.hi = xhi ^ 0x5f9478a733040c45; // md5 "octave_-6"
            U_initPerlin(&oct[41], &pxr2);
		}
		*climateSample += 40./63 * samplePerlin(&oct[41], *px/64., 0, *pz/64., 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[2]) || (upperBounds && *climateSample > upperBounds[2])) return 2;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ 0xe51c98ce7d1de664;
            pxr2.hi = xhi2 ^ 0x5f9478a733040c45; // md5 "octave_-6"
            U_initPerlin(&oct[44], &pxr2);
		}
		*climateSample += 40./63 * samplePerlin(&oct[44], *px/64.*OFF, 0, *pz/64.*OFF, 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[3]) || (upperBounds && *climateSample > upperBounds[3])) return 3;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo ^ 0x6d7b49e7e429850a;
            pxr2.hi = xhi ^ 0x2e3063c622a24777; // md5 "octave_-5"
            U_initPerlin(&oct[42], &pxr2);
		}
		*climateSample += 10./63 * samplePerlin(&oct[42], *px/32., 0, *pz/32., 0, 0);
        if ((lowerBounds && *climateSample < lowerBounds[4]) || (upperBounds && *climateSample > upperBounds[4])) return 4;

		if (seedIfInitializingClimate) {
			pxr2.lo = xlo2 ^ 0x6d7b49e7e429850a;
            pxr2.hi = xhi2 ^ 0x2e3063c622a24777; // md5 "octave_-5"
            U_initPerlin(&oct[45], &pxr2);
		}
		*climateSample += 10./63 * samplePerlin(&oct[45], *px/32.*OFF, 0, *pz/32.*OFF, 0, 0);
		return 5 + ((!lowerBounds || *climateSample > lowerBounds[5]) && (!upperBounds || *climateSample < upperBounds[5]));
	}
	return 0;
}