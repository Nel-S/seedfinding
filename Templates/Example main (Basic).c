#include "common.h"

FILE *inputFile = NULL, *outputFile = NULL;

bool getNextSeed(const void* workerIndex, uint64_t *seed) {
	if (INPUT_FILEPATH) return fscanf(inputFile, " %" PRId64 " \n", (int64_t *)seed) == 1;
	else {
		*seed = workerIndex ? *(int *)workerIndex + localStartSeed : *seed + localNumberOfWorkers;
		return *seed - localStartSeed < localSeedsToCheck;
	}
}

void outputValue(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(OUTPUT_FILEPATH ? outputFile : stdout, format, args);
	va_end(args);
}

int main() {
	if (INPUT_FILEPATH) {
		inputFile = fopen(INPUT_FILEPATH, "r");
		if (!inputFile) {
			fprintf(stderr, "Example main (Basic).c: main(): fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
			exit(1);
		}
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "w");
		if (!outputFile) {
			fprintf(stderr, "Example main (Basic).c: main(): fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
			exit(1);
		}
	}
	initGlobals();
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	int data = 0;
	runWorker(&data);
	if (INPUT_FILEPATH) fclose(inputFile);
	if (OUTPUT_FILEPATH) {
		fflush(outputFile);
		fclose(outputFile);
	}
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		printf("(%.9g seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}