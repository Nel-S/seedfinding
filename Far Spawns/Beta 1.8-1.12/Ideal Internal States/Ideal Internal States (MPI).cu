// Finds the internal java.util.Random states that could displace the player's spawnpoint the farthest in Minecraft Java Beta 1.8(?) - 1.12.
// (Much of this was taken from https://github.com/KaptainWutax/Kaktoos/blob/master/kaktoos.cu.)

// IDE indexing
#ifdef __JETBRAINS_IDE__
#define __host__
#define __device__
#define __shared__
#define __constant__
#define __global__
#define __CUDACC__
#include <device_functions.h>
#include <__clang_cuda_builtin_vars.h>
#include <__clang_cuda_intrinsics.h>
#include <__clang_cuda_math_forward_declares.h>
#include <__clang_cuda_complex_builtins.h>
#include <__clang_cuda_cmath.h>
#endif

#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
// #include <chrono>
#include <mutex>
#include <thread>

#define SEEDS_ARRAY_CAPACITY 1024

const uint64_t GLOBAL_START_STATE = 0;
const uint64_t GLOBAL_STATES_TO_CHECK = 1ULL << 36;
const uint64_t BLOCK_SIZE = 256;
const uint64_t WORK_UNIT_SIZE = 1ULL << 23;
// Best state so far: 1459339358529, (-4, -6357) = 40411465
// const uint64_t DIST_THRESHOLD = 37000000;
const uint64_t DIST_THRESHOLD = 30000000;
const std::string FILEPATH_PREFIX = "internalStates";

// const size_t NODES_COUNT = 1;
// const size_t GPUS_PER_NODE = 1;
// The maximum number of GPUs the system will try to use per cluster node; you shouldn't need to touch this
const int MAX_GPUS_PER_NODE = INT_MAX;

// -----------------------------------------------------------------------
// GPU Helper Functions

// new Random(seed)
__device__ inline void setSeed(uint64_t *random, const uint64_t seed) {
    *random = (seed ^ 25214903917ULL) & ((1ULL << 48) - 1);
}

// Random::nextInt(bound)
__device__ int32_t nextInt(uint64_t *seed, int16_t bound) {
    if ((bound & -bound) == bound) {
        *seed = (*seed * 25214903917ULL + 11ULL) & ((1ULL << 48) - 1);
        return (int32_t)((bound * (*seed >> 17)) >> 31);
    }
    int32_t bits, value;
    do {
        *seed = (*seed * 25214903917ULL + 11ULL) & ((1ULL << 48) - 1);
        bits = *seed >> 17;
        value = bits % bound;
    } while (bits - value + (bound - 1) < 0);
    return value;
}
// -------------------------------------------------------------------------
// Main GPU code

typedef struct {
    uint_fast64_t internalState;
    int_fast16_t i;
    uint_fast64_t squaredDist;
} SeedsArray;
enum {messageInternalState, messageI, messageSquaredDist, messageSize};

typedef struct {
    uint64_t startState, statesToCheck;
    SeedsArray *seedsArray;
    unsigned long long *seedsArraySize;
} GPUnode;

__global__ void test(uint64_t offse, SeedsArray *seedsArray, unsigned long long *seedsArraySize) {
    uint_fast64_t state = blockIdx.x * blockDim.x + threadIdx.x + offse;
    uint_fast64_t random = state;
    uint_fast64_t currentDist, bestDist = 0;
    int_fast16_t posX = 0, posZ = 0, bestI;
    for (int_fast16_t i = 0; i < 1000; ++i) {
        int disp = nextInt(&random, 64);
        posX += disp - nextInt(&random, 64);
        disp = nextInt(&random, 64);
        posZ += disp - nextInt(&random, 64);
        currentDist = posX * posX + posZ * posZ;
        if (currentDist > bestDist) {
            bestDist = currentDist;
            bestI = i;
        }
    }
    if (bestDist >= DIST_THRESHOLD) {
        // printf("%" PRId64"\t%d\t%d\t(%d)\t%" PRIu64 "\n", state, posX, posZ, i, currentDist);
        uint64_t index = atomicAdd(seedsArraySize, 1ull);
        if (index >= SEEDS_ARRAY_CAPACITY) return;
        seedsArray[index].internalState = state;
        seedsArray[index].i = bestI;
        seedsArray[index].squaredDist = bestDist;
    }
}
// ------------------------------------------------------------------------------------
// Host code and variables

