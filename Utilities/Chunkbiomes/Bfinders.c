#include "Bfinders.h"
#include <stdbool.h>
#include <string.h>

int getBedrockStructureConfig(const int structureType, const int mc, StructureConfig *sconf) {
    static const StructureConfig
    s_desert_pyramid  = { 14357617, 32, 24, Desert_Pyramid,  0,0},
    s_igloo           = { 14357617, 32, 24, Igloo,           0,0},
    s_jungle_pyramid  = { 14357617, 32, 24, Jungle_Pyramid,   0,0},
    s_mansion         = { 10387319, 80, 60, Mansion,         STRUCT_TRIANGULAR,0},
    s_monument        = { 10387313, 32, 27, Monument,        STRUCT_TRIANGULAR,0},
    s_outpost         = {165745296, 80, 56, Outpost,         STRUCT_TRIANGULAR,0},
    s_ruined_portal   = { 40552231, 40, 25, Ruined_Portal,   0,0},
    s_shipwreck_117   = {165745295, 10,  5, Shipwreck,       STRUCT_TRIANGULAR,0},
    s_shipwreck       = {165745295, 24, 20, Shipwreck,       0,0},
    s_swamp_hut       = { 14357617, 32, 24, Swamp_Hut,       0,0},
    s_village_117     = { 10387312, 27, 17, Village,         STRUCT_TRIANGULAR,0},
    s_village         = { 10387312, 34, 26, Village,         STRUCT_TRIANGULAR,0},
    // Nether structures
    s_bastion         = { 30084232, 30, 26, Bastion,         STRUCT_NETHER,0},
    s_fortress        = { 30084232, 30, 26, Fortress,        STRUCT_NETHER,0},
    s_ruined_portal_n = { 40552231, 25, 15, Ruined_Portal_N, STRUCT_NETHER,0},
    // End structures
    s_end_city        = { 10387313, 20,  9, End_City,        STRUCT_END|STRUCT_TRIANGULAR,0}
    // s_unknown         = { 16842397,  4,  2, Feature,         STRUCT_TRIANGULAR,0} // Not sure which structure/feature this corresponds to
    ;

    // Chunkbase only goes back to Bedrock 1.14
    switch (structureType) {
    case Desert_Pyramid:
        *sconf = s_desert_pyramid;
        return mc >= MC_1_14;
    case Igloo:
        *sconf = s_igloo;
        return mc >= MC_1_14;
    case Jungle_Pyramid: // -
        *sconf = s_jungle_pyramid;
        return mc >= MC_1_14;
    case Mansion: // -
        *sconf = s_mansion;
        return mc >= MC_1_14;
    case Monument: // -
        *sconf = s_monument;
        return mc >= MC_1_14;
    case Outpost:
        *sconf = s_outpost;
        return mc >= MC_1_14;
    case Ruined_Portal: // -
        *sconf = s_ruined_portal;
        return mc >= MC_1_14;
    case Shipwreck: // -
        *sconf = mc <= MC_1_17 ? s_shipwreck_117 : s_shipwreck;
        return mc >= MC_1_14;
    case Swamp_Hut: // -
        *sconf = s_swamp_hut;
        return mc >= MC_1_14;
    case Village: // -
        *sconf = mc <= MC_1_17 ? s_village_117 : s_village;
        return mc >= MC_1_14;
    case Bastion:
        *sconf = s_bastion;
        return mc >= MC_1_14;
    case Fortress:
        *sconf = s_fortress;
        return mc >= MC_1_14;
    case Ruined_Portal_N:
        *sconf = s_ruined_portal_n;
        return mc >= MC_1_14;
    case End_City:
        *sconf = s_end_city;
        return mc >= MC_1_14;
    // case Feature:
    //     *sconf = s_unknown;
    //     return mc >= MC_1_14;
    default:
        memset(sconf, 0, sizeof(StructureConfig));
        return 0;
    }
}


static inline ATTR(const)
Pos mGetFeatureChunkInRegion(const StructureConfig config, uint64_t seed, int regX, int regZ) {
    MersenneTwister mt;
    mSetSeed(&mt, regX*341873128712 + regZ*132897987541 + seed + config.salt);
    Pos pos;
    if (config.properties & STRUCT_TRIANGULAR) {
        pos.x = (mNextInt(&mt, config.chunkRange) + mNextInt(&mt, config.chunkRange))/2;
        pos.z = (mNextInt(&mt, config.chunkRange) + mNextInt(&mt, config.chunkRange))/2;
    } else {
        pos.x = mNextInt(&mt, config.chunkRange);
        pos.z = mNextInt(&mt, config.chunkRange);
    }
    return pos;
}

