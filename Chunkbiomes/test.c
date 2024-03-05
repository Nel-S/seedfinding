#include "Bfinders.c"
#include <stdio.h>

int main() {
    const uint64_t SEED = 1221211212212112;
    const int OVERWORLD_STRUCTURES[] = {Desert_Pyramid, Igloo, Jungle_Pyramid, Monument, Swamp_Hut},
                 NETHER_STRUCTURES[] = {Bastion, Fortress, Ruined_Portal_N},
                    END_STRUCTURES[] = {End_City};
    const int RADIUS = 3;

    Generator g;
    setupGenerator(&g, MC_NEWEST, 0);
    applySeed(&g, DIM_OVERWORLD, SEED);
    // g.platform = PLATFORM_JAVA;

    StructureConfig sconf;
    Pos pos;

    for (int chunkX = -RADIUS; chunkX <= RADIUS; ++chunkX) {
        for (int chunkZ = -RADIUS; chunkZ <= RADIUS; ++chunkZ) {
            for (size_t i = 0; i < sizeof(OVERWORLD_STRUCTURES)/sizeof(*OVERWORLD_STRUCTURES); ++i) {
                if (!getBedrockStructureConfig(OVERWORLD_STRUCTURES[i], g.mc, &sconf)) {
                    printf("ERR: Structure %d's configuration could not be found.\n", OVERWORLD_STRUCTURES[i]);
                    continue;
                }
                if (!getBedrockStructurePos(OVERWORLD_STRUCTURES[i], g.mc, g.seed, chunkX, chunkZ, &pos)) {
                    printf("ERR: Position for structure %d in region (%d, %d) ((%d, %d) - (%d, %d)) could not be determined.\n", OVERWORLD_STRUCTURES[i], chunkX, chunkZ, chunkX * sconf.regionSize * 16, chunkZ * sconf.regionSize * 16, (chunkX + 1) * sconf.regionSize * 16 - 1, (chunkZ + 1) * sconf.regionSize * 16 - 1);
                    continue;
                }
                // TODO: Investigate Chunkbase's isViableStructureTerrain for mansions
                if (OVERWORLD_STRUCTURES[i] && (!isViableStructurePos(OVERWORLD_STRUCTURES[i], &g, pos.x, pos.z, 0) || !isViableStructureTerrain(OVERWORLD_STRUCTURES[i], &g, pos.x, pos.z))) continue;
                printf("%d: (%d, %d)\n", OVERWORLD_STRUCTURES[i], pos.x, pos.z);
            }
        }
    }

    applySeed(&g, DIM_NETHER, SEED);
    for (int chunkX = -RADIUS; chunkX <= RADIUS; ++chunkX) {
        for (int chunkZ = -RADIUS; chunkZ <= RADIUS; ++chunkZ) {
            for (size_t i = 0; i < sizeof(NETHER_STRUCTURES)/sizeof(*NETHER_STRUCTURES); ++i) {
                if (!getBedrockStructureConfig(NETHER_STRUCTURES[i], g.mc, &sconf)) {
                    printf("ERR: Structure %d's configuration could not be found.\n", NETHER_STRUCTURES[i]);
                    continue;
                }
                if (!getBedrockStructurePos(NETHER_STRUCTURES[i], g.mc, g.seed, chunkX, chunkZ, &pos)) {
                    printf("ERR: Position for structure %d in region (%d, %d) ((%d, %d) - (%d, %d)) could not be determined.\n", NETHER_STRUCTURES[i], chunkX, chunkZ, chunkX * sconf.regionSize * 16, chunkZ * sconf.regionSize * 16, (chunkX + 1) * sconf.regionSize * 16 - 1, (chunkZ + 1) * sconf.regionSize * 16 - 1);
                    continue;
                }
                if (!isViableStructurePos(NETHER_STRUCTURES[i], &g, pos.x, pos.z, 0)) continue;
                printf("%d: (%d, %d)\n", NETHER_STRUCTURES[i], pos.x, pos.z);
            }
        }
    }

    applySeed(&g, DIM_END, SEED);
    for (int chunkX = -RADIUS; chunkX <= RADIUS; ++chunkX) {
        for (int chunkZ = -RADIUS; chunkZ <= RADIUS; ++chunkZ) {
            for (size_t i = 0; i < sizeof(END_STRUCTURES)/sizeof(*END_STRUCTURES); ++i) {
                if (!getBedrockStructureConfig(END_STRUCTURES[i], g.mc, &sconf)) {
                    printf("ERR: Structure %d's configuration could not be found.\n", END_STRUCTURES[i]);
                    continue;
                }
                if (!getBedrockStructurePos(END_STRUCTURES[i], g.mc, g.seed, chunkX, chunkZ, &pos)) {
                    printf("ERR: Position for structure %d in region (%d, %d) ((%d, %d) - (%d, %d)) could not be determined.\n", END_STRUCTURES[i], chunkX, chunkZ, chunkX * sconf.regionSize * 16, chunkZ * sconf.regionSize * 16, (chunkX + 1) * sconf.regionSize * 16 - 1, (chunkZ + 1) * sconf.regionSize * 16 - 1);
                    continue;
                }
                if (!isViableStructurePos(END_STRUCTURES[i], &g, pos.x, pos.z, 0)) continue;
                printf("%d: (%d, %d)\n", END_STRUCTURES[i], pos.x, pos.z);
            }
        }
    }

    return 0;
}