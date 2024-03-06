#include "../cubiomes/finders.h"
#include <cinttypes>
#include <cstdio>
#include <pthread.h>
#include <unordered_set>
#include <utility>

const uint64_t START_SEED = 0;
const uint64_t SEEDS_TO_CHECK = 1ULL << 48;
const uint8_t NUMBER_OF_THREADS = 4;
const int MAX_EMPTY_AREA = 10;

int bestCount;

typedef struct {
    uint8_t index;
} ThreadData;

struct pair_hash {
    std::size_t operator()(std::pair<int, int> const &v) const {
        // return ((v.first + v.second) * (v.first + v.second + 1) / 2) + v.second;
        return (v.second << 16) ^ v.first;
    }
};

int testForSlimeAt(uint64_t seed, int chunkX, int chunkZ, std::unordered_set<std::pair<int, int>, pair_hash> *set, int depth) {
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

void *checkSeed(void *dat) {
    ThreadData *data = (ThreadData *)dat;
    std::unordered_set<std::pair<int, int>, pair_hash> checkedChunks;
    for (uint64_t count = data->index; count < SEEDS_TO_CHECK; count += NUMBER_OF_THREADS) {
        uint64_t seed = START_SEED + count;
        checkedChunks.clear();
        int currentCount = testForSlimeAt(seed, 0, 0, &checkedChunks, 0);
        if (currentCount >= bestCount) {
            printf("%" PRIu64 "\t%d\n", seed, currentCount);
            if (currentCount > bestCount) bestCount = currentCount;
        }
    }
    return NULL;
}

int main() {
    pthread_t threads[NUMBER_OF_THREADS];
    ThreadData data[NUMBER_OF_THREADS];
    bestCount = 2;
    for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        data[i].index = i;
        pthread_create(&threads[i], NULL, checkSeed, &data[i]);
    }
    for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) pthread_join(threads[i], NULL);
    return 0;
}