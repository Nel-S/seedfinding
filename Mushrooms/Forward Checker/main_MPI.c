/*
	A C program to rapidly find Minecraft Java spawns far from the origin.
	This program can search for two types of spawns:
		- Spawns within a particular "ring", given the spawn algorithm initially checks five spaced-out "rings" of points
		  to determine what distance range the spawn will be. Note that this may return more seeds than you intend; I previously
		  estimated that there are about 22.8 trillion seeds with third-ring spawns, for example. Output will be of the form
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

	Note: the seedspace this program must operate within is unimaginably large (2^64 ~ 18 quintillion possible seeds), so I have completely
		forsaken conciseness in the program below in the named of speed. The following code cannot be said to utilize good industry coding
		practices such as splitting repeated code into functions to avoid repeating oneself, etc. ...but in exchange, when compiled on GCC with 
		-O3 optimization, this seems to be capable at running at 94000 seeds/s on my machine, about 383x quicker than the default getSpawn()
		function in Cubiomes Viewer on a single thread.

	Almost all of the code below has been adapted (though heavily optimized) from the Cubiomes library by Cubitect et.al.
	(https://github.com/Cubitect/cubiomes), which is released under the MIT License.
	That repository, in turn, is a C adaptation of certain worldgen functions performed in Java by Minecraft, which is the
	property of Mojang A.B. (and was presumably decompiled legally by Cubitect).
	All Xoroshiro128++ functions were developed by David Blackman and Sebastiano Vigna in 2019 (https://prng.di.unimi.it/xoroshiro128plusplus.c)
	and are public domain.
*/

// Imports settings and constants
#include "functions.c"
#include <limits.h>
#include <mpi.h>
#include <stdio.h>

int commrank, commsize;
enum {messageSeed, messageDist, messageSize};

extern void outputValue(uint64_t seed, int dist) {
	unsigned long long message[messageSize] = {seed, dist};
	MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
}

int main() {
	for (ringStartingIndex = 0; ringStartingIndex < sizeof(COORDS)/sizeof(*COORDS) && COORDS[ringStartingIndex][4] <= FITNESS; ++ringStartingIndex);

	for (uint_fast8_t i = 0; i < ringStartingIndex; ++i) {
		for (size_t j = 0; j < sizeof(MAX_OCTAVE_AMPLITUDE_SUMS)/sizeof(*MAX_OCTAVE_AMPLITUDE_SUMS); ++j) {
			Cdouble[i][j] = MAX_OCTAVE_AMPLITUDE_SUMS[j] - (1100 + sqrt(FITNESS - COORDS[i][4]))/15000.;
		}
	}

	MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &commrank);
	if (commsize < 2) {
        fprintf(stderr, "Node %d: MPI_Comm_size(): commsize = %d must be 2 or greater (1 writer process, 1+ calculation processes).\n", commrank, commsize);
        exit(1);
    }

	if (commrank) {
		localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK/(commsize - 1) + (commrank - 1 < GLOBAL_SEEDS_TO_CHECK % (commsize - 1));
    	localStartSeed = GLOBAL_START_SEED + (GLOBAL_SEEDS_TO_CHECK/(commsize - 1))*(commrank - 1) + MIN(commrank - 1, GLOBAL_SEEDS_TO_CHECK % (commsize - 1));
		Data d;
		d.id = commrank;
		checkSeed(&d);
		unsigned long long message[messageSize] = {ULLONG_MAX, ULLONG_MAX};
    	MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
	} else {
		FILE *file = fopen(FILEPATH, "w");
        if (!file) {
            fprintf(stderr, "Device %d: fopen(\"%s\", \"w\"): Filepath could not be opened.\n", commrank, FILEPATH);
            exit(1);
        }
		// Starts the clock
		struct timespec startTime, endTime;
		if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);

        int openConnections = commsize - 1;
        unsigned long long message[messageSize];
        while (openConnections) {
            MPI_Recv(message, messageSize, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (message[messageSeed] == ULLONG_MAX && message[messageDist] == ULLONG_MAX) --openConnections;
            else fprintf(file, "%llu\t%llu\n", message[messageSeed], message[messageDist]);
        }
        fflush(file);
        fclose(file);

		if (TIME_PROGRAM) {
			clock_gettime(CLOCK_MONOTONIC, &endTime);
			fprintf(stdout, "(%f seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
		}
    }
    MPI_Finalize();
	return 0;
}