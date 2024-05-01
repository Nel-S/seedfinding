#include "utilities/cubiomes/rng.h"
#include <stdio.h>
#include <queue>

uint64_t reverse4000calls(uint64_t rng) {
	return (137992601189761 * rng + 154155999165792) & 0xffffffffffff;
}

int main() {
	// Queue to store the last 3999 nextInt results.
	std::queue<int8_t> queue;
	// The internal state of the PRNG.
	uint64_t rng = 0;
	/* evenOne and evenTwo track the X/Z (or Z/X) coordinates corresponding to the last 4000 nextInt results, assuming an even offset from the original RNG state.
	   oddOne  and oddTwo  track the X/Z (or Z/X) coordinates corresponding to the last 4000 nextInt results, assuming an odd  offset from the original RNG state.*/
	int evenOne = 0, evenTwo = 0, oddOne = 0, oddTwo = 0;

	uint64_t currentSquaredDistance, bestSquaredDistance = 0;

	// The first 3999 iterations store the first 3999 nextInt calls in the queue, building all but one of the even set, and all but two of the odd set, in the process. 
	for (uint64_t i = 0; i < 3999 + (1ULL << 3); ++i) {
		int currentCall = nextInt(&rng, 64);
		queue.push(currentCall);
		switch (i & 0b11) {
			case 0:
				evenOne += currentCall;
				// Ignore the very first call for the odd offset
				if (i) oddTwo -= currentCall;
				break;
			case 1:
				evenOne -= currentCall;
				oddOne += currentCall;
				break;
			case 2:
				evenTwo += currentCall;
				oddOne -= currentCall;
				break;
			case 3:
				evenTwo -= currentCall;
				oddTwo += currentCall;
		}
		// if (i & 0b10 < 2) printf("\t%" PRIu64 ": %d\t%d\n", i, evenOne, evenTwo);
		if (i >= 3999) {
			currentSquaredDistance = i & 1 ? evenOne * evenOne + evenTwo * evenTwo : oddOne * oddOne + oddTwo * oddTwo;
			// if (currentSquaredDistance >= bestSquaredDistance) {
				printf("%" PRIu64 "\t(%d,\t%d)\t= %" PRIu64 "\t(%" PRIu64 ")\n", reverse4000calls(rng), i & 1 ? evenOne : oddOne, i & 1 ? evenTwo : oddTwo, currentSquaredDistance, i);
				if (currentSquaredDistance > bestSquaredDistance) bestSquaredDistance = currentSquaredDistance;
			// }
			int call4000prior = queue.front();
			queue.pop();
			switch (i & 0b11) {
				case 0:
					evenOne -= call4000prior;
					oddTwo += call4000prior;
					break;
				case 1:
					evenOne += call4000prior;
					oddOne -= call4000prior;
					break;
				case 2:
					evenTwo -= call4000prior;
					oddOne += call4000prior;
					break;
				case 3:
					evenTwo += call4000prior;
					if (i != 3999) oddTwo -= call4000prior;
			}
		}
	}
	return 0;
}