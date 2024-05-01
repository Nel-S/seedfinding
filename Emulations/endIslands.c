#include "utilities/cubiomes/finders.h"

STRUCT(EndIsland) {
	Pos3 topPos;
	int radius, depth;
};

/* Returns the number of End islands in a particular chunk.
   If not `NULL`, the End islands' data are stored in `island1` and `island2`.*/
int getSmallEndIslands(uint64_t lower48, int chunkX, int chunkZ, EndIsland *island1, EndIsland *island2) {
	// Initializes RNG
	uint64_t rng;
	setSeed(&rng, lower48);
	setSeed(&rng, 16*(chunkX*(nextLong(&rng) | 1) + chunkZ*(nextLong(&rng) | 1)) ^ lower48);

	/* If island1/island2 are NULL, the island's data is saved to a temporary EndIsland that's ultimately discarded, which the compiler should optimize out.
	   (We still need to emulate the RNG calls if e.g. island1 is NULL but island2 isn't.)*/
	EndIsland *saveData1, *saveData2;
	if (island1) saveData1 = island1;
	if (island2) saveData2 = island2;
	int count = 0;

	// Determines first island position, if applicable
	if (!nextInt(&rng, 14)) {
		++count;
		saveData1->topPos.x = 16*chunkX + nextInt(&rng, 16);
		saveData1->topPos.y = 55 + nextInt(&rng, 16);
		saveData1->topPos.z = 16*chunkZ + nextInt(&rng, 16);
		
		// Determines second island position, if applicable
		if (!nextInt(&rng, 4)) {
			++count;
			saveData2->topPos.x = 16*chunkX + nextInt(&rng, 16);
			saveData2->topPos.y = 55 + nextInt(&rng, 16);
			saveData2->topPos.z = 16*chunkZ + nextInt(&rng, 16);
		}

		// Determines first island dimensions, if applicable
		saveData1->radius = nextInt(&rng, 3) + 4;
		saveData1->depth = 0;
		for (double i = saveData1->radius; i > 0.5; i -= nextInt(&rng, 2) + 0.5) ++saveData1->depth;

		// Determines second island dimensions, if applicable
		if (count > 1) {
			saveData2->radius = nextInt(&rng, 3) + 4;
			saveData2->depth = 0;
			for (double i = saveData2->radius; i > 0.5; i -= nextInt(&rng, 2) + 0.5) ++saveData2->depth;
		}
	}
	return count;
}