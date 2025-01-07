#include "../utilities/core/common_seedfinding.h"
#include "../utilities/cubiomes/finders.h"
#include <unordered_set>
#include <utility>

const uint64_t GLOBAL_START_SEED = 0;
const uint64_t GLOBAL_SEEDS_TO_CHECK = 1ULL << 48;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH = NULL;
const char *OUTPUT_FILEPATH = NULL;
const Pos COORDINATE_TO_CHECK = {-27, 16};
const int INITIAL_BEST_COUNT = 2;
const bool TIME_PROGRAM = false;


DEFAULT_LOCALS_INITIALIZATION

int bestCount;

struct pairHash {
    [[nodiscard]] std::size_t operator()(std::pair<int, int> const &v) const {
        // return ((v.first + v.second) * (v.first + v.second + 1) / 2) + v.second;
        return (v.second << 16) ^ v.first;
    }
};

void initGlobals() {
    bestCount = INITIAL_BEST_COUNT;
}

[[nodiscard]] int testForSlimeAt(uint64_t seed, int chunkX, int chunkZ, std::unordered_set<std::pair<int, int>, pairHash> *set) {
    set->insert(std::pair(chunkX, chunkZ));
    if (!isSlimeChunk(seed, chunkX, chunkZ)) return 0;
    int count = 1;
    if (set->find(std::pair(chunkX + 1, chunkZ)) == set->end() && chunkX + 1 <=  29999999) count += testForSlimeAt(seed, chunkX + 1, chunkZ, set);
    if (set->find(std::pair(chunkX - 1, chunkZ)) == set->end() && chunkX - 1 >= -30000000) count += testForSlimeAt(seed, chunkX - 1, chunkZ, set);
    if (set->find(std::pair(chunkX, chunkZ + 1)) == set->end() && chunkZ + 1 <=  29999999) count += testForSlimeAt(seed, chunkX, chunkZ + 1, set);
    if (set->find(std::pair(chunkX, chunkZ - 1)) == set->end() && chunkZ - 1 >= -30000000) count += testForSlimeAt(seed, chunkX, chunkZ - 1, set);
    return count;
}

void *runWorker(void *workerIndex) {
    std::unordered_set<std::pair<int, int>, pairHash> checkedChunks;
    uint64_t seed;
    if (!getNextSeed(workerIndex, &seed)) return NULL;
    do {
        checkedChunks.clear();
        int currentCount = testForSlimeAt(seed, COORDINATE_TO_CHECK.x, COORDINATE_TO_CHECK.z, &checkedChunks);
        if (currentCount < bestCount) continue;
        outputValues("%" PRId64 "\t%d\n", seed, currentCount);
        if (bestCount < currentCount) bestCount = currentCount;
    } while (getNextSeed(NULL, &seed));
    return NULL;
}