static inline Pos getBedrockFeaturePos(StructureConfig config, uint64_t seed, int regX, int regZ) {
    Pos pos = mGetFeatureChunkInRegion(config, seed, regX, regZ);
    // Bedrock features are offset by +8.
    pos.x = (((uint64_t)regX*config.regionSize + pos.x) << 4) + 8;
    pos.z = (((uint64_t)regZ*config.regionSize + pos.z) << 4) + 8;
    return pos;
}

int getBedrockStructurePos(int structureType, int mc, uint64_t seed, int regX, int regZ, Pos *pos) {
    StructureConfig sconf;
    if (!getBedrockStructureConfig(structureType, mc, &sconf)) return 0;

    // I suspect every Bedrock structure uses the same 
    // switch (structureType) {
    // case Desert_Pyramid:
    // case Igloo:
    // case Jungle_Pyramid:
    // case Mansion:
    // case Monument:
    // case Ruined_Portal:
    // case Shipwreck:
    // case Swamp_Hut:
    // case Village:
    // case Bastion:
    // case Fortress:
    // case 
        *pos = getBedrockFeaturePos(sconf, seed, regX, regZ);
        return 1;

    // default:
    //     fprintf(stderr, "ERR getStructurePos: unsupported structure type %d\n", structureType);
    //     exit(1);
    // }
    // return 0;
}

//TODO: Check if x/z are blocks of chunks
int getRavinePos(int mc, uint64_t seed, int x, int z, const Generator *g, StructureVariant *ravine1, StructureVariant *ravine2) {
    uint64_t random;
    if (mc < MC_1_18) {
        int count = 0;
        for (int i = 0; i < 2; ++i) {
            setSeed(&random, seed + 1 - i);
            setSeed(&random, (x * nextLong(&random)) ^ (z * nextLong(&random)) ^ (seed + 1 - i));
            if (nextFloat(&random) < 0.02 && (!count || (g && isOceanic(getBiomeAt(g, 1, 16*x, 0, 16*z))))) {
                StructureVariant *ravinePointer = count ? ravine2 : ravine1;
                if (ravinePointer) {
                    ravinePointer->x = 16 * x + nextInt(&random, 16);
                    int temp = nextInt(&random, 40);
                    ravinePointer->y = 20     + nextInt(&random, temp + 8);
                    ravinePointer->z = 16 * z + nextInt(&random, 16);
                    nextFloat(&random); nextFloat(&random);
                    ravinePointer->size = 4*nextFloat(&random) + 2*nextFloat(&random); //TODO: Check if float->uint conversion is intended
                }
                ++count;
            }
        }
        return count;
    } else {
        setSeed(&random, seed + 2);
        setSeed(&random, (x * nextLong(&random)) ^ (z * nextLong(&random)) ^ (seed + 2));
        if (nextFloat(&random) < 0.01) {
            if (ravine1) {
                ravine1->x = 16 * x + nextInt(&random, 16);
                ravine1->y = 10     + nextInt(&random, 58);
                ravine1->z = 16 * z + nextInt(&random, 16);
                nextFloat(&random); nextFloat(&random);
                ravine1->size = 4*nextFloat(&random) + 2*nextFloat(&random); //TODO: Check if float->uint conversion is intended
            }
            return 1;
        }
        return 0;
    }
}

int getBedrockRavinePos(uint64_t seed, int x, int z, StructureVariant *ravine) {
    MersenneTwister mt;
    mSetSeed(&mt, seed);
    uint32_t call1 = mNextIntUnbound(&mt);
    mSetSeed(&mt, (seed ^ x*(call1 | 1)) + z*(mNextIntUnbound(&mt) | 1));
    if (!mNextInt(&mt, 150)) {
        if (ravine) {
            ravine->x = 16*x + mNextInt(&mt, 16);
            ravine->y = 20   + mNextInt(&mt, mNextInt(&mt, 40) + 8);
            mNextIntUnbound(&mt);
            ravine->z = 16*z + mNextInt(&mt, 16);
            mNextFloat(&mt); mNextFloat(&mt);
            ravine->size = 3*(mNextFloat(&mt) + mNextFloat(&mt));
            if (mNextFloat(&mt) < .05) {
                ravine->giant = 1;
                ravine->size *= 2;
            }
        }
        return 1;
    }
    return 0;
}

// Unfinished
// int getBedrockStronghold(uint64_t seed) {
//     static const double PI = 3.1415926535897932384626433;
//     MersenneTwister mt;
//     mSetSeed(&mt, seed);
//     double i = 2*PI*mNextFloat(&mt);
//     int j = 40 + mNextInt(&mt, 16);
//     Pos f[3];
//     for (int k = 0; k < 3; ++k) {

//         bool found = false;
//         int l = floor(j*cos(i));
//         int m = floor(j*sin(i));
//         int o = l - 8, p = m - 8;
//         if (found) {
//             i += 3*PI/5;
//             j += 8;
//         } else {
//             i += PI/4;
//             j += 4;
//         }
//     }
// }