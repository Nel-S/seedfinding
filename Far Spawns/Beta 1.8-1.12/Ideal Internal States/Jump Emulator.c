#include "../../cubiomes/rng.h"
#include <math.h>
#include <stdio.h>

// const uint64_t INTERNAL_STATES[] = {0, 11, 277363943098, 11718085204285, 49720483695876, 102626409374399, 25707281917278, 25979478236433};
const uint64_t INTERNAL_STATES[] = {123788543, 3121837407};

typedef struct {
	int x, z;
} Pos;

int main() {
	for (size_t j = 0; j < sizeof(INTERNAL_STATES)/sizeof(*INTERNAL_STATES); ++j) {
		uint64_t rng = INTERNAL_STATES[j];
		Pos currentPos = {0, 0};
		// Pos xCalls[1000];
		for (int i = 0; i < 1000; ++i) {
			// xCalls[i].x = nextInt(&rng, 64);
			// xCalls[i].z = nextInt(&rng, 64);
			// currentPos.x += xCalls[i].x - xCalls[i].z;
			currentPos.x += nextInt(&rng, 64) - nextInt(&rng, 64);
			// if (j == 3) printf("\t%d.5: %d\t%d\n", i - 1, currentPos.x, currentPos.z);
			currentPos.z += nextInt(&rng, 64) - nextInt(&rng, 64);
			// if (j == 3) printf("\t%d: %d\t%d\n", i, currentPos.x, currentPos.z);
		}
		uint64_t currentDist = currentPos.x * currentPos.x + currentPos.z * currentPos.z;
		printf("%" PRIu64 "\t(%d,\t%d)\t= %" PRIu64 "\t(%f)\n", INTERNAL_STATES[j], currentPos.x, currentPos.z, currentDist, sqrt(currentDist));
		// for (size_t i = 0; i < sizeof(xCalls)/sizeof(*xCalls); ++i) printf("%d %d ", xCalls[i].x, xCalls[i].z);
		// printf("\n");
	}
	return 0;
}