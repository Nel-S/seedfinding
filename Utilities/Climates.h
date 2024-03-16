#include "cubiomes/finders.h"
#include <stdbool.h>

extern const bool LARGE_BIOMES_FLAG;

// pi/2pi.
const double U_PI     = 3.1415926535897932384626433;
const double U_TWO_PI = 6.2831853071795864769252866;

// The maximum value that a Perlin sample can return.
const double U_MAX_PERLIN_VALUE = 1.0363538112118025;
// The minimum value that a Perlin sample can return.
const double U_MIN_PERLIN_VALUE = -U_MAX_PERLIN_VALUE;
// The maximum value each climate sample can return.
const double U_MAX_CLIMATE_AMPLITUDES[] = {20./9  * U_MAX_PERLIN_VALUE, 320./189 * U_MAX_PERLIN_VALUE, 267./73 * U_MAX_PERLIN_VALUE,
										   75./31 * U_MAX_PERLIN_VALUE,   7./3   * U_MAX_PERLIN_VALUE,  20./7  * U_MAX_PERLIN_VALUE};
// The minimum value each climate sample can return.
const double U_MIN_CLIMATE_AMPLITUDES[] = {20./9  * U_MIN_PERLIN_VALUE, 320./189 * U_MIN_PERLIN_VALUE, 267./73 * U_MIN_PERLIN_VALUE,
										   75./31 * U_MIN_PERLIN_VALUE,   7./3   * U_MIN_PERLIN_VALUE,  20./7  * U_MIN_PERLIN_VALUE};

// The BiomeNoise-set amplitude for each climate.
const double U_SET_CLIMATE_AMPLITUDES[] = {5./4, 10./9, 3./2, 25./18, 5./4, 5./4};

// The maximum value that all continentalness Perlin octaves after the ith one can return.
const double U_MAX_CONT_OCTAVE_AMPLITUDE_SUMS[] = {990./511 * U_MAX_PERLIN_VALUE, 734./511 * U_MAX_PERLIN_VALUE, 606./511 * U_MAX_PERLIN_VALUE, 478./511 * U_MAX_PERLIN_VALUE,
													50./73  * U_MAX_PERLIN_VALUE, 222./511 * U_MAX_PERLIN_VALUE, 158./511 * U_MAX_PERLIN_VALUE,  94./511 * U_MAX_PERLIN_VALUE,
													62./511 * U_MAX_PERLIN_VALUE,  30./511 * U_MAX_PERLIN_VALUE,  22./511 * U_MAX_PERLIN_VALUE,   2./73  * U_MAX_PERLIN_VALUE,
													10./511 * U_MAX_PERLIN_VALUE,   6./511 * U_MAX_PERLIN_VALUE,   4./511 * U_MAX_PERLIN_VALUE,   2./511 * U_MAX_PERLIN_VALUE,
													 1./511 * U_MAX_PERLIN_VALUE,   0};



