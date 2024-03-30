#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

extern const int GLOBAL_NUMBER_OF_WORKERS;
extern const bool TIME_PROGRAM;
extern const char *FILEPATH;
extern void initGlobals();
extern void *checkSeeds(void *workerIndex);

FILE *file;

void outputValues(const uint64_t *seeds, const void *otherValues, const size_t count) {
	for (size_t i = 0; i < count; ++i) {
		if (!otherValues) fprintf(FILEPATH ? file : stdout, "%" PRId64 "\n", seeds[i]);
		else fprintf(FILEPATH ? file : stdout, "%" PRId64 "\t(%d)\n", seeds[i], ((int *)otherValues)[i]);
	}
}

int main() {
	file = NULL;
	if (FILEPATH) file = fopen(FILEPATH, "w");

	initGlobals();
	// Starts the clock
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	pthread_t threads[GLOBAL_NUMBER_OF_WORKERS];
	int data[GLOBAL_NUMBER_OF_WORKERS];
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) {
		data[i] = i;
		pthread_create(&threads[i], NULL, checkSeeds, &data[i]);
	}
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) pthread_join(threads[i], NULL);
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		fprintf(stdout, "(%f seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}