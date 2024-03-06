/*
	A C program to rapidly find 1.18+ Minecraft Java spawns far from the origin.
	This program can search for two types of spawns:
		- Spawns within a particular "ring", given the spawn algorithm initially checks five spaced-out "rings" of points
		  to determine what distance range the spawn will fall within. Note that this may return more seeds than you intend; I
		  previously estimated that there are about 22.8 trillion seeds with third-ring spawns, for example. Output will be of the form
		  [Seed] [Position], where
			 1 -  7 = Ring 1,
			 8 - 20 = Ring 2,
			21 - 39 = Ring 3, and
			40 - 65 = Ring 4.
		  (Ring 0 is not supported because every seed meets the criteria of potentially being a zeroth-ring spawn, meaning every
		  seed would just be outputted.)
		  (If you desire more precision, the exact coordinates corresponding to each position number are stored in FIRST_STAGE_POINTS[]
		  in constants.h.)
		- Spawns whose approximate locations are a given distance from the origin. Note that this program does not model the
		  last stage of the spawn algorithm, so all results have a maximum error of 95sqrt(2) ~ 134.4 blocks. With that in mind,
		  outputs will be of the form [Seed] [Approximate spawn X] [Approximate spawn Z] [Approximate spawn distance from origin].

	Note: the seedspace this program must operate within is unimaginably large (2^64 ~ 18 quintillion possible seeds), so unless you have an incredible computer,
		do not expect to iterate over every possible seed anytime within the next century. But when compiled on GCC with -O3 optimization, this seems
		to be capable of running at 153000 seeds/second/thread on my own machine, 518x faster than a default spawn search in Cubiomes Viewer on a single thread.

	(This program was used to find 2242180942571 and 2939217260932 (with spawnpoints 1759.0 blocks from the origin and 1673 blocks from
		the origin along one axis, respectively), which are the farthest 1.18+ normal-biomes spawns of either type in the first 3.1 trillion seeds.)

	Almost all of the code below has been adapted (though heavily optimized) from the Cubiomes library by Cubitect et.al.
	(https://github.com/Cubitect/cubiomes), which is released under the MIT License.
	That repository, in turn, is a C adaptation of certain worldgen functions performed in Java by Minecraft, which is the
	property of Microsoft (and was presumably decompiled legally by Cubitect).
	All xoroshiro128++ functions were developed by David Blackman and Sebastiano Vigna in 2019 (https://prng.di.unimi.it/xoroshiro128plusplus.c)
	and are public domain.
*/

// Imports settings and constants
#include "functions.c"

int main() {
	// Initialize Cdouble table
	for (uint8_t i = 0; i < RING_STARTING_INDEX; ++i) {
		for (size_t j = 0; j < sizeof(MAX_OCTAVE_AMPLITUDE_SUMS)/sizeof(*MAX_OCTAVE_AMPLITUDE_SUMS); ++j) {
			Cdouble[i][j]  = MAX_OCTAVE_AMPLITUDE_SUMS[j] - (1100 + sqrt(FITNESS - COORDS[i][4]))/(10000.*AMPLITUDES[2]);
		}
	}
	pthread_t threads[NUMBER_OF_THREADS];
	ThreadData data[NUMBER_OF_THREADS];
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) {
		data[i].id = i;
		pthread_create(&threads[i], NULL, checkSeed, &data[i]);
	}
	for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) pthread_join(threads[i], NULL);
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		printf("(%.17g seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}
