/*
	A program to iteratively find Minecraft seeds with octave b-values as close to 0.5 as possible.
	Since the b-values are the only octave doubles that remain unchanged no matter what coordinates one samples the Perlin values at for that octave,
	seeds with relevant octave b-values close to 0.5 will be more likely to contain extreme climate values, both positive and negative, than normal.

	Output is of the form [Seed] ([0 <= Offset percentage <= 1]).

	The number of seeds to check, the climates to check, and whether to use Large Biomes or not can all be adjusted below.
	For multithreading purposes, the number of threads to use can also be specified below.

	All Xoroshiro-related code was adapted from Cubitect's cubiomes library (https://github.com/Cubitect/cubiomes, released under GPL-3),
	which are in turn a C representation of certain operations performed in Java by Minecraft, which is property of Mojang AB.
	Xoroshiro128++ itself was developed by David Blackman and Sebastiano Vigna in 2019 (https://prng.di.unimi.it/xoroshiro128plusplus.c)
	and is in the public domain.
	The code for initializing multiple threads is based on that from a blog post by Robert A. McDougal (https://ramcdougal.com/threads.html).
*/

#include "../../cubiomes/layers.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

//Enumeration of climate types for choosing which climates to sample
enum {TEMPERATURE = 0b10000, HUMIDITY = 0b1000, CONTINENTALNESS = 0b100, EROSION = 0b10, WEIRDNESS = 0b1};

/*
	FLAGS AND SETTINGS
*/
//The climates whose octaves to check.
const int CLIMATES_TO_CHECK = TEMPERATURE;
const int OCTAVES_TO_MAX = 4;
//For multi-core computers: The number of threads to use when running.
const int NUMBER_OF_THREADS = 4;
//Flag: 1 indicates Large Biomes generation, 0 indicates normal generation.
const int LARGE_BIOMES_FLAG = 0;

typedef struct ReducedPerlinNoise ReducedPerlinNoise;
struct ReducedPerlinNoise {
	double a, b, c;
	uint8_t d[512];
};

typedef struct ThreadData ThreadData;
struct ThreadData {
	uint64_t start, end;
};

typedef struct Tuple Tuple;
struct Tuple {
	int first, second;
};

/*
	Each octave in a climate's octave set is given a particular amplitude to weight their eventual sum (and thus returned value).
	These have been all been scaled to A. be integer values and B. sum to the same value (SUM) while retaining their respective proportions,
	allowing different climates to be compared together.
	For comparison, the original climate proportions are listed below:
		TEMPERATURE:		{6, 6, 1, 1}
		HUMIDITY:			{2, 2, 1, 1}
		CONTINENTALNESS:	{256, 256, 128, 128, 128, 128, 64, 64, 32, 32, 8, 8, 4, 4, 2, 2, 1, 1}
		EROSION:			{16, 16, 8, 8, 2, 2, 1, 1}
		WEIRDNESS:			{4, 4, 4, 4, 1, 1}

*/
const uint64_t TEMPERATURE_WEIGHTS[] = {14418, 14418, 2403, 2403};
const uint64_t HUMIDITY_WEIGHTS[] = {11214, 11214, 5607, 5607};
const uint64_t CONTINENTALNESS_WEIGHTS[] = {6912, 6912, 3456, 3456, 3456, 3456, 1728, 1728, 864, 864, 216, 216, 108, 108, 54, 54, 27, 27};
const uint64_t EROSION_WEIGHTS[] = {9968, 9968, 4984, 4984, 1246, 1246, 623, 623};
const uint64_t WEIRDNESS_WEIGHTS[] = {7476, 7476, 7476, 7476, 1869, 1869};
const double SUM = 33642;

//Initialized in global scope so all threads can access it
double minOffsetsSum;

