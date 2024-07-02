#include "../common.h"
#include <pthread.h>

FILE *inputFile = NULL, *outputFile = NULL;
pthread_mutex_t outputMutex;

bool getNextSeed(const void* workerIndex, uint64_t *seed) {
	if (INPUT_FILEPATH) return fscanf(inputFile, " %" PRId64 " \n", (int64_t *)seed) == 1;
	else {
		*seed = workerIndex ? *(int *)workerIndex + localStartSeed : *seed + localNumberOfWorkers;
		return *seed - localStartSeed < localSeedsToCheck;
	}
}

void outputValue(const char *format, ...) {
	// vfprintf is thread-safe when printing to stdout, but not when printing to a file.
	// TODO: Is there any way to do this without mutexes?
	if (OUTPUT_FILEPATH) pthread_mutex_lock(&outputMutex);
	va_list args;
	va_start(args, format);
	// if (OUTPUT_FILEPATH) pthread_mutex_lock(&outputMutex);
	vfprintf(OUTPUT_FILEPATH ? outputFile : stdout, format, args);
	if (OUTPUT_FILEPATH)  {
		fflush(OUTPUT_FILEPATH ? outputFile : stdout);
		// pthread_mutex_unlock(&outputMutex);
	}
	va_end(args);
	if (OUTPUT_FILEPATH) pthread_mutex_unlock(&outputMutex);
}

int main() {
	if (INPUT_FILEPATH) {
		inputFile = fopen(INPUT_FILEPATH, "r");
		if (!inputFile) {
			fprintf(stderr, "Example main (pthreads).c: int main(): inputFile = fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
			exit(1);
		}
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "w");
		if (!outputFile) {
			fprintf(stderr, "Example main (pthreads).c: int main(): outputFile = fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
			exit(1);
		}
	}
	initGlobals();
	// pthread_mutex_init(&outputMutex, NULL);
	// Starts the clock
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	pthread_t threads[GLOBAL_NUMBER_OF_WORKERS];
	int data[GLOBAL_NUMBER_OF_WORKERS];
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) {
		data[i] = i;
		pthread_create(&threads[i], NULL, runWorker, &data[i]);
	}
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) pthread_join(threads[i], NULL);
	if (INPUT_FILEPATH) fclose(inputFile);
	if (OUTPUT_FILEPATH) {
		fflush(outputFile);
		fclose(outputFile);
	}
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		fprintf(stdout, "(%.9g seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}