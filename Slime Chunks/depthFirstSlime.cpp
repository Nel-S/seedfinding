#include "cubiomes/finders.h"
#include <unordered_set>
#include <utility>

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = 1ULL << 48;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const Pos COORDINATE_TO_CHECK = {-27, 16};
const int INITIAL_BEST_COUNT = 2;
const bool TIME_PROGRAM = false;


uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK;
int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
extern void outputValues(const uint64_t *seeds, const void *otherValues, size_t count);

int bestCount = INITIAL_BEST_COUNT;

struct pair_hash {
    std::size_t operator()(std::pair<int, int> const &v) const {
        // return ((v.first + v.second) * (v.first + v.second + 1) / 2) + v.second;
        return (v.second << 16) ^ v.first;
    }
};

void initGlobals() {}

int testForSlimeAt(uint64_t seed, int chunkX, int chunkZ, std::unordered_set<std::pair<int, int>, pair_hash> *set) {
    set->insert(std::pair(chunkX, chunkZ));
    if (!isSlimeChunk(seed, chunkX, chunkZ)) return 0;
    int count = 1;
    if (chunkX + 1 <=  29999999 && set->find(std::pair(chunkX + 1, chunkZ)) == set->end()) count += testForSlimeAt(seed, chunkX + 1, chunkZ, set);
    if (chunkX - 1 >= -30000000 && set->find(std::pair(chunkX - 1, chunkZ)) == set->end()) count += testForSlimeAt(seed, chunkX - 1, chunkZ, set);
    if (chunkZ + 1 <=  29999999 && set->find(std::pair(chunkX, chunkZ + 1)) == set->end()) count += testForSlimeAt(seed, chunkX, chunkZ + 1, set);
    if (chunkZ - 1 >= -30000000 && set->find(std::pair(chunkX, chunkZ - 1)) == set->end()) count += testForSlimeAt(seed, chunkX, chunkZ - 1, set);
    return count;
}

void *checkSeed(void *workerIndex) {
    std::unordered_set<std::pair<int, int>, pair_hash> checkedChunks;
    for (uint64_t count = *(int *)workerIndex; count < localSeedsToCheck; count += localNumberOfWorkers) {
        uint64_t seed = localStartSeed + count;
        checkedChunks.clear();
        int currentCount = testForSlimeAt(seed, COORDINATE_TO_CHECK.x, COORDINATE_TO_CHECK.z, &checkedChunks);
        if (currentCount > bestCount) {
            bestCount = currentCount;
            outputValues(&seed, &currentCount, 1);
        }
    }
    return NULL;
}