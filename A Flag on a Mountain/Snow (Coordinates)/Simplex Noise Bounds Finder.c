#include "utilities/cubiomes/noise.h"
#include <stdio.h>

const int RADIUS = 10000;

int main() {
    uint64_t seed;
    setSeed(&seed, 1234);
    PerlinNoise temperature;
    perlinInit(&temperature, &seed);

    double max = -INFINITY, min = INFINITY;
    int x = 0, z = 0, i = 0, j = -1;
    for (uint64_t count = 0; count < (2 * RADIUS + 1) * (2 * RADIUS + 1); ++count) {
        double current = sampleSimplex2D(&temperature, x/8., z/8.);
        if (current > max) {
            printf("Max: %.18g\t(%d\t%d)\n", current, x, z);
            max = current;
        }
        if (current < min) {
            printf("Min: %.18g\t(%d\t%d)\n", current, x, z);
            min = current;
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