void *checkSeed(void *dat) {
	ThreadData *data = dat;
	PerlinNoise oct[46];
	double b, offsetsSum;
	uint64_t xloo, xhio, xlo, xhi, xlo2, xhi2;
	Xoroshiro pxr;
	// for (uint64_t seed = data->start; seed < data->end; ++seed) {
	/*When checking extremely large numbers of seeds (where there is little chance the program will actually ever finish),
	  you can *very slightly* speed things up by removing the end condition search for every seed.
	  DON'T FORGET TO READD if switching back to a lower seedcount, though.*/
	for (uint64_t seed = data->start; ; ++seed) {
		if (!(seed % 8000000000)) printf("<%" PRId64">\n", seed);
		xSetSeed(&pxr, seed);
		xloo = xNextLong(&pxr);
		xhio = xNextLong(&pxr);
		offsetsSum = 0;
		
		if (CLIMATES_TO_CHECK & TEMPERATURE) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x944b0073edf549db : 0x5c7e6b29735f0d7f);
			pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0x4ff44347e9d22b96 : 0xf7d86f1bbc734988); // md5 "minecraft:temperature_large" or "minecraft:temperature"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);

			// Check Octave 0 b-values
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 14418 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			// Check Octave 2 b-values
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 14418 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			if (OCTAVES_TO_MAX > 2) {
				// Check Octave 1 b-values
				pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
				pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
				xNextDouble(&pxr);
				b = xNextDouble(&pxr) * 256.0;
				offsetsSum += 2403 * fabs(b - (int)(b) - .5);
				if (offsetsSum > minOffsetsSum) continue;

				// Check Octave 3 b-values
				pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
				pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
				xNextDouble(&pxr);
				b = xNextDouble(&pxr) * 256.0;
				offsetsSum += 2403 * fabs(b - (int)(b) - .5);
				if (offsetsSum > minOffsetsSum) continue;
			}

			// 
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
			oct[0].a = (int)(xNextDouble(&pxr) * 256.);
			oct[0].b = (int)(xNextDouble(&pxr) * 256.);
			oct[0].c = (int)(xNextDouble(&pxr) * 256.);
			for (int i = 0; i < 256; ++i) oct[0].d[i] = i;
			for (int i = 0; i < 255; ++i) {
				int j = xNextInt(&pxr, 256 - i) + i;
				uint8_t n = oct[0].d[i];
				oct[0].d[i] = oct[0].d[j];
				oct[0].d[j] = n;
				oct[0].d[i + 256] = oct[0].d[i];
			}
			oct[0].d[511] = oct[0].d[255];
			for (int i = 0; i < 255; ++i) {
				int a2 = oct[0].d[i] % 16;
				if (a2 == 0 || a2 == 4 || a2 == 8 || a2 == 12) {
					int a2PlusOne = oct[0].d[i + 1] % 16;
					if (a2PlusOne == 0 || a2PlusOne == 6 || a2PlusOne == 10 || a2PlusOne == 12) {
						
					}
				}

			}

		}

		if (CLIMATES_TO_CHECK & HUMIDITY) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x71b8ab943dbd5301 : 0x81bb4d22e8dc168e);
			pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xbb63ddcf39ff7a2b : 0xf1c8b4bea16303cd); // md5 "minecraft:vegetation_large" or "minecraft:vegetation"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);

			//Octave 4
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 11214 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 6
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 11214 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 5
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 5607 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 7
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 5607 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;
		}

		if (CLIMATES_TO_CHECK & CONTINENTALNESS) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x9a3f51a113fce8dc : 0x83886c9d0ae3a662);
			pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xee2dbd157e5dcdad : 0xafa638a61b42e8ad); // md5 "minecraft:continentalness_large" or "minecraft:continentalness"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);

			//Octave 8
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 6912 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 17
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 6912 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 9
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 18
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 10
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 19
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 11
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1728 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 20
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1728 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 12
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 864 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 21
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 864 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 13
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 216 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 22
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 216 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 14
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 108 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 23
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 108 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 15
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 54 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 24
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 54 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 16
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 27 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 25
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 27 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;
		}

		if (CLIMATES_TO_CHECK & EROSION) {
			pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x8c984b1f8702a951 : 0xd02491e6058f6fd8);
			pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xead7b1f92bae535f : 0x4792512c94c17a80); // md5 "minecraft:erosion_large" or "minecraft:erosion"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);

			//Octave 26
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 9968 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 30
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 9968 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 27
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 4984 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 31
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 4984 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 28
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1246 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 32
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1246 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 29
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 623 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 33
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 623 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;
		}

		if (CLIMATES_TO_CHECK & WEIRDNESS) {
			pxr.lo = xloo ^ 0xefc8ef4d36102b34;
			pxr.hi = xhio ^ 0x1beeeb324a0f24ea; // md5 "minecraft:ridge"
			xlo = xNextLong(&pxr);
			xhi = xNextLong(&pxr);
			xlo2 = xNextLong(&pxr);
			xhi2 = xNextLong(&pxr);

			//Octave 40
			pxr.lo = xlo ^ 0xf11268128982754f;
			pxr.hi = xhi ^ 0x257a1d670430b0aa; // md5 "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 43
			pxr.lo = xlo2 ^ 0xf11268128982754f;
			pxr.hi = xhi2 ^ 0x257a1d670430b0aa; // md5 "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 41
			pxr.lo = xlo ^ 0xe51c98ce7d1de664;
			pxr.hi = xhi ^ 0x5f9478a733040c45; // md5 "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 44
			pxr.lo = xlo2 ^ 0xe51c98ce7d1de664;
			pxr.hi = xhi2 ^ 0x5f9478a733040c45; // md5 "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 42
			pxr.lo = xlo ^ 0x6d7b49e7e429850a;
			pxr.hi = xhi ^ 0x2e3063c622a24777; // md5 "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1869 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;

			//Octave 45
			pxr.lo = xlo2 ^ 0x6d7b49e7e429850a;
			pxr.hi = xhi2 ^ 0x2e3063c622a24777; // md5 "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1869 * fabs(b - (int)(b) - .5);
			if (offsetsSum > minOffsetsSum) continue;
		}

		//Prints seed
		printf("%" PRId64 " (%f)\n", seed, offsetsSum/(SUM/2));
		if (offsetsSum < minOffsetsSum) minOffsetsSum = offsetsSum;
	}
	return NULL;
}

