#include "settings.h"
#include "utilities/cubiomes/util.h"
#include "Utilities/U_Math.h"

typedef struct {
	uint64_t lower, upper;
} Bound;

void crack() {
	/* Stronghold generation occurs in two parts: An approximate location is picked, then with biomes it is narrowed to an exact location.
	   We first determine what approximate locations could have resulted in the known exact locations.*/

	/* In 1.16.5-1.18.2, stronghold generation points have offset (8,8) within their chosen chunk. In 1.19+, they have offset (0,0).
	   If the specified coordinates don't have that corresponding offset under the specified version, there's a mistake in the data, so abort.
	   TODO: Check for 1.16.4- (1.11+ still has /locate)*/
	const int POSSIBLE_CHUNK_OFFSET = VERSION <= MC_1_18_2 ? 8 : 0;
	for (size_t i = 0; i < sizeof(STRONGHOLD_GENERATION_POINTS)/sizeof(*STRONGHOLD_GENERATION_POINTS); ++i) {
		if ((STRONGHOLD_GENERATION_POINTS[i].x & 0xf) != POSSIBLE_CHUNK_OFFSET || (STRONGHOLD_GENERATION_POINTS[i].z & 0xf) != POSSIBLE_CHUNK_OFFSET) {
			printf("Error: Stronghold entry #%d (%d, %d) is not a possible stronghold generation position in version %s.\n", i + 1, STRONGHOLD_GENERATION_POINTS[i].x, STRONGHOLD_GENERATION_POINTS[i].z, mc2str(VERSION));
			return;
		}
	}

	Bound prngBounds[sizeof(STRONGHOLD_GENERATION_POINTS)/sizeof(*STRONGHOLD_GENERATION_POINTS)][2];
	for (size_t i = 0; i < sizeof(STRONGHOLD_GENERATION_POINTS)/sizeof(*STRONGHOLD_GENERATION_POINTS); ++i) {
		/* When an approximate location is picked, the resultant exact location will be within 120 - POSSIBLE_CHUNK_OFFSET Chebyshev blocks in the North/West directions,
		   or 104 + POSSIBLE_CHUNK_OFFSET Chebyshev blocks in the South/East directions. Therefore, the opposite is true for the farthest possible approximate locations
		   given an exact location.
		   Note that these are in chunk coordinates; the right-bitshift by 4 isn't disturbed by the approximation points' (8,8) offset.*/
		int approximationWestEdge  = (STRONGHOLD_GENERATION_POINTS[i].x - (104 + POSSIBLE_CHUNK_OFFSET)) >> 4;
		int approximationNorthEdge = (STRONGHOLD_GENERATION_POINTS[i].z - (104 + POSSIBLE_CHUNK_OFFSET)) >> 4;
		int approximationEastEdge  = (STRONGHOLD_GENERATION_POINTS[i].x + (120 - POSSIBLE_CHUNK_OFFSET)) >> 4;
		int approximationSouthEdge = (STRONGHOLD_GENERATION_POINTS[i].z + (120 - POSSIBLE_CHUNK_OFFSET)) >> 4;

		/* We then determine what the corresponding corners' smallest and largest Euclidean distances from the origin are.
		   (We can delay the square root until the end because if a and b are non-negative integers, 0 <= a^2 < b^2 implies 0 <= a < b.)*/
		uint_fast32_t northwestSquaredDistance = (uint_fast32_t)approximationNorthEdge*approximationNorthEdge + (uint_fast32_t)approximationWestEdge*approximationWestEdge;
		uint_fast32_t northeastSquaredDistance = (uint_fast32_t)approximationNorthEdge*approximationNorthEdge + (uint_fast32_t)approximationEastEdge*approximationEastEdge;
		uint_fast32_t southwestSquaredDistance = (uint_fast32_t)approximationSouthEdge*approximationSouthEdge + (uint_fast32_t)approximationWestEdge*approximationWestEdge;
		uint_fast32_t southeastSquaredDistance = (uint_fast32_t)approximationSouthEdge*approximationSouthEdge + (uint_fast32_t)approximationEastEdge*approximationEastEdge;
		double minDistance = sqrt(min(min(northwestSquaredDistance, northeastSquaredDistance), min(southwestSquaredDistance, southeastSquaredDistance)));
		double maxDistance = sqrt(max(max(northwestSquaredDistance, northeastSquaredDistance), max(southwestSquaredDistance, southeastSquaredDistance)));

		/* A stronghold's approximate distance is determined by the formula 80*prng.nextDouble() + 192*ring + 88.
		   Since minDistance and maxDistance can only be at most 14sqrt(2) units away, and 14sqrt(2) + max(80*prng.nextDouble()) < 192, each position can only fall within
		   exactly one stronghold ring.*/
		int ring = floor((minDistance - 88)/192.);
		// Meanwhile, only rings in the range [0,7] are valid, and if the second expression doesn't fall into the ring immediately behind it, the original distance would require
		// TODO: Finish explanation
		if (ring < 0 || 7 < ring || floor((minDistance - 167.99999999)/192.) != ring - 1) {
			ring = floor((maxDistance - 88)/192.);
			if (ring < 0 || 7 < ring || floor((maxDistance - 167.99999999)/192.) != ring - 1) {
				printf("Error: Stronghold entry #%d (%d, %d) is not within range of any stronghold rings.\n", i + 1, STRONGHOLD_GENERATION_POINTS[i].x, STRONGHOLD_GENERATION_POINTS[i].z);
				return;
			}
		}
		uint64_t distanceLowerBound = 9.007199254740992e15*max(minDistance, 192*ring + 88);
		uint64_t distanceUpperBound = 9.007199254740992E15*min(maxDistance, 192*ring + 167.99999999);

		double northwestAngle = atan2(approximationNorthEdge, approximationWestEdge);
		double northeastAngle = atan2(approximationNorthEdge, approximationEastEdge);
		double southwestAngle = atan2(approximationSouthEdge, approximationWestEdge);
		double southeastAngle = atan2(approximationSouthEdge, approximationEastEdge);
		double minAngle = min(min(northwestAngle, northeastAngle), min(southwestAngle, southeastAngle));
		double maxAngle = max(max(northwestAngle, northeastAngle), max(southwestAngle, southeastAngle));
		if (minAngle < 0) minAngle += U_TWO_PI;
		if (maxAngle < 0) maxAngle += U_TWO_PI;
		
	}
}