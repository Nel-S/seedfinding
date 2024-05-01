#include "common.h"
#include "utilities/cubiomes/finders.h"
#include <math.h>

const uint64_t BASE_STATES[] = {1459339358529, 5159848224617, 90311412211, 8062451346052};
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const int SECOND_CHANCES = 0;
const int MC_VERSION = MC_1_8;
const char *INPUT_FILEPATH  = NULL;
const char *OUTPUT_FILEPATH = NULL;
const bool LARGE_BIOMES_FLAG = false;
const bool TIME_PROGRAM = false;

int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;


const uint64_t g_spawn_biomes_17 = (1ULL << forest) | (1ULL << plains) | (1ULL << taiga) | (1ULL << taiga_hills) | (1ULL << wooded_hills) | (1ULL << jungle) |(1ULL << jungle_hills);
const uint64_t g_spawn_biomes_1  = (1ULL << forest) | (1ULL << swamp) | (1ULL << taiga);

uint64_t farthestDist = 0;
// const uint64_t farthestDist = 14776985;

void initGlobals() {}

// Steps the given java.util.Random internal state backwards once.
static inline void stepBackState(uint64_t *state) {
	*state = (*state * 246154705703781 + 107048004364969) & 0xffffffffffff;
}

// From Cubiomes
bool mapApproxHeight2(const Generator *g, const SurfaceNoise *sn, int x, int z) {
    const float biome_kernel[25] = { // with 10 / (sqrt(i**2 + j**2) + 0.2)
        3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127,
        4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761,
        4.545454545, 8.333333333, 50.00000000, 8.333333333, 4.545454545,
        4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761,
        3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127,
    };

    Range r = {4, x-2, z-2, 6, 6, 0, 1};
    int cache[5208];
    genBiomes(g, cache, r);

	int grass;
	double d0;
	getBiomeDepthAndScale(cache[14], &d0, NULL, &grass);

	double wt = 0, ws = 0, wd = 0;
	for (int jj = 0; jj < 5; jj++) {
		for (int ii = 0; ii < 5; ii++) {
			double d, s;
			int id = cache[jj*6 + ii];
			getBiomeDepthAndScale(id, &d, &s, 0);
			float weight = biome_kernel[jj*5+ii] / (d + 2);
			if (d > d0) weight *= 0.5;
			ws += s * weight;
			wd += d * weight;
			wt += weight;
		}
	}
	ws = 960./(9.*ws/wt + 1);
	wd = (wd/wt - 1./4)*17./128;

	double off = sampleOctaveAmp(&sn->octdepth, x*200, 10, z*200, 1, 0, 1);
	off = off*(off < 0 ? -3./10 : 1)*39321./1600 - 2;
	if (off > 1) off = 1;
	off *= 17./(off < 0 ? 1792 : 2560);

	double vmin = 0, vmax = 0;
	int ytest = 8, ymin = 0, ymax = 32;
	do {
		double v[2];
		int k;
		for (k = 0; k < 2; k++) {
			int py = ytest + k;
			double n0 = sampleSurfaceNoise(sn, x, py, z);
			double fall = 17./32 - py/16.0 + off;
			fall = ws * (fall + wd);
			// fall = 960./(9.*ws/wt + 1) * (fall + (wd/wt - 1./4)*17./128);
			n0 += (fall > 0 ? 4*fall : fall);
			v[k] = n0;
			if (n0 >= 0 && py > ymin) {
				ymin = py;
				vmin = n0;
			}
			if (n0 < 0 && py < ymax) {
				ymax = py;
				vmax = n0;
			}
		}
		double dy = v[0] / (v[0] - v[1]);
		ytest += (int)(dy <= 0 ? floor(dy) : ceil(dy)); // round away from zero;
		if (ytest <= ymin) ytest = ymin+1;
		if (ytest >= ymax) ytest = ymax-1;
	} while (ymax - ymin > 1);

    return 8 * (vmin / (double)(vmin - vmax) + ymin) >= grass;
}

