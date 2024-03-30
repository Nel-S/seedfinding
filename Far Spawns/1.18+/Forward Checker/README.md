# Forward Checker
This is a Minecraft seedfinding program for finding spawns beyond a given distance from the world origin. It returns the exact spawnpoints for Bedrock 1.18+, and the spawnpoints within a 11x11-chunk bounding box for Java 1.18+. (This is not compatible with Java 1.17-.)

See [the repository's parent README](../../../README.md) for compilation instructions.

## Using
The program support two search modes:
- one based on underlying first-round spawn rings (which broadly determine the rough spawn location and distance from the origin to within ~512 blocks on average); and
- one using specified distances from the origin (that models the second-round spawn ring calculations to narrow the spawnpoint down to at most 96sqrt(2) blocks, ensuring more precise results at the cost of speed). This search mode can also search based on distances from the origin along a single axis.
<!-- The precompiled version also supports a mode that monotonically increases the radius based on each best result found. This cannot be added to the static version due to it treating the origin distance thresholds as constants and optimizing using that. -->

It should be emphasized that with the second search mode type, **this program does not calculate the exact spawnpoint of Java Edition worlds**: doing that would require a block-by-block recreation of that part of the world. Instead, this program's results may be up to 96sqrt(2) blocks off at most, or 96 blocks off at most in a single direction. The benefit of this approach is that
- the programs runs far quicker, and
- the programs are also version- and platform-independent, in that terrain differences between Java 1.18+ versions, or between Java and Bedrock, will not affect the program's results.
The caveat, however, is that for Java, one must treat every world returning a distance within 96sqrt(2) blocks of the farthest distance found (or 96 blocks of the farthest distance along one axis if one uses that metric) as a potential candidate. This will still usually eliminate all but a handful of worlds, however.<!--, and on my machine I have found this program to run on average 518-692x faster than an equivalent default spawn search using [Cubiomes Viewer](https://github.com/Cubitect/cubiomes-viewer). -->

## Acknowledgements
This program relies on my Utilities library, which in turn relies on the [Cubiomes](https://github.com/Cubitect/cubiomes) library; that was created by [Cubitect](https://github.com/Cubitect) and released under the MIT License.