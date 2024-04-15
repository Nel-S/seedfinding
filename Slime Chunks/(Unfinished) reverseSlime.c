#include "common.h"

const int GLOBAL_NUMBER_OF_WORKERS = 4;
const bool TIME_PROGRAM = false;
const char *INPUT_FILEPATH = NULL;
const char *OUTPUT_FILEPATH = NULL;

localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;


bool testForSlime(uint64_t seed, int chunkX, int chunkZ) {
    uint64_t rng = ((seed + (int)(chunkX * (5947611 + chunkX * 4987142) + chunkZ * 389711) + (int)(chunkZ * chunkZ) * 4392871ULL) ^ 25303508018) & 0xffffffffffff;
    rng = (rng * 25214903917 + 11) & 0xffffffffffff;
    if ((rng >> 17) & 0x7fffffff >= 2147483640) rng = (rng * 25214903917 + 11) & 0xffffffffffff;
    return (rng >> 17) % 10;
}

uint64_t reverseSlime() {
    for (uint64_t upperBits = 0; upperBits < 2147483640; upperBits += 10) {
        for (uint64_t lowerBits = 0; lowerBits < 1ULL << 17; ++lowerBits) {
            uint64_t fullState = (upperBits << 17) + lowerBits;
            // Step state back one advancement.
            fullState = 246154705703781 * fullState + 107048004364969;
            // Mark whether the original state could have been the result of a double advancement
            _Bool possibleDoubleAdvancement = (fullState >> 17) >= 2147483640;
            uint64_t xoredState = fullState ^ 25303508018;
        }
    }
}