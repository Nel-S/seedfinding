# 1.13-1.17

Between 1.13 and 1.17, Minecraft: Java Edition still searches for a valid biome in the area ((-256, -256), (256, 256)), then subsequently looks for a grass block. However, the game now performs a spiral iteration up to 16 chunks away instead of jumping using nextInts, which significantly shrinks the area spawnpoints can fall within.

The farthest theoretically-possible spawnpoint in this version is (-512, -512) at 724.08 blocks away from (0,0). (Cubiomes currently suggests it is (527, 527), but I suspect this is a bug.)<br>
No attempts have been made by me to find the farthest spawnpoints in these versions yet.

## Contributors
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)