int commrank, commsize;
// GPUnode nodes[GPUS_PER_NODE];
GPUnode *nodes;
uint64_t startState = GLOBAL_START_STATE, statesToCheck = GLOBAL_STATES_TO_CHECK, offset = 0;
std::mutex offsetMutex;
#define MIN(X,Y) (X < Y ? X : Y)

void gpu_manager(int index) {
    cudaError_t error;
    if (error = cudaSetDevice(index)) {
        fprintf(stderr, "Node %d: gpu_manager(%d): cudaSetDevice(): %s (%s).\n", commrank, index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }
    if (error = cudaMallocManaged(&nodes[index].seedsArray, sizeof(*nodes->seedsArray) * SEEDS_ARRAY_CAPACITY)) {
        fprintf(stderr, "Node %d: gpu_manager(%d): cudaMallocManaged(seedsArray): %s (%s).\n", commrank, index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }
    if (error = cudaMallocManaged(&nodes[index].seedsArraySize, sizeof(*nodes->seedsArraySize))) {
        fprintf(stderr, "Node %d: gpu_manager(%d): cudaMallocManaged(seedsArraySize): %s (%s).\n", commrank, index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }

    unsigned long long message[messageSize];
    while (offset < statesToCheck) {
        *nodes[index].seedsArraySize = 0;
        test<<<WORK_UNIT_SIZE / BLOCK_SIZE, BLOCK_SIZE, 0>>>(startState + offset, nodes[index].seedsArray, nodes[index].seedsArraySize);
        offsetMutex.lock();
        offset += WORK_UNIT_SIZE;
        offsetMutex.unlock();
        if (error = cudaDeviceSynchronize()) {
            fprintf(stderr, "Node %d: gpu_manager(%d): cudaDeviceSynchronize(): %s (%s).\n", commrank, index, cudaGetErrorString(error), cudaGetErrorName(error));
            exit(1);
        }
        for (uint64_t i = 0; i < *nodes[index].seedsArraySize && i < SEEDS_ARRAY_CAPACITY; ++i) {
            message[messageInternalState] = nodes[index].seedsArray[i].internalState;
            message[messageI] = nodes[index].seedsArray[i].i;
            message[messageSquaredDist] = nodes[index].seedsArray[i].squaredDist;
            MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }
    message[messageInternalState] = message[messageI] = message[messageSquaredDist] = ULLONG_MAX;
    MPI_Send(message, messageSize, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
}

int main() {
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &commrank);
    if (commsize < 2) {
        fprintf(stderr, "Node %d: MPI_Comm_size(): commsize = %d must be 2 or greater (1 writer process, 1+ calculation processes).\n", commrank, commsize);
        exit(1);
    }

    if (commrank) {
        cudaError_t error;
        int gpuCount;
        if (error = cudaGetDeviceCount(&gpuCount)) {
            fprintf(stderr, "Node %d: cudaGetDeviceCount(): %s (%s).\n", commrank, cudaGetErrorString(error), cudaGetErrorName(error));
            exit(1);
        }
        if (gpuCount > MAX_GPUS_PER_NODE) gpuCount = MAX_GPUS_PER_NODE;
        if (gpuCount <= 0) {
            fprintf(stderr, "Node %d: cudaGetDeviceCount(): Zero or an invalid number of CUDA devices were found (%d).\n", commrank, gpuCount);
            exit(1);
        }
        nodes = (GPUnode *)malloc(gpuCount*sizeof(GPUnode));
        if (!nodes) {
            fprintf(stderr, "Node %d: nodes = malloc(%zd): Could not allocate %zd bytes.\n", commrank, gpuCount*sizeof(GPUnode), gpuCount*sizeof(GPUnode));
            exit(1);
        }
        statesToCheck = GLOBAL_STATES_TO_CHECK/(commsize - 1) + (commrank - 1 < GLOBAL_STATES_TO_CHECK % (commsize - 1));
        startState = GLOBAL_START_STATE + (GLOBAL_STATES_TO_CHECK/(commsize - 1))*(commrank - 1) + MIN(commrank - 1, GLOBAL_STATES_TO_CHECK % (commsize - 1));
        
        std::thread threads[gpuCount];
        for (int i = 0; i < gpuCount; ++i) threads[i] = std::thread(gpu_manager, i);
        for (int i = 0; i < gpuCount; ++i) threads[i].join();
        free(nodes);
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