// Clone of Cubiomes' xPerlinInit() that doesn't mess with the octave's amplitudes/lacunarities.
static inline void U_initPerlin(PerlinNoise *octave, Xoroshiro *xoroshiro) {
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

// Initializes several 1.18+ BiomeNoise constants that Cubiomes chooses to regenerate each time instead of hardcoding (https://github.com/Cubitect/cubiomes/issues/82).
// Note that Cubiomes' xPerlinInit() will wipe out the amplitudes and lacunarities; use U_initPerlin() instead.
void U_manualBNinit(BiomeNoise *bn) {
    if (bn->mc < MC_1_18) return;
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
	bn->oct[0].lacunarity  = bn->oct[2].lacunarity  = 1./(LARGE_BIOMES_FLAG ? 4096 : 1024);
	bn->oct[1].amplitude   = bn->oct[3].amplitude   = 8./63;
	bn->oct[1].lacunarity  = bn->oct[3].lacunarity  = 1./(LARGE_BIOMES_FLAG ? 1024 : 256);
	// Humidity
	bn->oct[4].amplitude   = bn->oct[6].amplitude   = 32./63;
	bn->oct[4].lacunarity  = bn->oct[6].lacunarity  = 1./(LARGE_BIOMES_FLAG ? 1024 : 256);
	bn->oct[5].amplitude   = bn->oct[7].amplitude   = 16./63;
	bn->oct[5].lacunarity  = bn->oct[7].lacunarity  = 1./(LARGE_BIOMES_FLAG ? 512 : 128);
	// Continentalness
	bn->oct[8].amplitude   = bn->oct[17].amplitude  = 256./511;
	bn->oct[8].lacunarity  = bn->oct[17].lacunarity = 1./(LARGE_BIOMES_FLAG ? 2048 : 512);
	bn->oct[9].amplitude   = bn->oct[18].amplitude  = bn->oct[10].amplitude = bn->oct[19].amplitude  = 128./511;
	bn->oct[9].lacunarity  = bn->oct[18].lacunarity = 1./(LARGE_BIOMES_FLAG ? 1024 : 256);
	bn->oct[10].lacunarity = bn->oct[19].lacunarity = 1./(LARGE_BIOMES_FLAG ? 512 : 128);
	bn->oct[11].amplitude  = bn->oct[20].amplitude  = 64./511;
	bn->oct[11].lacunarity = bn->oct[20].lacunarity = 1./(LARGE_BIOMES_FLAG ? 256 : 64);
	bn->oct[12].amplitude  = bn->oct[21].amplitude  = 32./511;
	bn->oct[12].lacunarity = bn->oct[21].lacunarity = 1./(LARGE_BIOMES_FLAG ? 128 : 32);
	bn->oct[13].amplitude  = bn->oct[22].amplitude  = 8./511;
	bn->oct[13].lacunarity = bn->oct[22].lacunarity = 1./(LARGE_BIOMES_FLAG ? 64 : 16);
	bn->oct[14].amplitude  = bn->oct[23].amplitude  = 4./511;
	bn->oct[14].lacunarity = bn->oct[23].lacunarity = 1./(LARGE_BIOMES_FLAG ? 32 : 8);
	bn->oct[15].amplitude  = bn->oct[24].amplitude  = 2./511;
	bn->oct[15].lacunarity = bn->oct[24].lacunarity = 1./(LARGE_BIOMES_FLAG ? 16 : 4);
	bn->oct[16].amplitude  = bn->oct[25].amplitude  = 1./511;
	bn->oct[16].lacunarity = bn->oct[25].lacunarity = 1./(LARGE_BIOMES_FLAG ? 8 : 2);
	// Erosion
	bn->oct[26].amplitude  = bn->oct[30].amplitude  = 16./31;
	bn->oct[26].lacunarity = bn->oct[30].lacunarity = 1./(LARGE_BIOMES_FLAG ? 2048 : 512);
	bn->oct[27].amplitude  = bn->oct[31].amplitude  = 8./31;
	bn->oct[27].lacunarity = bn->oct[31].lacunarity = 1./(LARGE_BIOMES_FLAG ? 1024 : 256);
	bn->oct[28].amplitude  = bn->oct[32].amplitude  = 2./31;
	bn->oct[28].lacunarity = bn->oct[32].lacunarity = 1./(LARGE_BIOMES_FLAG ? 256 : 64);
	bn->oct[29].amplitude  = bn->oct[33].amplitude  = 1./31;
	bn->oct[29].lacunarity = bn->oct[33].lacunarity = 1./(LARGE_BIOMES_FLAG ? 128 : 32);
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

// 
double U_sampleClimate(const int climate, const PerlinNoise *oct, double *px, double *pz) {
	switch (climate) {
		case NP_TEMPERATURE:
		return 16./21 * (samplePerlin(&oct[0], *px/(LARGE_BIOMES_FLAG ? 4096. : 1024.), 0, *pz/(LARGE_BIOMES_FLAG ? 4096. : 1024.), 0, 0)
					   + samplePerlin(&oct[2], *px * (LARGE_BIOMES_FLAG ? 337./1355776 : 337./(331*1024)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./1355776 : 337./(331*1024)), 0, 0))
			 +  8./63 * (samplePerlin(&oct[1], *px/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, *pz/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, 0)
					   + samplePerlin(&oct[3], *px * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, 0));
		case NP_HUMIDITY:
		return 32./63 * (samplePerlin(&oct[4], *px/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, *pz/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, 0)
                       + samplePerlin(&oct[6], *px * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, 0))
             + 16./63 * (samplePerlin(&oct[5], *px/(LARGE_BIOMES_FLAG ? 512. : 128.), 0, *pz/(LARGE_BIOMES_FLAG ? 512. : 128.), 0, 0)
                       + samplePerlin(&oct[7], *px * (LARGE_BIOMES_FLAG ? 337./(331*512) : 337./(331*128)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*512) : 337./(331*128)), 0, 0));
		case NP_CONTINENTALNESS:
		return 256./511 * (samplePerlin(&oct[8], *px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, *pz/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, 0)
						 + samplePerlin(&oct[17], *px * (LARGE_BIOMES_FLAG ? 337./(331*2048) : 337./(331*512)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*2048) : 337./(331*512)), 0, 0))
			 + 128./511 * (samplePerlin(&oct[9], *px/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, *pz/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, 0)
						 + samplePerlin(&oct[18], *px * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, 0)
						 + samplePerlin(&oct[10], *px/(LARGE_BIOMES_FLAG ? 512. : 128.), 0, *pz/(LARGE_BIOMES_FLAG ? 512. : 128.), 0, 0)
						 + samplePerlin(&oct[19], *px * (LARGE_BIOMES_FLAG ? 337./(331*512) : 337./(331*128)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*512) : 337./(331*128)), 0, 0))
			 +  64./511 * (samplePerlin(&oct[11], *px/(LARGE_BIOMES_FLAG ? 256. : 64.), 0, *pz/(LARGE_BIOMES_FLAG ? 256. : 64.), 0, 0)
						 + samplePerlin(&oct[20], *px * (LARGE_BIOMES_FLAG ? 337./(331*256) : 337./(331*64)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*256) : 337./(331*64)), 0, 0))
			 +  32./511 * (samplePerlin(&oct[12], *px/(LARGE_BIOMES_FLAG ? 128. : 32.), 0, *pz/(LARGE_BIOMES_FLAG ? 128. : 32.), 0, 0)
						 + samplePerlin(&oct[21], *px * (LARGE_BIOMES_FLAG ? 337./(331*128) : 337./(331*32)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*128) : 337./(331*32)), 0, 0))
			 +   8./511 * (samplePerlin(&oct[13], *px/(LARGE_BIOMES_FLAG ? 64. : 16.), 0, *pz/(LARGE_BIOMES_FLAG ? 64. : 16.), 0, 0)
						 + samplePerlin(&oct[22], *px * (LARGE_BIOMES_FLAG ? 337./(331*64) : 337./(331*16)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*64) : 337./(331*16)), 0, 0))
			 +   4./511 * (samplePerlin(&oct[14], *px/(LARGE_BIOMES_FLAG ? 32. : 8.), 0, *pz/(LARGE_BIOMES_FLAG ? 32. : 8.), 0, 0)
						 + samplePerlin(&oct[23], *px * (LARGE_BIOMES_FLAG ? 337./(331*32) : 337./(331*8)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*32) : 337./(331*8)), 0, 0))
			 +   2./511 * (samplePerlin(&oct[15], *px/(LARGE_BIOMES_FLAG ? 16. : 4.), 0, *pz/(LARGE_BIOMES_FLAG ? 16. : 4.), 0, 0)
						 + samplePerlin(&oct[24], *px * (LARGE_BIOMES_FLAG ? 337./(331*16) : 337./(331*4)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*16) : 337./(331*4)), 0, 0))
			 +   1./511 * (samplePerlin(&oct[16], *px/(LARGE_BIOMES_FLAG ? 8. : 2.), 0, *pz/(LARGE_BIOMES_FLAG ? 8. : 2.), 0, 0)
						 + samplePerlin(&oct[25], *px * (LARGE_BIOMES_FLAG ? 337./(331*8) : 337./(331*2)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*8) : 337./(331*2)), 0, 0));
		case NP_EROSION:
		return 16./31 * (samplePerlin(&oct[26], *px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, *pz/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, 0)
					   + samplePerlin(&oct[30], *px * (LARGE_BIOMES_FLAG ? 337./(331*2048) : 337./(331*512)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*2048) : 337./(331*512)), 0, 0))
			 +  8./31 * (samplePerlin(&oct[27], *px/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, *pz/(LARGE_BIOMES_FLAG ? 1024. : 256.), 0, 0)
					   + samplePerlin(&oct[31], *px * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*1024) : 337./(331*256)), 0, 0))
			 +  2./31 * (samplePerlin(&oct[28], *px/(LARGE_BIOMES_FLAG ? 256. : 64.), 0, *pz/(LARGE_BIOMES_FLAG ? 256. : 64.), 0, 0)
					   + samplePerlin(&oct[32], *px * (LARGE_BIOMES_FLAG ? 337./(331*256) : 337./(331*64)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*256) : 337./(331*64)), 0, 0))
			 +  1./31 * (samplePerlin(&oct[29], *px/(LARGE_BIOMES_FLAG ? 128. : 32.), 0, *pz/(LARGE_BIOMES_FLAG ? 128. : 32.), 0, 0)
					   + samplePerlin(&oct[33], *px * (LARGE_BIOMES_FLAG ? 337./(331*128) : 337./(331*32)), 0, *pz * (LARGE_BIOMES_FLAG ? 337./(331*128) : 337./(331*32)), 0, 0));
		case NP_SHIFT:
		*px += 8./3 * (samplePerlin(&oct[34], *px/8., 0, *pz/8., 0, 0) + samplePerlin(&oct[37], *px*337./(331*8), 0, *pz*337./(331*8), 0, 0))
			 + 4./3 * (samplePerlin(&oct[35], *px/4., 0, *pz/4., 0, 0) + samplePerlin(&oct[38], *px*337./(331*4), 0, *pz*337./(331*4), 0, 0))
			 + 2./3 * (samplePerlin(&oct[36], *px/2., 0, *pz/2., 0, 0) + samplePerlin(&oct[39], *px*337./(331*2), 0, *pz*337./(331*2), 0, 0));
		*pz += 8./3 * (samplePerlin(&oct[34], *pz/8., *px/8., 0, 0, 0) + samplePerlin(&oct[37], *pz*337./(331*8), *px*337./(331*8), 0, 0, 0))
			 + 4./3 * (samplePerlin(&oct[35], *pz/4., *px/4., 0, 0, 0) + samplePerlin(&oct[38], *pz*337./(331*4), *px*337./(331*4), 0, 0, 0))
			 + 2./3 * (samplePerlin(&oct[36], *pz/2., *px/2., 0, 0, 0) + samplePerlin(&oct[39], *pz*337./(331*2), *px*337./(331*2), 0, 0, 0));
		return 0;
		case NP_WEIRDNESS:
		return 32./63 * (samplePerlin(&oct[40], *px/128., 0, *pz/128., 0, 0) + samplePerlin(&oct[43], *px * 337./(331*128), 0, *pz * 337./(331*128), 0, 0)
					   + samplePerlin(&oct[41], *px/64., 0, *pz/64., 0, 0)   + samplePerlin(&oct[44], *px * 337./(331*64), 0, *pz * 337./(331*64), 0, 0))
			 +  8./63 * (samplePerlin(&oct[42], *px/32., 0, *pz/32., 0, 0)   + samplePerlin(&oct[45], *px * 337./(331*32), 0, *pz * 337./(331*32), 0, 0));
	}
	return INFINITY;
}