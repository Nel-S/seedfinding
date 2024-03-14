// Finds the internal java.util.Random states that could displace the player's spawnpoint the farthest in Minecraft Java Beta 1.8(?) - 1.12.
// Must be compiled and linked to 1_idealInternalStatesSplit-CUDA.cu to function.

#include <climits>
#include <mpi.h>
#include <stdio.h>
#include <string>

const std::string FILEPATH_PREFIX = "internalStates";

enum {messageInternalState, messageI, messageSquaredDist, messageSize};

// Defined in CUDA section
extern void launchNode(int commrank, int commsize);

extern void sendMessage(unsigned long long *message) {
    MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
}

int main() {
    MPI_Init(NULL, NULL);
    int commrank, commsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &commrank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    if (commsize < 2) {
        fprintf(stderr, "Node %d: MPI_Comm_size(): commsize = %d must be 2 or greater (1 writer process, 1+ calculation processes).\n", commrank, commsize);
        exit(1);
    }

    if (commrank) {
        launchNode(commrank, commsize);
    } else {
        FILE *file = fopen((FILEPATH_PREFIX + ".txt").c_str(), "w");
        if (!file) {
            fprintf(stderr, "Device %d: fopen(\"%s\", \"w\"): Filepath could not be opened.\n", commrank, (FILEPATH_PREFIX + ".txt").c_str());
            exit(1);
        }
        int openConnections = commsize - 1;
        unsigned long long message[messageSize];
        while (openConnections) {
            MPI_Recv(message, messageSize, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (message[messageInternalState] == ULLONG_MAX && message[messageI] == ULLONG_MAX && message[messageSquaredDist] == ULLONG_MAX) --openConnections;
            else fprintf(file, "%llu\t%llu\t%llu\n", message[messageInternalState], message[messageI], message[messageSquaredDist]);
        }
        fflush(file);
        fclose(file);
    }
    MPI_Finalize();
    return 0;
}