int main() {
	pthread_t threads[NUMBER_OF_THREADS];
	ThreadData data[NUMBER_OF_THREADS];
	uint64_t seedsToCheck, startSeed, tasksPerThread;
	//Continually prompts user for a seed to begin searching from
	while (1) {
		printf("Enter the number of seeds to check for.\n");
		printf("0 will be treated as all seeds, -N will be treated as all but N seeds. ");
		if (scanf("%" PRId64, &seedsToCheck) <= 0) seedsToCheck = 0;
		tasksPerThread = (seedsToCheck - 1) / NUMBER_OF_THREADS + 1;

		printf("Enter an initial minimum offset percentage in the range [0, 1]. ");
		if (scanf("%lf", &minOffsetsSum) <= 0) minOffsetsSum = 1;
		if (minOffsetsSum < 0) minOffsetsSum = 0;
		if (minOffsetsSum > 1) minOffsetsSum = 1;
		minOffsetsSum *= .5 * SUM * ((CLIMATES_TO_CHECK & TEMPERATURE) + (CLIMATES_TO_CHECK & HUMIDITY)
						+ (CLIMATES_TO_CHECK & CONTINENTALNESS) + (CLIMATES_TO_CHECK & EROSION) + (CLIMATES_TO_CHECK & WEIRDNESS));

		printf("Enter a seed to begin searching from. ");
		scanf("%" PRId64, &startSeed);
		clock_t startTime = clock();
		//Iterates over the specified number of seeds, beginning at the one originally specified by the user
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			data[i].start = i * tasksPerThread + startSeed;
			data[i].end = (i + 1) * tasksPerThread + startSeed;
			pthread_create(&threads[i], NULL, checkSeed, &data[i]);
		}
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			pthread_join(threads[i], NULL);
		}
		printf("(%lf seconds)\n", (double)(clock() - startTime) / CLOCKS_PER_SEC);
	}
	return 0;
}