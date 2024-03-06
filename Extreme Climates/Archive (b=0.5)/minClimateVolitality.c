#include "rng.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

enum {TEMPERATURE = 0b10000, HUMIDITY = 0b1000, CONTINENTALNESS = 0b100, EROSION = 0b10, WEIRDNESS = 0b1};

const uint64_t SEEDS_TO_CHECK = -4;
const int CLIMATES_TO_CHECK = CONTINENTALNESS;
//Set to 1 for Large Biomes generation, or 0 for normal generation.
const int LARGE_BIOMES_FLAG = 0;

const int NUMBER_OF_THREADS = 4;
const uint64_t TASKS_PER_THREAD = (SEEDS_TO_CHECK + NUMBER_OF_THREADS - 1) / NUMBER_OF_THREADS;

typedef struct ThreadData ThreadData;
struct ThreadData {
	uint64_t start, end;
};
// const int TEMPERATURE_WEIGHTS[] = {6, 6, 1, 1};
// const int HUMIDITY_WEIGHTS[] = {2, 2, 1, 1};
// const int CONTINENTALNESS_WEIGHTS[] = {256, 256, 128, 128, 128, 128, 64, 64, 32, 32, 8, 8, 4, 4, 2, 2, 1, 1};
// const int EROSION_WEIGHTS[] = {16, 16, 8, 8, 2, 2, 1, 1};
// const int WEIRDNESS_WEIGHTS[] = {4, 4, 4, 4, 1, 1};
const uint64_t TEMPERATURE_WEIGHTS[] = {14418, 14418, 2403, 2403};
const uint64_t HUMIDITY_WEIGHTS[] = {11214, 11214, 5607, 5607};
const uint64_t CONTINENTALNESS_WEIGHTS[] = {6912, 6912, 3456, 3456, 3456, 3456, 1728, 1728, 864, 864, 216, 216, 108, 108, 54, 54, 27, 27};
const uint64_t EROSION_WEIGHTS[] = {9968, 9968, 4984, 4984, 1246, 1246, 623, 623};
const uint64_t WEIRDNESS_WEIGHTS[] = {7476, 7476, 7476, 7476, 1869, 1869};
const double MAX_CLIMATES = 33642.;

double minOffsetsSum;

