// Finds the internal states that could displace the player's spawnpoint the farthest in Minecraft Java Beta 1.8(?) - 1.12.

#include <cuda.h>
#include <cinttypes>
#include <cstdio>

constexpr const uint64_t START_STATE = 285936611421756;
constexpr const uint64_t STATES_TO_CHECK = (1ULL << 48) - START_STATE;
constexpr const uint64_t BLOCK_SIZE = 256;
constexpr const uint64_t WORK_UNIT_SIZE = 1ULL << 32;
// const uint64_t DIST_THRESHOLD = 40411465; // Best so far: 1459339358529   -4      -6357
// const uint64_t DIST_THRESHOLD = 37000000;
constexpr const uint64_t DIST_THRESHOLD = 36178138;
constexpr const char *FILEPATH = "internalStates (36178138).txt";

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

constexpr uint64_t SEEDS_ARRAY_CAPACITY = 8192;
__managed__ SeedsArray seedsArray[SEEDS_ARRAY_CAPACITY];
__managed__ uint64_t seedsArraySize;

__global__ void test(uint64_t offse) {
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
    if (bestDist < DIST_THRESHOLD) return;
    // printf("%" PRId64"\t%d\t%d\t(%d)\t%" PRIu64 "\n", state, posX, posZ, i, currentDist);
    uint64_t index = atomicAdd(&seedsArraySize, 1);
    if (index >= SEEDS_ARRAY_CAPACITY) return;
    seedsArray[index] = {state, bestI, bestDist};
}
// ------------------------------------------------------------------------------------

template <class T>
constexpr T& min(const T &first, const T &second) {
    return first < second ? first : second;
}

int main() {
    cudaError_t error;
    if (error = cudaSetDevice(0)) {
        fprintf(stderr, "cudaSetDevice(): %s (%s).\n", cudaGetErrorString(error), cudaGetErrorName(error));
        exit(1);
    }
    FILE *file = fopen(FILEPATH, "w");
    if (!file) {
        fprintf(stderr, "fopen(): Filepath %s could not be opened.\n", FILEPATH);
        exit(1);
    }

    for (uint64_t offset = 0; offset < STATES_TO_CHECK; offset += WORK_UNIT_SIZE) {
        seedsArraySize = 0;
        test<<<WORK_UNIT_SIZE / BLOCK_SIZE, BLOCK_SIZE>>>(offset);
        if (error = cudaDeviceSynchronize()) {
            fprintf(stderr, "cudaDeviceSynchronize(): %s (%s).\n", cudaGetErrorString(error), cudaGetErrorName(error));
            exit(1);
        }
        // fileMutex.lock();
        for (uint64_t i = 0; i < min(seedsArraySize, SEEDS_ARRAY_CAPACITY); ++i) {
            printf("%" PRIuFAST64 "\t%" PRIdFAST16 "\t%" PRIuFAST64 "\n", seedsArray[i].internalState, seedsArray[i].i, seedsArray[i].squaredDist);
            fprintf(file, "%" PRIuFAST64 "\t%" PRIdFAST16 "\t%" PRIuFAST64 "\n", seedsArray[i].internalState, seedsArray[i].i, seedsArray[i].squaredDist);
        }
        fflush(stdout);
        fflush(file);
        // fileMutex.unlock();
    }
    fclose(file);
}