void *runWorker(void *workerIndex) {
	Generator g;
	// setupGenerator(&g, MC_1_12, LARGE_BIOMES_FLAG);
	setupGenerator(&g, MC_VERSION, LARGE_BIOMES_FLAG);
	g.dim = DIM_OVERWORLD;
	SurfaceNoise sn;

	int ids[91007]; // Size returned by getMinCacheSize(&g, 4, 129, 1, 129);
	// Chooses a base state to test.
	uint64_t origInitialState;
	if (!getNextSeed(workerIndex, &origInitialState)) return NULL;
	do {
		uint64_t initialState = origInitialState;
		// This tracks how many state advancements there should be between the XORed worldseed and the chosen base state.
		// It can range from 0 (if only one biome point is valid) to 16647 (most possible state advancements in nextInt(2) nextInt(3) ... nextInt(16641)).
		for (int desiredNumberOfAdvancements = 0; desiredNumberOfAdvancements <= 16647; ++desiredNumberOfAdvancements) {
			// These are the top 16 bits for the current seed.
			for (uint64_t topBits = 0; topBits < (1ULL << 16); ++topBits) {
				// Create and apply full seed
				uint64_t seed = (topBits << 48) | (initialState ^ 0x5deece66d);
				applySeed(&g, DIM_OVERWORLD, seed);
				g.seed = seed;
				g.sha = 0;
				setLayerSeed(g.entry ? g.entry : g.ls.entry_1, seed);
				// Generates the 1:4 biomes from (-64, -64) to (64, 64) ((-256, -256) to (256, 256) on a block scale)
				// If that for whatever reason fails, continue on.
				if (genArea(g.ls.entry_4, ids, -64, -64, 129, 129)) goto nextTopSeed;
				// We then emulate the biome search algorithm, tracking the number of state advancements made as we go.
				uint64_t rng = initialState; //, nextState = initialState;
				Pos out = {0, 0};
				int found = 0, currentNumberOfAdvancements = 0;
				for (int i = 0; i < 16647; ++i) {
					// The seed will be invalidated if at any point A. it has more advancements then the desired number, or B. there are not enough iterations left for the number of advancements to reach to the desired number.
					if (currentNumberOfAdvancements > desiredNumberOfAdvancements || 2*(16647 - i) + currentNumberOfAdvancements < desiredNumberOfAdvancements) goto nextTopSeed;
					if (!id_matches(ids[i], MC_VERSION <= MC_1_0 ? g_spawn_biomes_1 : g_spawn_biomes_17, 0)) continue;
					if (found) {
						// Manually test if nextInt(&rng, found + 1) == 0
						bool nextIntIsZero = false;
						if (!(found & (found + 1))) {
							++currentNumberOfAdvancements;
							nextIntIsZero = !((int64_t)((found + 1) * (uint64_t)next(&rng, 31)) >> 31);
						} else {
							int bits, val;
							do {
								++currentNumberOfAdvancements;
								bits = next(&rng, 31);
								val = bits % (found + 1);
							} while (bits - val + found < 0);
							nextIntIsZero = !val;
						}
						if (!nextIntIsZero) continue;
					}
					out.x = -256 + 4*(i%129);
					out.z = -256 + 4*(i/129);
					++found;
				}
				if (rng != origInitialState) goto nextTopSeed;
				// printf("(%" PRIu64 ")\n", seed);
				if (!found) out.x = out.z = 8;

				// Second half of the algorithm
				int chances = 0, bestChance = 0;
				uint64_t currentBestDist = 0;
				initSurfaceNoise(&sn, DIM_OVERWORLD, g.seed);
				float y;
				int id, grass = 0;
				for (int i = 0; i < 1000; i++) {
					// mapApproxHeight2(&y, &id, &g, &sn, out.x >> 2, out.z >> 2);
					// float y2;
					// int id2;
					// mapApproxHeight(&y2, &id2, &g, &sn, out.x >> 2, out.z >> 2, 1, 1);
					// if (y != y2 || id != id2) {
					// 	printf("%" PRIu64 "\t(%f, \t%d)\tvs. (%f, \t%d)\n", seed, y, id, y2, id2);
					// 	goto nextTopSeed;	
					// }
					// getBiomeDepthAndScale(id, NULL, NULL, &grass);
					// if (grass > 0 && y >= grass) {
					if (mapApproxHeight2(&g, &sn, out.x >> 2, out.z >> 2)) {
						uint64_t currentDist = out.x * out.x + out.z * out.z;
						if (currentDist > currentBestDist) {
							bestChance = chances;
							currentBestDist = currentDist;
						}
						++chances;
						if (chances >= SECOND_CHANCES) break;
					}
					out.x += nextInt(&rng, 64) - nextInt(&rng, 64);
					out.z += nextInt(&rng, 64) - nextInt(&rng, 64);
				}
				if (currentBestDist < farthestDist) goto nextTopSeed;

				outputValue("%" PRId64 "\t%d\t%f\n", seed, bestChance, sqrt(currentBestDist));
				if (currentBestDist > farthestDist) farthestDist = currentBestDist;
				nextTopSeed: continue;
			}
			stepBackState(&initialState);
		}
	} while (getNextSeed(NULL, &origInitialState));
	return NULL;
}