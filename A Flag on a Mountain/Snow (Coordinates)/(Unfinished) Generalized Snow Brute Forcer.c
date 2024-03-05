#include "../../cubiomes/noise.h"
#include <pthread.h>

typedef struct Coord {
    int x, z;
} Coord;

typedef struct CheckData {
    int x, y, z;
    _Bool snowPresent;
} CheckData;

typedef struct ThreadData {
    
} ThreadData;

enum Biomes {Cold_Taiga = -10, Cold_Taiga_Hills = Cold_Taiga,
             Frozen_Ocean = 0, Frozen_River = Frozen_Ocean, Ice_Mountains = Frozen_Ocean, Ice_Plains = Frozen_Ocean,
             Cold_Beach = 1,
             Extreme_Hills = 4, Extreme_Hills_Edge = Extreme_Hills, Extreme_Hills_Plus = Extreme_Hills, Stone_Beach = Extreme_Hills,
             Taiga = 5, Taiga_Hills = Taiga,
             Mega_Taiga = 6, Mega_Taiga_Hills = Mega_Taiga,
             Birch_Forest = 10, Birch_Forest_Hills = Birch_Forest, Deep_Ocean = Birch_Forest, Forest = Birch_Forest, Forest_Hills = Birch_Forest, Mesa = Birch_Forest, Mesa_Plateau = Birch_Forest, Mesa_Plateau_F = Birch_Forest, Ocean = Birch_Forest, Plains = Birch_Forest, River = Birch_Forest, Roofed_Forest = Birch_Forest, The_End = Birch_Forest,
             Beach = 18, Swampland = Beach,
             Jungle = 19, Jungle_Edge = Jungle, Jungle_Hills = Jungle,
             Savanna_Plateau = 20,
             Savanna = 24,
             Desert = 40, Desert_Hills = Desert, The_Nether = Desert};
enum {noSnow, snow};

const int BIOME = Extreme_Hills;
const uint64_t SQUARE_RADIUS = 10000;
const Coord CENTER = {0, 0};
const uint8_t NUMBER_OF_THREADS = 4;
CheckData CHECKS[] = {{-63,  5,  15, noSnow}, { 20,  5, -3 , noSnow}, // No snow at level minSnowtopY + 5
                      { 20,  4, -2 , noSnow}, // No snow at level minSnowtopY + 4 
                      { 0 ,  0,  0 , snow  }, // Snow at level minSnowtopY + 0
                      { 20,  3, -1 , noSnow}, {-4 ,  3, -9 , noSnow}, {-4 ,  3, -10, noSnow}, {-7 ,  3, -15, noSnow}, {-11,  3, -15, noSnow}, {-12,  3, -16, noSnow}, // No snow at level minSnowtopY + 3
                      {-41,  1,  11, snow  }, {-42,  1,  11, snow  }, {-49,  1,  10, snow  }, {-50,  1,  10, snow  }, {-51,  1,  10, snow  }, {-4 ,  1, -14, snow  }, // Snow at level minSnowtopY + 1
                      {-4 ,  2, -3 , noSnow}, {-4 ,  2, -8 , noSnow}, {-20,  2, -13, noSnow}, {-10,  2, -16, noSnow}, {-11,  2, -16, noSnow}, {-12,  2, -17, noSnow}, // No snow at level minSnowtopY + 2
                      {-48,  2,  11, snow  }, {-49,  2,  11, snow  }, {-50,  2,  11, snow  }, {-51,  2,  11, snow  }, {-52,  2,  11, snow  }, {-1 ,  2, -2 , snow  }, {-2 ,  2, -2 , snow  }, {-3 ,  2, -13, snow  }, // Snow at level minSnowtopY + 2
                      {-3 ,  1, -3 , noSnow}, {-3 ,  1, -9 , noSnow}, {-3 ,  1, -10, noSnow}, {-3 ,  1, -11, noSnow}, {-3 ,  1, -12, noSnow}, {-14,  1, -15, noSnow}, {-8 ,  1, -16, noSnow}, {-9 ,  1, -16, noSnow}, // No snow at level minSnowtopY + 1
                      {-3 ,  3, -2 , snow  }, {-5 ,  3, -14, snow  }, // Snow at level minSnowtopY + 3
                      { 0 ,  0, -2 , noSnow}, {-2 ,  0, -3 , noSnow}, {-4 ,  0, -4 , noSnow}, {-3 ,  0, -8 , noSnow}, {-18,  0, -14, noSnow}, {-6 ,  0, -15, noSnow}, {-15,  0, -15, noSnow}, {-11,  0, -17, noSnow}, // No snow at level minSnowtopY + 0
                      {-4 ,  4, -2 , snow  }, {-4 ,  4, -11, snow  }, {-20,  4, -12, snow  }, {-12,  4, -13, snow  }, {-13,  4, -14, snow  }, {-14,  4, -14, snow  }, {-10,  4, -15, snow  }, // Snow at level minSnowtopY + 4
                      {-1 , -1, -3 , noSnow}, {-3 , -1, -4 , noSnow}, {-3 , -1, -6 , noSnow}, {-2 , -1, -8 , noSnow}, {-2 , -1, -9 , noSnow}, {-2 , -1, -10, noSnow}, {-2 , -1, -11, noSnow}, {-10, -1, -17, noSnow}, // No snow at level minSnowtopY - 1
                      {-2 ,  5, -1 , snow  }, {-3 ,  5, -1 , snow  }, {-4 ,  5, -1 , snow  }, {-14,  5, -11, snow  }, {-4 ,  5, -12, snow  }, {-11,  5, -13, snow  }, {-13,  5, -13, snow  }, {-19,  5, -13, snow  }, {-10,  5, -14, snow  }, {-15,  5, -14, snow  }, {-9 ,  5, -15, snow  }, // Snow at level minSnowtopY + 5
                      { 0 ,  6, -1 , snow  }, {-1 ,  6, -1 , snow  }, { 20,  6, -4 , snow  }, {-15,  6, -7 , snow  }, {-20,  6, -11, snow  }, {-14,  6, -12, snow  }, {-19,  6, -12, snow  }, {-6 ,  6, -14, snow  }, {-16,  6, -14, snow  }, {-8 ,  6, -15, snow  }, // Snow at level minSnowtopY + 6
};

