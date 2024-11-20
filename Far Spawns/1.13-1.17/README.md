# 1.13-1.17

Between 1.13 and 1.17, Minecraft: Java Edition still searches for a valid biome in the area ((-256, -256), (256, 256)), then subsequently looks for a grass block. However, the game now performs a spiral iteration up to 16 chunks away (offset by one chunk to the Southeast) instead of jumping using nextInts, which significantly shrinks the range spawnpoints can fall within.

| Criteria                               | <ins>Theoretical</ins> Farthest Spawnpoint from (0, 0) | Distance (blocks) |
| -------------------------------------- | ------------------------------------------------------ | ----------------- |
| L<sup>2</sup>/Euclidean distance       | (527, 527)                                             | 745.29            |
| L<sup>&infin;</sup>/Chebyshev distance | x/z = 527                                              | 527               |
| L<sup>-&infin;</sup> distance          | x/z = 527                                              | 527               |

The only way to achieve such a spawnpoint is for a suitable biome to be at (256, 256); then for the initial biome search to pick exactly that coordinate; then for the spiral iteration to not find any sky-exposed grass blocks until (527, 527).

<table>
	<thead>
		<tr> <th>Distance metric</th> <th>Generation mode</th> <th>Version range</th> <th>Current Farthest-known Spawnpoint from (0, 0)</th> <th>Distance (blocks)</th> <th>Seed</th> <th>Discoverer</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=3>L<sup>2</sup> (Euclidean)</td> <td>Default</td>      <td>1.13-1.17.1</td> <td>(-464,-480)</td> <td>667.60</td> <td>9000000061086707</td> <td>M33_Galaxy</td> </tr>
		<tr>                                              <td>Large Biomes</td> <td>1.13-1.17.1</td> <td>(384, 412)</td> <td>563.21</td> <td>390301013</td> <td>NelS</td>        </tr>
		<tr>                                              <td>Superflat</td>    <td>1.13-1.17.1</td> <td>(256, 256)</td> <td>362.03</td> <td>None yet</td> <td>N/A</td>          </tr>
		<tr> <td rowspan=3>L<sup>&infin;</sup> (Chebyshev)</td> <td>Default</td>      <td>1.13-1.17.1</td> <td>z = 524</td> <td>524</td> <td>199000008157440</td> <td>M33_Galaxy</td> </tr>
		<tr>                                                    <td>Large Biomes</td> <td>1.13-1.17.1</td> <td>z = 412</td> <td>412</td> <td>390301013</td> <td>NelS</td> </tr>
		<tr>                                                    <td>Superflat</td>    <td>1.13-1.17.1</td> <td>x = 256</td> <td>256</td> <td>None yet</td> <td>N/A</td>   </tr>
		<tr> <td rowspan=3>L<sup>-&infin;</sup></td> <td>Default</td>      <td>1.13-1.17.1</td> <td>x = 296</td> <td>296</td> <td>719387297</td> <td>NelS</td> </tr>
		<tr>                                         <td>Large Biomes</td> <td>1.13-1.17.1</td> <td>x = 384</td> <td>384</td> <td>390301013</td> <td>NelS</td> </tr>
		<tr>                                         <td>Superflat</td>    <td>1.13-1.17.1</td> <td>x = 256</td> <td>256</td> <td>None yet</td> <td>N/A</td>   </tr>
	</tbody>
</table>

## Contributors
<table>
	<thead>
		<tr> <th>Contributor</th> <th>First Seed in Checked Range</th> <th>Last Seed in Checked Range</th> <th>Total Seeds Checked</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=2><b>NelS</b></td> <td>0</td>                <td>719387297</td> <td rowspan=2>719 million (Normal)<br>577 million (Large Biomes)</td> </tr>
		<tr>                                <td>0 (Large Biomes)</td> <td>577447509 (Large Biomes)</td>                                                        </tr>
		<tr> <td rowspan=2><b>M33_Galaxy</b></td> <td>199000000000000</td>  <td>199000008157440</td> <td rowspan=2>69 million (Normal)</td> </tr>
		<tr>                                      <td>9000000000000000</td> <td>9000000061086707</td>                                       </tr>
	</tbody>
</table>

### Other contributions
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created programs