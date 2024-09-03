# 1.13-1.17

Between 1.13 and 1.17, Minecraft: Java Edition still searches for a valid biome in the area ((-256, -256), (256, 256)), then subsequently looks for a grass block. However, the game now performs a spiral iteration up to 16 chunks away (offset by one chunk to the Southeast) instead of jumping using nextInts, which significantly shrinks the range spawnpoints can fall within.

| Criteria                               | <ins>Theoretical</ins> Farthest Spawnpoint from (0, 0) | Distance (blocks) |
| -------------------------------------- | ------------------------------------------------------ | ----------------- |
| L<sup>2</sup>/Euclidean distance       | (527, 527)                                             | 745.29            |
| L<sup>&infin;</sup>/Chebyshev distance | x/z = 527                                              | 527               |
| L<sup>-&infin;</sup> distance          | x/z = 527                                              | 527               |

The only way to achieve such a spawnpoint is for a suitable biome to be at (256, 256); then for the initial biome search to pick exactly that coordinate; then for the spiral iteration to not find any sky-exposed grass blocks until (527, 527).

| Criteria                                              | Current Farthest-known Spawnpoint from (0, 0) | Distance (blocks) | Seed      | Discoverer |
| ----------------------------------------------------- | --------------------------------------------- | ----------------- | --------- | ---------- |
| Normal biomes, L<sup>2</sup>/Euclidean distance       | (257, 447)                                    | 515.61            | 40593770  | NelS       |
| Normal biomes, L<sup>&infin;</sup>/Chebyshev distance | z = 447                                       | 447               | 40593770  | NelS       |
| Normal biomes, L<sup>-&infin;</sup> distance          | x = 296                                       | 296               | 719387297 | NelS       |
| Large Biomes, L<sup>2</sup>/Euclidean distance        | (384, 412)                                    | 563.21            | 390301013 | NelS       |
| Large Biomes, L<sup>&infin;</sup>/Chebyshev distance  | z = 412                                       | 412               | 390301013 | NelS       |
| Large Biomes, L<sup>-&infin;</sup> distance           | x = 384                                       | 384               | 390301013 | NelS       |
| Superflat, L<sup>2</sup>/Euclidean distance           |                                               |                   |           |            |
| Superflat, L<sup>&infin;</sup>/Chebyshev distance     |                                               |                   |           |            |
| Superflat, L<sup>-&infin;</sup> distance              |                                               |                   |           |            |
<!--
| Single Biome, L<sup>2</sup>/Euclidean distance        |                                               |                   |           |            |
| Single Biome, L<sup>&infin;</sup>/Chebyshev distance  |                                               |                   |           |            |
| Single Biome, L<sup>-&infin;</sup> distance           |                                               |                   |           |            |
-->

## Contributors
<table>
	<thead>
		<tr> <th>Contributor</th> <th>First Seed in Checked Range</th> <th>Last Seed in Checked Range</th> <th>Total Seeds Checked</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=2><b>NelS</b></td> <td>0</td>                <td>719387297</td> <td rowspan=2>719 million (Normal)<br>577 million (Large Biomes)</td> </tr>
		<tr>                                <td>0 (Large Biomes)</td> <td>577447509 (Large Biomes)</td>                                                        </tr>
	</tbody>
</table>

### Other contributions
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created programs