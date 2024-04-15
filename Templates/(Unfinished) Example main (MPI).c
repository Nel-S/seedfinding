#include "common.h"
#include <limits.h>
#include <mpi.h>
#include <string.h>

int commrank, commsize;
size_t _messageSize = 0;
FILE *inputFile = NULL;

// TODO: Generalize
const char *FORMAT = "%" PRId64 "\t%d\t%d";

bool getNextSeed(const void* workerIndex, uint64_t *seed) {
	if (INPUT_FILEPATH) {
		// TODO: Seriously needs to be tested
		for (int i = 0; i < (workerIndex ? *(int *)workerIndex : localNumberOfWorkers - 1); ++i) {
			if (fscanf(inputFile, " %" PRId64 " \n", (int64_t *)seed) != 1) return false;
		}
		return fscanf(inputFile, " %" PRId64 " \n", (int64_t *)seed) == 1;
	} else {
		*seed = workerIndex ? *(int *)workerIndex + localStartSeed : *seed + localNumberOfWorkers;
		return *seed - localStartSeed < localSeedsToCheck;
	}
}

static inline size_t getMessageSize(const char *format) {
	size_t size = 0;
	for (size_t i = 0; format[i] != '\0'; ++i) {
		if (format[i] == '%' && format[++i] != '%') ++size;
	}
	return size;
}

void outputValue(const char *format, ...) {
	va_list args;
	// Each process has its own copy of _messageSize, so there is no risk of race conditions here.
	if (!_messageSize) _messageSize = getMessageSize(format);
	unsigned long long message[_messageSize];
	va_start(args, format);
	// TODO: Figure out how to transmit strings?
	for (size_t i = 0; i < _messageSize; ++i) message[i] = va_arg(args, unsigned long long);
	va_end(args);
	MPI_Send(message, _messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
}

int main() {
	initGlobals();

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &commrank);
	if (commsize < 2) {
		fprintf(stderr, "Worker %d: main(): MPI_Comm_size(): commsize = %d must be 2 or greater (1 writer process, 1+ calculation processes).\n", commrank, commsize);
		exit(1);
	}

	if (commrank) {
		if (INPUT_FILEPATH) {
			inputFile = fopen(INPUT_FILEPATH, "w");
			if (!inputFile) {
				fprintf(stderr, "Worker %d: main(): fopen(INPUT_FILEPATH, \"w\"): %s could not be opened.\n", commrank, INPUT_FILEPATH);
				exit(1);
			}
		}
		localNumberOfWorkers = commsize - 1;
		int data = commrank - 1;
		runWorker(&data);
		if (INPUT_FILEPATH) fclose(inputFile);
		// Each process has its own copy of _messageSize, so there is no risk of race conditions here.
		if (!_messageSize) _messageSize = getMessageSize(FORMAT);
		unsigned long long message[_messageSize];
		for (size_t i = 0; i < _messageSize; ++i) message[i] = ULLONG_MAX;
		MPI_Send(message, _messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
	} else {
		FILE *outputFile = NULL;
		if (OUTPUT_FILEPATH) {
			outputFile = fopen(OUTPUT_FILEPATH, "w");
			if (!outputFile) {
				fprintf(stderr, "Worker %d: main(): fopen(OUTPUT_FILEPATH, \"w\"): %s could not be opened.\n", commrank, OUTPUT_FILEPATH);
				exit(1);
			}
		}
		// Starts the clock
		struct timespec startTime, endTime;
		if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);

		int openConnections = commsize - 1;
		// Each process has its own copy of _messageSize, so there is no risk of race conditions here.
		if (!_messageSize) _messageSize = getMessageSize(FORMAT);
		unsigned long long message[_messageSize];
		char printBuffer[5000];
		while (openConnections) {
			MPI_Recv(message, _messageSize, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			bool possibleTerminationSignal = true;
			for (size_t i = 0; i < _messageSize; ++i) {
				if (message[i] != ULLONG_MAX) possibleTerminationSignal = false;
			}
			if (possibleTerminationSignal) --openConnections;
			else {
				// TODO: Add support for non-LL representations
				for (size_t i = 0; i < _messageSize; ++i) fprintf(OUTPUT_FILEPATH ? outputFile : stdout, i == _messageSize - 1 ? "%lld\n" : "%lld\t", message[i]);
			}
		}
		if (OUTPUT_FILEPATH) {
			fflush(outputFile);
			fclose(outputFile);
		}
		if (TIME_PROGRAM) {
			clock_gettime(CLOCK_MONOTONIC, &endTime);
			fprintf(stdout, "(%.9g seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
		}
	}
	MPI_Finalize();
	return 0;
}