#include "../../../Utilities/Spawn.h"
#include "../../../core/common_seedfinding.h"

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = 0;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH = "C:\\msys64\\home\\seedfinding\\Far Spawns\\1.18+\\24w40a First-Stage Continentalness below -0.110125.txt";
const char *OUTPUT_FILEPATH = NULL;
const bool TIME_PROGRAM = false;
const bool LARGE_BIOMES_FLAG = false;
const bool SNAPSHOT = true;

const double JAVA_RADIAL_THRESHOLD = 2560.19999219;
const int JAVA_AXIAL_THRESHOLD = 2560;
const int JAVA_L_INFINITY_THRESHOLD = 1696;

DEFAULT_LOCALS_INITIALIZATION

void initGlobals() {}

void fitnessAlg(const PerlinNoise *oct, const double r, const double k, Pos *p, double *fitness) {
// void fitnessAlg(const PerlinNoise *oct, const double r, const double k, Pos *p, double *fitness, const Generator *g) {
	Pos initPos = *p;
	for (double rad = 0; rad <= r; rad += k) {
		for (double ang = 0.; ang <= U_TWO_PI; ang += (rad ? k/rad : INFINITY)) {
			int x = initPos.x + (int)(sin(ang) * rad);
			int z = initPos.z + (int)(cos(ang) * rad);
			Pos pos = {x, z};
			double fit = U_sampleAndGetFitness(&pos, oct, SNAPSHOT, LARGE_BIOMES_FLAG);
			// int64_t np[6];
			// sampleBiomeNoise(&g->bn, np, x, 0, z, NULL, LARGE_BIOMES_FLAG | SAMPLE_NO_BIOME | SAMPLE_NO_DEPTH);
			// printf("\t(%d, %d): %.4f\n", x, z, np[NP_CONTINENTALNESS]/10000.);
			// double px = x, pz = z;
			// U_sampleClimate(NP_SHIFT, oct, &px, &pz, LARGE_BIOMES_FLAG);
			// double px = floor(x/4.), pz = floor(z/4.);
			// printf("\t(%d, %d): %.4f\n", x, z, U_sampleClimate(NP_CONTINENTALNESS, oct, &px, &pz, LARGE_BIOMES_FLAG));
			if (fit < *fitness) {
				p->x = x;
				p->z = z;
				*fitness = fit;
				// printf("\t(%d, %d): %.17g\n", x, z, fit);
			}
		}
	}
}

void *runWorker(void *workerIndex) {
	Generator g;
	setupGenerator(&g, MC_NEWEST, LARGE_BIOMES_FLAG);

	uint64_t seed = 0;
	if (!getNextSeed(workerIndex, &seed)) return NULL;
	do {
		applySeed(&g, DIM_OVERWORLD, seed);
		// g->bn.nptype = NP_CONTINENTALNESS;
		Pos spawn = {0, 0};
		double fitness = INFINITY;
		fitnessAlg(g.bn.oct, 2048, 512, &spawn, &fitness);
		// fitnessAlg(g.bn.oct, 2048, 512, &spawn, &fitness, &g);
		// printf("---\n");
		fitnessAlg(g.bn.oct, 512, 32, &spawn, &fitness);
		// fitnessAlg(g.bn.oct, 512, 32, &spawn, &fitness, &g);
		// Pos offsetSpawn = {(spawn.x & -16) + (spawn.x > 0 ? 95 : -80), (spawn.z & -16) + (spawn.z > 0 ? 95 : -80)};
		Pos offsetSpawn = {(spawn.x & -16) + 8, (spawn.z & -16) + 8};
		int flags = 0;
		flags = (((offsetSpawn.x + (offsetSpawn.x > 0 ? 87 : -88)) * (offsetSpawn.x + (offsetSpawn.x > 0 ? 87 : -88)) + (offsetSpawn.z + (offsetSpawn.z > 0 ? 87 : -88)) * (offsetSpawn.z + (offsetSpawn.z > 0 ? 87 : -88)) >= JAVA_RADIAL_THRESHOLD*JAVA_RADIAL_THRESHOLD) << 2) | ((max(abs(offsetSpawn.x), abs(offsetSpawn.z)) >= JAVA_AXIAL_THRESHOLD - 88) << 1) | (min(abs(offsetSpawn.x), abs(offsetSpawn.z)) >= JAVA_L_INFINITY_THRESHOLD - 88);
		if (!flags) continue;
		outputValues("%" PRIu64 "\t%d\t%d\t%f\t%d\n", seed, spawn.x, spawn.z, sqrt(spawn.x * spawn.x + spawn.z * spawn.z), flags);
		// double efc = -sqrt(fitness - (spawn.x*spawn.x + spawn.z*spawn.z))/20480000. - 0.11;
		// if (efc > -0.19) continue;
		// outputValues("%" PRIu64 "\t%f\n", seed, efc);
		// Pos estimate = estimateSpawn(&g, NULL);
		// printf("Estimate: (%d, %d) vs (%d, %d)\n", estimate.x, estimate.z, spawn.x, spawn.z);
		// estimate = getSpawn(&g);
		// printf("Estimate 2: (%d, %d)\n", estimate.x, estimate.z);
	} while (getNextSeed(NULL, &seed));

	return 0;
}