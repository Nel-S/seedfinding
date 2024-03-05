#include "../cubiomes/rng.h"
#include <math.h>
#include <stdio.h>

// 0: Dirt, 1: Coal
const uint8_t ORE = 1;
const double FLOAT_INCREMENT = 0.05;
const double I_DOUBLE_INCREMENT = 0.05;

typedef struct Coord {
    int x, y, z;
} Coord;

const uint8_t ORE_SIZE = (!ORE) ? 32 : 16;
const double PI = 3.1415926535897932384626433;

const Coord ORE_MAP[] = {{}};

int main() {
    for (float f = 0; f < 1; f += FLOAT_INCREMENT) {
        for (int i = 0; i <= ORE_SIZE; ++i) {
            double d = sin(f*PI)*(ORE_SIZE - 2*i)/8.0;
            double d3 = cos(f*PI)*(ORE_SIZE - 2*i)/8.0;
            double d4Base = (sin(i*PI/ORE_SIZE) + 1)*ORE_SIZE/32.0;
            for (double di = 0; di < 1; di += I_DOUBLE_INCREMENT) {
                double d4 = d4Base*di + 0.5;
                for (int j = floor(d - d4); j <= floor(d + d4); ++j) {

                }
            }
        }
    }
    return 0;
}