#include "../core/common_seedfinding.h"
#include "../Utilities/Climates.h"
// #include "../utilities/cubiomes/finders.c"
// #include "../utilities/cubiomes/biomenoise.c"

// const uint64_t GLOBAL_START_SEED = -230734003789;
const uint64_t GLOBAL_START_SEED = 0;
// const uint64_t GLOBAL_SEEDS_TO_CHECK = 100000000;
const uint64_t GLOBAL_SEEDS_TO_CHECK = CHECK_THIS_SEED_AND_FOLLOWING(GLOBAL_START_SEED);
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const int PERCENTILE = FIFTIETH_PERCENTILE;
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;

const bool LARGE_BIOMES_FLAG = false;
const bool TIME_PROGRAM = false;
const bool DELAY_SHIFT = true;

DEFAULT_LOCALS_INITIALIZATION

void *runWorker(void *workerIndex) {
    Generator g;
    setupGenerator(&g, MC_NEWEST, LARGE_BIOMES_FLAG);
    U_manualBNinit(&g.bn, LARGE_BIOMES_FLAG);
    double Cdouble[U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]];
    U_initClimateBoundsArray(NP_CONTINENTALNESS, -0.87, PERCENTILE, Cdouble, U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]);
    PerlinNoise *oct = g.bn.oct;
    Xoroshiro pxr, pxr2;

    uint64_t seed;
    if (!getNextSeed(workerIndex, &seed)) return NULL;
    do {
		double px = 480, pz = 480;
		if (!DELAY_SHIFT) {
			U_initClimate(NP_SHIFT, oct, seed, LARGE_BIOMES_FLAG);
            U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        } else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        xSetSeed(&pxr, seed);
		uint64_t xloo = xNextLong(&pxr);
		uint64_t xhio = xNextLong(&pxr);
        pxr.lo = xloo ^ (LARGE_BIOMES_FLAG ? 0x9a3f51a113fce8dc : 0x83886c9d0ae3a662);
		pxr.hi = xhio ^ (LARGE_BIOMES_FLAG ? 0xee2dbd157e5dcdad : 0xafa638a61b42e8ad); // md5 "minecraft:continentalness_large" or "minecraft:continentalness"
		uint64_t xlo = xNextLong(&pxr);
		uint64_t xhi = xNextLong(&pxr);
		pxr2.lo = xlo ^ (LARGE_BIOMES_FLAG ? 0x0fd787bfbc403ec3 : 0x082fe255f8be6631);
		pxr2.hi = xhi ^ (LARGE_BIOMES_FLAG ? 0x74a4a31ca21b48b8 : 0x4e96119e22dedc81); // md5 "octave_-11" or "octave_-9"
		U_initPerlin(&oct[8], &pxr2);

        double npC = 256./511 * samplePerlin(&oct[8], px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, 0);
        if (npC > -0.35) continue;

        px = -416, pz = -416;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        npC = 256./511 * samplePerlin(&oct[8], px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, 0);
        if (npC > -0.25) continue;

        px = -416, pz = 480;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        npC = 256./511 * samplePerlin(&oct[8], px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, px/(LARGE_BIOMES_FLAG ? 2048. : 512.), 0, 0);
        if (npC > -0.25) continue;

        px = 544, pz = 480;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        if (U_initAndSampleClimateBounded(NP_CONTINENTALNESS, oct, &px, &pz, NULL, Cdouble, &seed, LARGE_BIOMES_FLAG, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

        px = -416, pz = -480;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        if (U_sampleClimateBounded(NP_CONTINENTALNESS, oct, px, pz, NULL, Cdouble, LARGE_BIOMES_FLAG, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

        px = -416, pz = 480;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        if (U_sampleClimateBounded(NP_CONTINENTALNESS, oct, px, pz, NULL, Cdouble, LARGE_BIOMES_FLAG, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

        px = 544, pz = -480;
        if (!DELAY_SHIFT) U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
        else {
            px = floor(px/4.);
            pz = floor(pz/4.);
        }
        if (U_sampleClimateBounded(NP_CONTINENTALNESS, oct, px, pz, NULL, Cdouble, LARGE_BIOMES_FLAG, &npC) < U_CLIMATE_NUMBER_OF_OCTAVES[NP_CONTINENTALNESS]) continue;

        if (DELAY_SHIFT) U_initClimate(NP_SHIFT, oct, seed, LARGE_BIOMES_FLAG);
        U_initClimate(NP_TEMPERATURE, oct, seed, LARGE_BIOMES_FLAG);
        U_initClimate(NP_HUMIDITY, oct, seed, LARGE_BIOMES_FLAG);
        U_initClimate(NP_EROSION, oct, seed, LARGE_BIOMES_FLAG);
        U_initClimate(NP_WEIRDNESS, oct, seed, LARGE_BIOMES_FLAG);

        // TODO: Remove
        setBiomeSeed(&g.bn, seed, LARGE_BIOMES_FLAG);
        Pos spawn = getSpawn(&g);
        // outputValues("%d %d\n", spawn.x, spawn.z);
        if (abs(spawn.x - 64) > 350 || abs(spawn.z) > 350) continue;

        outputValues("%" PRId64 "\n", seed);
    } while (getNextSeed(NULL, &seed));
    return NULL;
}