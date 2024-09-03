#include "../../common.h"
#include "../../utilities/cubiomes/finders.h"
#include "../../utilities/math.h"
#include <pthread.h>

enum Mode {Default, LargeBiomes, SuperflatAllGrass /* Not implemented accurately yet, at least for 1.6.4 */, SuperflatNoGrass};

typedef struct SeedCoordBiome {
	uint64_t seed;
	Pos coord;
	int biome;
} SeedCoordBiome;

const uint64_t GLOBAL_START_SEED = 0, GLOBAL_SEEDS_TO_CHECK = 0;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const int MC_VERSION = MC_B1_8;
const int GENERATION_MODE = Default;
// const char *INPUT_FILEPATH  = "C:\\msys64\\home\\seedfinding\\Far Spawns\\Beta 1.8-1.12\\Ideal Internal States\\Internal States (36178138).txt";
// const char *INPUT_FILEPATH  = "C:\\msys64\\home\\seedfinding\\Far Spawns\\Beta 1.8-1.12\\Ideal Internal States\\36178138 L-infinity.txt";
const char *INPUT_FILEPATH  = "C:\\msys64\\home\\seedfinding\\Far Spawns\\Beta 1.8-1.12\\Ideal Internal States\\36178138 L-negative infinity.txt";
const char *OUTPUT_FILEPATH = NULL;
// const char *OUTPUT_FILEPATH = "C:\\msys64\\home\\seedfinding\\Far Spawns\\Beta 1.8-1.12\\1.0 Seedlist.txt";
const bool TIME_PROGRAM = false;
const uint64_t INITIAL_SQUARED_RADIAL_THRESHOLD = 0;
const int INITIAL_AXIAL_THRESHOLD = 0;
const int INITIAL_MIN_COORD_THRESHOLD = 0;
const bool UPDATE_THRESHOLDS = true;
// const uint64_t INITIAL_SQUARED_RADIAL_THRESHOLD = 2243 * 2243 + 1779 * 1779 + 1;
// const int INITIAL_AXIAL_THRESHOLD = 2244;
// const int INITIAL_MIN_COORD_THRESHOLD = 1780;
// const bool UPDATE_THRESHOLDS = false;
const int SECOND_CHANCES = 0;
const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {{61654682630843, {166, -522}, ocean}, {61674689339201, {-394, 505}, ocean}, {61976165701569, {-2622, 3262}, ocean}, {69254814927237, {-247, 180}, ocean}, {94590021110080, {499, -548}, ocean}, {121006349069846, {-430, 293}, ocean}, {134901792375665, {-332, 159}, ocean}, {236723776710479, {194, -177}, ocean}, {264342219492421, {-49, 63}, ocean}}; // Beta 1.8-1.8.1
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {{104290869483320, {646, -3556}, ocean}, {212600822649594, {-3188, 394}, ocean}}; // 1.0
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {{236723776710479, {3732, -4025}, ocean}, {236723776710479, {3554, -3438}, ocean}, {264342219492421, {-1432, 1683}, ocean}}; // 1.1-1.6.4
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {}; // 1.3.1-1.6.4 Large Biomes
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {{264342219492421, {-1343, 1488}, beach}}; // 1.7.2-1.8.9
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {}; // 1.7.2-1.8.9 Large Biomes
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {}; // 1.9-1.12.2
// const SeedCoordBiome KNOWN_GRASS_FALSE_NEGATIVES[] = {}; // 1.9-1.12.2 Large Biomes

uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;

// The preferred first-stage biomes for Beta 1.8-1.0.
const uint64_t g_spawn_biomes_1  = (1ULL << forest) | (1ULL << swamp) | (1ULL << taiga);
// The preferred first-stage biomes for 1.1-1.17.
const uint64_t g_spawn_biomes_17 = (1ULL << forest) | (1ULL << plains) | (1ULL << taiga) | (1ULL << taiga_hills) | (1ULL << wooded_hills) | (1ULL << jungle) |(1ULL << jungle_hills);
// The most possible state advancements in nextInt(2) nextInt(3) ... nextInt(16641). Found by Andrew,
const int MAX_POSSIBLE_STATE_ADVANCEMENTS = 16647;

uint64_t farthestSquaredRadialDistance;
int farthestAxialDistance, farthestMinCoordinateDistance;
pthread_mutex_t mutex;