const double SQRT_3 = 1.7320508075688772;
const double MAX_PERLIN_AMPLITUDE = 140*SQRT_3/243;
const int MIN_Y_SNOW_CAN_GENERATE  = ceil(30*BIOME - 26 - 4*MAX_PERLIN_AMPLITUDE);
const int MAX_Y_SNOW_CANT_GENERATE = ceil(30*BIOME - 26 + 4*MAX_PERLIN_AMPLITUDE) - 1;
int minY, maxY;
size_t bestCount;
PerlinNoise temperature;

_Bool checkSnow(int x, int y, int z, _Bool snowPresent) {
    return (30*BIOME - 26 - 4*sampleSimplex2D(&temperature, x/8., z/8.) <= y) == snowPresent;
}

size_t checkCoord(int x, int y, int z) {
    size_t count = 0;
    for (size_t i = 0; i < sizeof(CHECKS)/sizeof(*CHECKS) && count + sizeof(CHECKS)/sizeof(*CHECKS) - i >= bestCount; ++i) {
        if (checkSnow(x + CHECKS[i].x, y + CHECKS[i].y, z + CHECKS[i].z, CHECKS[i].snowPresent)) ++count;
    }
    return count;
}

void *thread(void *dat) {
    ThreadData *data = dat;
    int minSnowtopY;

    int x = 0, z = 0, i = 0, j = -1;
    for (uint64_t count = 0; count < SQUARE_RADIUS * (SQUARE_RADIUS + 1); ++count) {
        for (minSnowtopY = minY; minSnowtopY <= maxY; ++minSnowtopY) {
            size_t currentCount = checkCoord(x, minSnowtopY, z);
            if (currentCount >= bestCount) {
                printf("%d/%zd\t(%d\t%d  %d)", currentCount, sizeof(CHECKS)/sizeof(*CHECKS), x, minSnowtopY, z);
                if (currentCount > bestCount) bestCount = currentCount;
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
    return NULL;
}

int main() {
    uint64_t seed;
    setSeed(&seed, 1234);
    perlinInit(&temperature, &seed);

    /* TODO: Reorder CHECKS from highest y-level without snow/lowest y-level with snow to lowest y-level without snow/highest y-level with snow.
       Also maybe discard samples that would fall out of the range regardless, and/or warn if range is impossibly wide.*/
    
    // TODO: Calculate from checks the true range of possible y-values
    minY = MIN_Y_SNOW_CAN_GENERATE;
    maxY = MAX_Y_SNOW_CANT_GENERATE;

    bestCount = 0;
    for (int minSnowtopY = minY; minSnowtopY <= maxY; ++minSnowtopY) {
        int currentCount = checkCoord(CENTER.x, minSnowtopY, CENTER.z);
        if (currentCount >= bestCount) {
            printf("%d/%llu\t(%d\t%d  %d)", currentCount, sizeof(CHECKS)/sizeof(*CHECKS), CENTER.x, minSnowtopY, CENTER.z);
            if (currentCount > bestCount) bestCount = currentCount;
        }
    }
    
    pthread_t threads[NUMBER_OF_THREADS];
    ThreadData data[NUMBER_OF_THREADS];
    for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        // Initialize data[i]
        pthread_create(&threads[i], NULL, thread, &data[i]);
    }
    for (uint8_t i = 0; i < NUMBER_OF_THREADS; ++i) pthread_join(threads[i], NULL);
    return 0;
}