void *checkSeed(void *dat) {
	ThreadData *data = dat;
	double b, offsetsSum;
	uint64_t xloo, xhio, xlo, xhi, xlo2, xhi2;
	Xoroshiro pxr, pxr2;
	// for (uint64_t seed = data->start; seed>data->end; ++seed) {
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

			//Octave 0
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 14418 * fabs(b - (int)(b) - .5);

			//Octave 2
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xb198de63a8012672 : 0x36d326eed40efeb2);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x7b84cad43ef7b5a8 : 0x5be9ce18223c636a); // md5 "octave_-12" or "octave_-10"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 14418 * fabs(b - (int)(b) - .5);

			//Octave 1
			pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 2403 * fabs(b - (int)(b) - .5);

			//Octave 3
			pxr2.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr2.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 2403 * fabs(b - (int)(b) - .5);
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
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 11214 * fabs(b - (int)(b) - .5);
			
			//Octave 6
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 11214 * fabs(b - (int)(b) - .5);
			
			//Octave 5
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 5607 * fabs(b - (int)(b) - .5);
			
			//Octave 7
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr2);
			b = xNextDouble(&pxr2) * 256.0;
			offsetsSum += 5607 * fabs(b - (int)(b) - .5);
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

			//Octave 17
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 6912 * fabs(b - (int)(b) - .5);
			

			//Octave 9
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			

			//Octave 18
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			

			//Octave 10
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			

			//Octave 19
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x082fe255f8be6631 : 0xf11268128982754f);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x4e96119e22dedc81 : 0x257a1d670430b0aa); // md5 "octave_-9" or "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 3456 * fabs(b - (int)(b) - .5);
			

			//Octave 11
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1728 * fabs(b - (int)(b) - .5);
			

			//Octave 20
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1728 * fabs(b - (int)(b) - .5);
			

			//Octave 12
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 864 * fabs(b - (int)(b) - .5);
			

			//Octave 21
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 864 * fabs(b - (int)(b) - .5);
			

			//Octave 13
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 216 * fabs(b - (int)(b) - .5);
			

			//Octave 22
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xe51c98ce7d1de664 : 0xbd90d5377ba1b762);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5f9478a733040c45 : 0xc07317d419a7548d); // md5 "octave_-6" or "octave_-4"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 216 * fabs(b - (int)(b) - .5);
			

			//Octave 14
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 108 * fabs(b - (int)(b) - .5);
			

			//Octave 23
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x6d7b49e7e429850a : 0x53d39c6752dac858);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x2e3063c622a24777 : 0xbcd1c5a80ab65b3e); // md5 "octave_-5" or "octave_-3"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 108 * fabs(b - (int)(b) - .5);
			

			//Octave 15
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 54 * fabs(b - (int)(b) - .5);
			

			//Octave 24
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xbd90d5377ba1b762 : 0xb4a24d7a84e7677b);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xc07317d419a7548d : 0x023ff9668e89b5c4); // md5 "octave_-4" or "octave_-2"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 54 * fabs(b - (int)(b) - .5);
			

			//Octave 16
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 27 * fabs(b - (int)(b) - .5);
			

			//Octave 25
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x53d39c6752dac858 : 0xdffa22b534c5f608);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0xbcd1c5a80ab65b3e : 0xb9b67517d3665ca9); // md5 "octave_-3" or "octave_-1"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 27 * fabs(b - (int)(b) - .5);
			
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
			

			//Octave 30
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 9968 * fabs(b - (int)(b) - .5);
			

			//Octave 27
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 4984 * fabs(b - (int)(b) - .5);
			

			//Octave 31
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x36d326eed40efeb2 : 0x0ef68ec68504005e);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x5be9ce18223c636a : 0x48b6bf93a2789640); // md5 "octave_-10" or "octave_-8"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 4984 * fabs(b - (int)(b) - .5);
			

			//Octave 28
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1246 * fabs(b - (int)(b) - .5);
			

			//Octave 32
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0x0ef68ec68504005e : 0xe51c98ce7d1de664);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x48b6bf93a2789640 : 0x5f9478a733040c45); // md5 "octave_-8" or "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1246 * fabs(b - (int)(b) - .5);
			

			//Octave 29
			pxr.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 623 * fabs(b - (int)(b) - .5);
			

			//Octave 33
			pxr.lo = xlo2 ^ (LARGE_BIOMES_FLAG ? 0xf11268128982754f : 0x6d7b49e7e429850a);
			pxr.hi = xhi2 ^ (LARGE_BIOMES_FLAG ? 0x257a1d670430b0aa : 0x2e3063c622a24777); // md5 "octave_-7" or "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 623 * fabs(b - (int)(b) - .5);
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

			//Octave 43
			pxr.lo = xlo2 ^ 0xf11268128982754f;
			pxr.hi = xhi2 ^ 0x257a1d670430b0aa; // md5 "octave_-7"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);

			//Octave 41
			pxr.lo = xlo ^ 0xe51c98ce7d1de664;
			pxr.hi = xhi ^ 0x5f9478a733040c45; // md5 "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);

			//Octave 44
			pxr.lo = xlo2 ^ 0xe51c98ce7d1de664;
			pxr.hi = xhi2 ^ 0x5f9478a733040c45; // md5 "octave_-6"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 7476 * fabs(b - (int)(b) - .5);

			//Octave 42
			pxr.lo = xlo ^ 0x6d7b49e7e429850a;
			pxr.hi = xhi ^ 0x2e3063c622a24777; // md5 "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1869 * fabs(b - (int)(b) - .5);

			//Octave 45
			pxr.lo = xlo2 ^ 0x6d7b49e7e429850a;
			pxr.hi = xhi2 ^ 0x2e3063c622a24777; // md5 "octave_-5"
			xNextDouble(&pxr);
			b = xNextDouble(&pxr) * 256.0;
			offsetsSum += 1869 * fabs(b - (int)(b) - .5);
		}

		if (offsetsSum < minOffsetsSum) continue;
		//Prints seed
		printf("%" PRId64 " (%f)\n", seed, offsetsSum);
		if (offsetsSum > minOffsetsSum) minOffsetsSum = offsetsSum;
	}
	return NULL;
}

int main() {
	pthread_t threads[NUMBER_OF_THREADS];
	ThreadData data[NUMBER_OF_THREADS];
	//Continually prompts user for a seed to begin searching from
	uint64_t startSeed;
	while (1) {
		// printf("Enter an initial maximum offset sum ([0, %lf]). ", MAX_CLIMATES * 5);
		// scanf("%lf", &minOffsetsSum);
		// if (minOffsetsSum < 0) minOffsetsSum = 0;
		// if (minOffsetsSum > MAX_CLIMATES * 5) minOffsetsSum = MAX_CLIMATES * 5;
		minOffsetsSum = 0.;
		printf("Enter a seed to begin searching from. ");
		scanf("%" PRId64, &startSeed);
		clock_t startTime = clock();
		//Iterates over N seeds, beginning at the one originally specified by the user
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			data[i].start = i * TASKS_PER_THREAD + startSeed;
			data[i].end = (i + 1) * TASKS_PER_THREAD + startSeed;
			pthread_create(&threads[i], NULL, checkSeed, &data[i]);
		}
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			pthread_join(threads[i], NULL);
		}
		printf("(%f seconds)\n", (double)(clock() - startTime) / CLOCKS_PER_SEC);
	}
	return 0;
}