#include "core/bruteforce.h"
#include "utilities/cubiomes/finders.h"
#include <unordered_set>
#include <utility>

const uint64_t GLOBAL_START_INTEGER = 0;
const uint64_t GLOBAL_NUMBER_OF_INTEGERS = 1ULL << 48;
const int GLOBAL_NUMBER_OF_WORKERS = 4;
const char *INPUT_FILEPATH = NULL;
const char *OUTPUT_FILEPATH = NULL;

const Pos COORDINATE_TO_CHECK = {0, 0};
const int INITIAL_BEST_COUNT = 2;
const int MAX_EMPTY_AREA = 10;
const bool TIME_PROGRAM = false;


DEFAULT_LOCALS_INITIALIZATION

int bestCount;

struct pair_hash {
    [[nodiscard]] std::size_t operator()(std::pair<int, int> const &v) const {
        // return ((v.first + v.second) * (v.first + v.second + 1) / 2) + v.second;
        return (v.second << 16) ^ v.first;
    }
};

void initializeGlobals() {
    bestCount = INITIAL_BEST_COUNT;
}

[[nodiscard]] int testForSlimeAt(uint64_t seed, int chunkX, int chunkZ, std::unordered_set<std::pair<int, int>, pair_hash> *set, int depth) {
    if (depth > MAX_EMPTY_AREA + 1) return -100;
    set->insert(std::pair(chunkX, chunkZ));
    if (isSlimeChunk(seed, chunkX, chunkZ)) return 0;
    int count = 1;
    if (chunkX + 1 <=  29999999 && set->find(std::pair(chunkX + 1, chunkZ)) == set->end()) count += testForSlimeAt(seed, chunkX + 1, chunkZ, set, depth + 1);
    if (chunkX - 1 >= -30000000 && set->find(std::pair(chunkX - 1, chunkZ)) == set->end()) count += testForSlimeAt(seed, chunkX - 1, chunkZ, set, depth + 1);
    if (chunkZ + 1 <=  29999999 && set->find(std::pair(chunkX, chunkZ + 1)) == set->end()) count += testForSlimeAt(seed, chunkX, chunkZ + 1, set, depth + 1);
    if (chunkZ - 1 >= -30000000 && set->find(std::pair(chunkX, chunkZ - 1)) == set->end()) count += testForSlimeAt(seed, chunkX, chunkZ - 1, set, depth + 1);
    return count;
}

void *runWorker(void *workerIndex) {
    std::unordered_set<std::pair<int, int>, pair_hash> checkedChunks;
    uint64_t seed;
    if (!getNextInteger(workerIndex, &seed)) return NULL; 
    do {
        checkedChunks.clear();
        int currentCount = testForSlimeAt(seed, COORDINATE_TO_CHECK.x, COORDINATE_TO_CHECK.z, &checkedChunks, 0);
        if (currentCount < bestCount) continue;
        outputString("%" PRId64 "\t%d\n", seed, currentCount);
        if (currentCount > bestCount) bestCount = currentCount;
    } while (getNextInteger(NULL, &seed));
    return NULL;
}