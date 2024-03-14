// Finds the internal states that could displace the player's spawnpoint the farthest in Minecraft Java Beta 1.8(?) - 1.12.
// This was almost entirely ripped from https://github.com/KaptainWutax/Kaktoos/blob/master/kaktoos.cu.

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
// #include <memory.h>
#include <stdio.h>
// #include <time.h>
#include <thread>
// #include <vector>
#include <mutex>
#include <chrono>

#define SEEDS_ARRAY_CAPACITY 1024

const uint64_t START_STATE = 0;
const uint64_t STATES_TO_CHECK = 1ULL << 36;
const uint64_t BLOCK_SIZE = 256;
const uint64_t WORK_UNIT_SIZE = 1ULL << 23;
// const uint64_t DIST_THRESHOLD = 40411465; // Best so far: 1459339358529   -4      -6357
// const uint64_t DIST_THRESHOLD = 37000000;
const uint64_t DIST_THRESHOLD = 30000000;
const std::string FILEPATH_PREFIX = "internalStates";

#ifndef GPU_COUNT
#define GPU_COUNT 1
#endif

// -----------------------------------------------------------------------
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

typedef struct {
    uint_fast64_t internalState;
    int_fast16_t i;
    uint_fast64_t squaredDist;
} SeedsArray;

typedef struct {
    int GPU;
    SeedsArray *seedsArray;
    uint32_t *seedsArraySize;
} GPUnode;

__global__ void test(uint64_t offse, SeedsArray *seedsArray, uint32_t *seedsArraySize) {
    uint_fast64_t state = START_STATE + blockIdx.x * blockDim.x + threadIdx.x + offse;
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
        uint32_t index = atomicAdd(seedsArraySize, 1u);
        if (index >= SEEDS_ARRAY_CAPACITY) return;
        seedsArray[index].internalState = state;
        seedsArray[index].i = bestI;
        seedsArray[index].squaredDist = bestDist;
    }
}
// ------------------------------------------------------------------------------------

GPUnode nodes[GPU_COUNT];
uint64_t offset = 0;
std::mutex offsetMutex; //, fileMutex;
// FILE *file;

void gpu_manager(int index) {
    cudaError_t error;
    if (error = cudaSetDevice(index)) {
        fprintf(stderr, "gpu_manager(%d): cudaSetDevice(): %s (%s).\n", index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }
    FILE *file = fopen((FILEPATH_PREFIX + "_" + std::to_string(index) + ".txt").c_str(), "w");
    if (!file) {
        fprintf(stderr, "fopen(): Filepath %s could not be opened.\n", FILEPATH_PREFIX.c_str());
        exit(1);
    }
    nodes[index].GPU = index;
    if (error = cudaMallocManaged(&nodes[index].seedsArray, sizeof(*nodes->seedsArray) * SEEDS_ARRAY_CAPACITY)) {
        fprintf(stderr, "gpu_manager(%d): cudaMallocManaged(seedsArray): %s (%s).\n", index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }
    if (error = cudaMallocManaged(&nodes[index].seedsArraySize, sizeof(*nodes->seedsArraySize))) {
        fprintf(stderr, "gpu_manager(%d): cudaMallocManaged(seedsArraySize): %s (%s).\n", index, cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }

    while (offset < STATES_TO_CHECK) {
        *nodes[index].seedsArraySize = 0;
        test<<<WORK_UNIT_SIZE / BLOCK_SIZE, BLOCK_SIZE, 0>>>(offset, nodes[index].seedsArray, nodes[index].seedsArraySize);
        offsetMutex.lock();
        offset += WORK_UNIT_SIZE;
        offsetMutex.unlock();
        if (error = cudaDeviceSynchronize()) {
            fprintf(stderr, "gpu_manager(%d): cudaDeviceSynchronize(): %s (%s).\n", index, cudaGetErrorString(error), cudaGetErrorName(error));
            exit(1);
        }
        // fileMutex.lock();
        for (uint32_t i = 0; i < *nodes[index].seedsArraySize && i < SEEDS_ARRAY_CAPACITY; ++i) {
            fprintf(file, "%" PRIuFAST64 "\t%" PRIdFAST16 "\t%" PRIuFAST64 "\n", nodes[index].seedsArray[i].internalState, nodes[index].seedsArray[i].i, nodes[index].seedsArray[i].squaredDist);
        }
        fflush(file);
        // fileMutex.unlock();
    }
    fclose(file);
}

int main() {
    // file = fopen(FILEPATH_PREFIX.c_str(), "w");
    // if (!file) {
    //     fprintf(stderr, "fopen(): Filepath %s could not be opened.\n", FILEPATH_PREFIX.c_str());
    //     exit(1);
    // }

    std::thread threads[GPU_COUNT];
    // time_t startTime = time(NULL), currentTime;
    for (int i = 0; i < GPU_COUNT; i++) threads[i] = std::thread(gpu_manager, i);
    for (int i = 0; i < GPU_COUNT; i++) threads[i].join();

    // using namespace std::chrono_literals;
    // while (offset < STATES_TO_CHECK) {
    // //     time(&currentTime);
    // //     int timeElapsed = (int)(currentTime - startTime);
    // //     double speed = (double)(offset) / (double)timeElapsed / 1000000.0;
    // //     printf("Searched %lld seeds, offset: %lld. Time elapsed: %ds. Speed: %.2fm seeds/s. %f%%\n", (long long int)(offset), (long long int)offset, timeElapsed, speed, (double)offset / SEEDS_TO_CHECK * 100);
    //     std::this_thread::sleep_for(3s);
    // }

    // fclose(file);
    // time(&currentTime);
    // printf("(%d seconds)\n", (int)(currentTime - startTime));
    return 0;
}