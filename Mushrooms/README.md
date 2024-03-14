# Mushrooms

These are files relating to finding a normal biomes mushroom-island spawn in Java 1.18+.

The spawn algorithm used in those versions heavily discourages coordinates with extremely low continentalnesses (which is the defining feature of mushroom islands), so having the algorithm directly choose a coordinate on a mushroom island is *de facto* impossible. However, since the last stage of the spawn algorithm can move spawnpoints from water onto land at most five chunks away, achieving a second-stage spawnpoint within five chunks of a mushroom island, with no other land blocks in that radius, will theoretically result in a mushroom island spawn just the same.

The current closest candidate found is 110234913140 with a mushroom island within 105 Chebyshev blocks of the chunk-centered second-stage spawnpoint.<br>
All seeds between 0 and 1245700495016 have been checked, though due to possible bugs only those between 0 and 146188867240 have been confirmed.

## Contributors
- Cubitect - implemented (ports of) the spawn, climates, and biome-mapping algorithms in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created C programs, checked 146188867240 seeds
- Colin - ran Forward Checker on computing cluster, checked 1099511627776 seeds