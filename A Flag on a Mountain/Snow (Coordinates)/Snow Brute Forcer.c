#include "cubiomes/noise.h"
#include <stdio.h>

typedef struct {
    int z, x, y;
    _Bool snowPresent;
} checkData;

typedef struct {
    int x, z;
    double data;
} CoordWithData;

const checkData CHECKS[] = {{ 15, -63,  5, 0}, {-3 ,  20,  5, 0}, // No snow at level minSnowtopY + 5
                            {-2 ,  20,  4, 0}, // No snow at level minSnowtopY + 4 
                            { 0 ,  0 ,  0, 1}, // Snow at level minSnowtopY + 0
                            {-1 ,  20,  3, 0}, {-9 , -4 ,  3, 0}, {-10, -4 ,  3, 0}, {-15, -7 ,  3, 0}, {-15, -11,  3, 0}, {-16, -12,  3, 0}, // No snow at level minSnowtopY + 3
                            { 11, -41,  1, 1}, { 11, -42,  1, 1}, { 10, -49,  1, 1}, { 10, -50,  1, 1}, { 10, -51,  1, 1}, {-14, -4 ,  1, 1}, // Snow at level minSnowtopY + 1
                            {-3 , -4 ,  2, 0}, {-8 , -4 ,  2, 0}, {-13, -20,  2, 0}, {-16, -10,  2, 0}, {-16, -11,  2, 0}, {-17, -12,  2, 0}, // No snow at level minSnowtopY + 2
                            { 11, -48,  2, 1}, { 11, -49,  2, 1}, { 11, -50,  2, 1}, { 11, -51,  2, 1}, { 11, -52,  2, 1}, {-2 , -1 ,  2, 1}, {-2 , -2 ,  2, 1}, {-13, -3 ,  2, 1}, // Snow at level minSnowtopY + 2
                            {-3 , -3 ,  1, 0}, {-9 , -3 ,  1, 0}, {-10, -3 ,  1, 0}, {-11, -3 ,  1, 0}, {-12, -3 ,  1, 0}, {-15, -14,  1, 0}, {-16, -8 ,  1, 0}, {-16, -9 ,  1, 0}, // No snow at level minSnowtopY + 1
                            {-2 , -3 ,  3, 1}, {-14, -5 ,  3, 1}, // Snow at level minSnowtopY + 3
                            {-2 ,  0 ,  0, 0}, {-3 , -2 ,  0, 0}, {-4 , -4 ,  0, 0}, {-8 , -3 ,  0, 0}, {-14, -18,  0, 0}, {-15, -6 ,  0, 0}, {-15, -15,  0, 0}, {-17, -11,  0, 0}, // No snow at level minSnowtopY + 0
                            {-2 , -4 ,  4, 1}, {-11, -4 ,  4, 1}, {-12, -20,  4, 1}, {-13, -12,  4, 1}, {-14, -13,  4, 1}, {-14, -14,  4, 1}, {-15, -10,  4, 1}, // Snow at level minSnowtopY + 4
                            {-3 , -1 , -1, 0}, {-4 , -3 , -1, 0}, {-6 , -3 , -1, 0}, {-8 , -2 , -1, 0}, {-9 , -2 , -1, 0}, {-10, -2 , -1, 0}, {-11, -2 , -1, 0}, {-17, -10, -1, 0}, // No snow at level minSnowtopY - 1
                            {-1 , -2 ,  5, 1}, {-1 , -3 ,  5, 1}, {-1 , -4 ,  5, 1}, {-11, -14,  5, 1}, {-12, -4 ,  5, 1}, {-13, -11,  5, 1}, {-13, -13,  5, 1}, {-13, -19,  5, 1}, {-14, -10,  5, 1}, {-14, -15,  5, 1}, {-15, -9 ,  5, 1}, // Snow at level minSnowtopY + 5
                            {-1 ,  0 ,  6, 1}, {-1 , -1 ,  6, 1}, {-4 ,  20,  6, 1}, {-7 , -15,  6, 1}, {-11, -20,  6, 1}, {-12, -14,  6, 1}, {-12, -19,  6, 1}, {-14, -6 ,  6, 1}, {-14, -16,  6, 1}, {-15, -8 ,  6, 1}, // Snow at level minSnowtopY + 6
};

const uint64_t SQUARE_RADIUS = 10000;
const int INITIAL_BEST = sizeof(CHECKS)/sizeof(*CHECKS);

int minSnowtopY; // 91 - 92
CoordWithData bestCount;
PerlinNoise temperature;

_Bool checkSnow(int x, int y, int z, _Bool snowPresent) {
    return (ceil(94 - 4*sampleSimplex2D(&temperature, x/8., z/8.)) <= y) == snowPresent;
}

int checkCoord(int x, int y, int z) {
    int count = 0;
    for (int i = 0; i < sizeof(CHECKS)/sizeof(*CHECKS) && count + sizeof(CHECKS)/sizeof(*CHECKS) - i >= bestCount.data; ++i) {
        if (checkSnow(x + CHECKS[i].x, y + CHECKS[i].y, z + CHECKS[i].z, CHECKS[i].snowPresent)) ++count;
    }
    return count;
}

int main() {
    uint64_t seed;
    setSeed(&seed, 1234);
    perlinInit(&temperature, &seed);

    bestCount.data = INITIAL_BEST;
    
    int x = 0, z = 0, i = 0, j = -1;
    for (uint64_t count = 0; count < (2 * SQUARE_RADIUS + 1) * (2 * SQUARE_RADIUS + 1); ++count) {
        // int zMod = z & 0xf;
        // if (zMod == 5 || zMod == 6 || zMod == 8) {
        if (1) {
            for (minSnowtopY = 91; minSnowtopY <= 92; ++minSnowtopY) {
                int currentCount = checkCoord(x, minSnowtopY, z);
                if (currentCount >= bestCount.data) {
                    printf("%d/%zd\t(%d\t%d  %d)", currentCount, sizeof(CHECKS)/sizeof(*CHECKS), x, minSnowtopY, z);
                    printf("\t-> (%d\t%d  %d)", x + 10, minSnowtopY + 2, z - 50);
                    printf("\t(%d %d)\n", (x + 10) & 0xf, (z - 50) & 0xf);
                    if (currentCount > bestCount.data) {
                        bestCount.x = x;
                        bestCount.z = z;
                        bestCount.data = currentCount;
                    }
                }
            }
        }

        if (x == z || (x < 0 && x == -z) || (x > 0 && x == 1 - z)) {
            int tmp = i;
            i = -j;
            j = tmp;
        }
        x += i;
        z += j;
    }
    return 0;
}