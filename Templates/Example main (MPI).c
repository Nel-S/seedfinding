#include <inttypes.h>
#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <time.h>

extern int localNumberOfProcesses;
extern const bool TIME_PROGRAM;
extern const char *FILEPATH;
extern void initGlobals();
extern void *checkSeeds(void *workerIndex);

int commrank, commsize;
enum {messageSeed, messageOtherValue, messageSize};

void outputValues(const uint64_t *seeds, const void *otherValues, const size_t count) {
	for (size_t i = 0; i < count; ++i) {
		unsigned long long message[messageSize] = {seeds[i], otherValues ? ((unsigned long long *)otherValues)[i] : ULLONG_MAX};
		MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
	}
}

int main() {
	initGlobals();

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &commrank);
	if (commsize < 2) {
		fprintf(stderr, "Node %d: MPI_Comm_size(): commsize = %d must be 2 or greater (1 writer process, 1+ calculation processes).\n", commrank, commsize);
		exit(1);
	}

	if (commrank) {
		localNumberOfProcesses = commsize - 1;
		int data = commrank - 1;
		checkSeeds(&data);
		unsigned long long message[messageSize] = {ULLONG_MAX, ULLONG_MAX};
		MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
	} else {
		FILE *file = FILEPATH ? fopen(FILEPATH, "w") : NULL;
		if (FILEPATH && !file) {
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
			if (message[messageSeed] == ULLONG_MAX && message[messageOtherValue] == ULLONG_MAX) --openConnections;
			else {
				if (message[messageOtherValue] == ULLONG_MAX) fprintf(FILEPATH ? file : stdout, "%lld\n", message[messageSeed]);
				else fprintf(FILEPATH ? file : stdout, "%lld\t%d\n", message[messageSeed], message[messageOtherValue]);
			}
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