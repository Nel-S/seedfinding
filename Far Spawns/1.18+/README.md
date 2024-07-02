# 1.18+

In 1.18, Minecraft: Java Edition and Minecraft: Bedrock Edition switched to using a fitness-based approach for their spawn algorithms: the games attempt to find a coordinate that balances being near (0, 0), not having climate values beyond &pm;1, and not having climate values liable to induce ocean or river generation. A fuller explanation of the algorithm is given underneath [this tool](https://nel-s.github.io/efc-calc/).

| Criteria                             | <ins>Theoretical</ins> Farthest Spawnpoint from (0, 0) | Distance (blocks) |
| ------------------------------------ | ------------------------------------------------------ | ----------------- |
| Java 1.18+, L2/Euclidean distance    | (1599, -2160)                                          | 2687.45           |
| Java 1.18+, L∞/Chebyshev distance    | x = 2647                                               | 2647              |
| Bedrock 1.18+, L2/Euclidean distance | (0, 2560)                                              | 2560              |
| Bedrock 1.18+, L∞/Chebyshev distance | x = 2560                                               | 2560              |

The [Forward Checker](./Forward%20Checker/) is usable for "small" ranges of seeds, and (excluding Jereaux's seeds) has been used to find almost all farthest-known spawnpoints below. However, the only true way to find the farthest spawnpoint in the entire seedspace would be by deriving worldseeds from their Perlin octaves. With regards to that, the maximum/minimum possible Perlin sample has been found to be &pm;1.0363538112118025; the next step involves creating a table of all Perlin octave configurations' possible ranges.

| Criteria                                                            | Current Farthest-known Spawnpoint from (0, 0) | Distance (blocks) | Seed                 | Discoverer |
| ------------------------------------------------------------------- | --------------------------------------------- | ----------------- | -------------------- | ---------- |
| Java 1.18+ normal biomes, L<sup>2</sup>/Euclidean distance          | (-1343, -1136)                                | 1759.02           | 2242180942571        | NelS       |
| Java 1.18+ normal biomes, L<sup>&infin;</sup>/Chebyshev distance    | x = -1728                                     | 1728              | 690455132394644      | Meox       |
| Java 1.18+ normal biomes, L<sup>-&infin;</sup> distance             | x = -1728                                     | 1728              | 690455132394644      | Meox       |
| Java 1.18+ Large Biomes, L<sup>2</sup>/Euclidean distance           | (2363, -356)                                  | 2389.67           | 6015757257           | Derm       |
| Java 1.18+ Large Biomes, L<sup>&infin;</sup>/Chebyshev distance     | x = 2363                                      | 2363              | 6015757257           | Derm       |
| Bedrock 1.18+ normal biomes, L<sup>2</sup>/Euclidean distance       | (1515, 829)                                   | 1726.98           | 8514982786202962122  | Jereaux    |
| Bedrock 1.18+ normal biomes, L<sup>&infin;</sup>/Chebyshev distance | x = 1716                                      | 1716              | -1942038948813007687 | Jereaux    |

## Contributors
<table>
	<thead>
		<tr> <th>Contributor</th> <th>First Seed in Checked Range</th> <th>Last Seed in Checked Range</th> <th>Total Seeds Checked</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=6><b>Jereaux</b></td> <td>? (included 136139140363362)</td> <td>?</td> <td rowspan=6>11.401 trillion (Normal)</td> </tr>
		<tr>                                   <td>-1942039172054938501</td>         <td>-1942033628660909384</td>                          </tr>
		<tr>                                   <td>-2086446383076507923</td>         <td>-2086442811344446008</td>                          </tr>
		<tr>                                   <td>8514982163198740964</td>          <td>8514982827054712191</td>                           </tr>
		<tr>                                   <td>-1519791477682421020</td>         <td>-1519790283805697334</td>                          </tr>
		<tr>                                   <td>-7221474987810836822</td>         <td>-7221474578187462918</td>                          </tr>
		<tr> <td rowspan=3><b>Meox</b></td> <td>215772259249 (48-bit)</td> <td>215773147572 (48-bit)</td> <td rowspan=3>2.871 trillion (Normal)</td> </tr>
		<tr>                                <td>3366025373172</td>         <td>3426926457792</td>                                                    </tr>
		<tr>                                <td>690000000000000</td>       <td>692752814164988</td>                                                  </tr>
		<tr> <td><b>Philipp_DE<br />BoySanic<br />Derm<br />Meox<br />SundayMC<br />Acebase<br />MPM<br />[Anonymous]</b></td> <td>320416846294</td> <td>2210416846294</td> <td>1.89 trillion (Normal)</td> </tr>
		<tr> <td rowspan=3><b>NelS</b></td> <td>0</td>                         <td>320416846294</td> <td rowspan=3>1.476 trillion (Normal)<br>154 billion (Large Biomes)</td> </tr>
		<tr>                                <td>2210416846294</td>             <td>3366025373172</td>                                                                         </tr>
		<tr>                                <td>6209940093 (Large Biomes)</td> <td>160265862227 (Large Biomes)</td>                                                           </tr>
		<tr> <td><b>Derm</b></td> <td>0 (Large Biomes)</td> <td>6209940093 (Large Biomes)</td> <td>6.2 billion (Large Biomes)</td> </tr>
	</tbody>
</table>

### Other contributions
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created most programs
- Jereaux - improved Cubiomes Viewer sessions
- Matt - set up a microBOINC project for the Forward Checker, run by Philipp_DE et. al.

## Relevant Links
- A [Desmos graph](https://www.desmos.com/calculator/6jefxtspjo) displaying the coordinates checked by the first two spawn stages
- A [Reddit post](https://www.reddit.com/r/minecraftseeds/comments/13kc14n/a_435_million_block_mushroom_island_and_a_spawn/) by me revealing 2242180942571 and 2939217269032
- A [compilation video](https://www.youtube.com/watch?v=hvKbkyQuZF8&t=38) by TheMisterEpic showcasing 2242180942571 (mostly accurate)
- A [compilation video](https://www.youtube.com/watch?v=_gx-fPZdrb8&t=348) by Rovant showcasing 2242180942571 (very, very inaccurate)