void initGlobals() {
	farthestSquaredRadialDistance = INITIAL_SQUARED_RADIAL_THRESHOLD;
	farthestAxialDistance = INITIAL_AXIAL_THRESHOLD;
	farthestMinCoordinateDistance = INITIAL_MIN_COORD_THRESHOLD;
}

// Steps the given java.util.Random internal state backwards once.
static inline void stepBackState(uint64_t *state) {
	*state = (*state * 246154705703781 + 107048004364969) & 0xffffffffffff;
}

// From Cubiomes
static bool id_matches(int id, uint64_t b, uint64_t m) {
    return id < 128 ? b & (1ULL << id) : m & (1ULL << (id - 128));
}

// Adapted from Cubiomes' mapApproxHeight
bool isLikelyGrass(const Generator *g, const SurfaceNoise *sn, int x, int z, bool checkForFalseNegatives) {
	// For some reason superflat ignores the grass checks. Maybe because they're at such a low y-level?
	if (GENERATION_MODE == SuperflatAllGrass || GENERATION_MODE == SuperflatNoGrass) return false;

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
	if (KNOWN_GRASS_FALSE_NEGATIVES && checkForFalseNegatives) {
		for (size_t i = 0; i < sizeof(KNOWN_GRASS_FALSE_NEGATIVES)/sizeof(*KNOWN_GRASS_FALSE_NEGATIVES); ++i) {
			if ((g->seed & 0xffffffffffff) == KNOWN_GRASS_FALSE_NEGATIVES[i].seed && x == KNOWN_GRASS_FALSE_NEGATIVES[i].coord.x >> 2 && z == KNOWN_GRASS_FALSE_NEGATIVES[i].coord.z >> 2 && cache[14] == KNOWN_GRASS_FALSE_NEGATIVES[i].biome) return true;
		}
	}
	if (cache[14] == beach || cache[14] == snowy_beach || cache[14] == stony_shore) grass = 0;
	if (!grass) return false;

	double wt = 0, ws = 0, wd = 0;
	for (int jj = 0; jj < 5; jj++) {
		for (int ii = 0; ii < 5; ii++) {
			double d, s;
			int id = cache[jj*6 + ii];
			getBiomeDepthAndScale(id, &d, &s, NULL);
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
	SurfaceNoise sn;
	if (GENERATION_MODE != SuperflatAllGrass && GENERATION_MODE != SuperflatNoGrass) {
		setupGenerator(&g, MC_VERSION, GENERATION_MODE == LargeBiomes);
		g.dim = DIM_OVERWORLD;
	}
	int ids[91007]; // Size returned by getMinCacheSize(&g, 4, 129, 1, 129);

	// Chooses a base state to test.
	uint64_t origInitialState;
	if (!getNextSeed(workerIndex, &origInitialState)) return NULL;
	do {
		uint64_t initialState = origInitialState;
		// This tracks how many state advancements there should be between the XORed worldseed and the chosen base state.
		// It can range from 0 (if only one biome point is valid) to MAX_POSSIBLE_STATE_ADVANCEMENTS.
		for (int desiredNumberOfAdvancements = 0; desiredNumberOfAdvancements <= MAX_POSSIBLE_STATE_ADVANCEMENTS; ++desiredNumberOfAdvancements) {
			bool checkForFalseNegatives = false;
			if (KNOWN_GRASS_FALSE_NEGATIVES) {
				for (size_t i = 0; i < sizeof(KNOWN_GRASS_FALSE_NEGATIVES)/sizeof(*KNOWN_GRASS_FALSE_NEGATIVES); ++i) {
					if ((initialState ^ 0x5deece66d) == KNOWN_GRASS_FALSE_NEGATIVES[i].seed) {
						checkForFalseNegatives = true;
						break;
					}
				}
			}
			// These are the top 16 bits for the current seed.
			for (uint64_t topBits = 0; topBits < 1 + ((1ULL << 16) - 1)*(GENERATION_MODE != SuperflatAllGrass && GENERATION_MODE != SuperflatNoGrass); ++topBits) {
				// Create and apply full seed
				uint64_t seed = (topBits << 48) | (initialState ^ 0x5deece66d);
				if (GENERATION_MODE != SuperflatAllGrass && GENERATION_MODE != SuperflatNoGrass) {
					applySeed(&g, DIM_OVERWORLD, seed);
					g.seed = seed;
					g.sha = 0;
					setLayerSeed(g.entry ? g.entry : g.ls.entry_1, seed);
					// Generates the 1:4 biomes from (-64, -64) to (64, 64) ((-256, -256) to (256, 256) on a block scale)
					// If that for whatever reason fails, continue on.
					if (genArea(g.ls.entry_4, ids, -64, -64, 129, 129)) goto nextTopSeed;
				}
				// We then emulate the biome search algorithm, tracking the number of state advancements made as we go.
				uint64_t rng = initialState;
				Pos out = {0, 0};
				int found = 0, currentNumberOfAdvancements = 0;
				for (int i = 0; i < MAX_POSSIBLE_STATE_ADVANCEMENTS; ++i) {
					// The seed will be invalidated if at any point A. it has more advancements then the desired number, or B. there are not enough iterations left for the number of advancements to reach to the desired number.
					if (currentNumberOfAdvancements > desiredNumberOfAdvancements || 2*(MAX_POSSIBLE_STATE_ADVANCEMENTS - i) + currentNumberOfAdvancements < desiredNumberOfAdvancements) goto nextTopSeed;
					if (GENERATION_MODE == SuperflatNoGrass || (GENERATION_MODE != SuperflatAllGrass && !id_matches(ids[i], MC_VERSION <= MC_1_0 ? g_spawn_biomes_1 : g_spawn_biomes_17, 0))) continue;
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
				if (!found) out.x = out.z = 8*(MC_VERSION >= MC_1_9);

				// Second half of the algorithm
				int chances = 0, bestChance = 0, currentBestAxialDist = 0, currentBestMinDist = 0;
				uint64_t currentBestRadialDist = 0;
				if (GENERATION_MODE != SuperflatAllGrass && GENERATION_MODE != SuperflatNoGrass) initSurfaceNoise(&sn, DIM_OVERWORLD, g.seed);
				for (int i = 0; i < 1000 + 1; i++) {
					if (i == 1000 || isLikelyGrass(&g, &sn, out.x >> 2, out.z >> 2, checkForFalseNegatives)) {
						uint64_t currentDist = out.x * out.x + out.z * out.z;
						int bestAxis = max(abs(out.x), abs(out.z)), bestMin = min(abs(out.x), abs(out.z));
						if (currentDist > currentBestRadialDist || bestAxis > currentBestAxialDist || bestMin > currentBestMinDist) {
							bestChance = chances;
							if (currentDist > currentBestRadialDist) currentBestRadialDist = currentDist;
							if (bestAxis > currentBestAxialDist) currentBestAxialDist = bestAxis;
							if (bestMin > currentBestMinDist) currentBestMinDist = bestMin;
						}
						++chances;
						if (chances >= SECOND_CHANCES) break;
					}
					out.x += nextInt(&rng, 64) - nextInt(&rng, 64);
					out.z += nextInt(&rng, 64) - nextInt(&rng, 64);
				}
				pthread_mutex_lock(&mutex);
				int updateFlags = ((currentBestRadialDist >= farthestSquaredRadialDistance) << 2) | ((currentBestAxialDist >= farthestAxialDistance) << 1) | (currentBestMinDist >= farthestMinCoordinateDistance);
				if (!updateFlags) {
					pthread_mutex_unlock(&mutex);
					goto nextTopSeed;
				}
				outputValues("%" PRId64 "\t%d\t%f\t%d\t%d\t%d\t%" PRIu64 "\n", seed, bestChance, sqrt(currentBestRadialDist), currentBestAxialDist, currentBestMinDist, updateFlags, seed & 0xffffffffffff);
				if (UPDATE_THRESHOLDS) {
					if (currentBestRadialDist > farthestSquaredRadialDistance) farthestSquaredRadialDistance = currentBestRadialDist;
					if (currentBestAxialDist > farthestAxialDistance) farthestAxialDistance = currentBestAxialDist;
					if (currentBestMinDist > farthestMinCoordinateDistance) farthestMinCoordinateDistance = currentBestMinDist;
				}
				pthread_mutex_unlock(&mutex);
				nextTopSeed: continue;
			}
			stepBackState(&initialState);
		}
	} while (getNextSeed(NULL, &origInitialState));
	return NULL;
}