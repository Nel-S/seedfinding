#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

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
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	int data = 0;
	checkSeeds(&data);
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		printf("(%f seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}