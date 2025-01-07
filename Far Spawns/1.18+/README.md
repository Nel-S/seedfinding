# 1.18+

In 1.18, Minecraft: Java Edition and Minecraft: Bedrock Edition switched to using a fitness-based approach for their spawn algorithms: the games attempt to find a coordinate that balances being near (0, 0), not having climate values beyond &pm;1, and not having climate values liable to induce ocean or river generation. A fuller explanation of the algorithm is given underneath [this tool](https://nel-s.github.io/efc-calc/).

In 1.21.2+, the same algorithm is used, but the penalty for undesirable climates is increased, greatly increasing the spawnpoint distances that can be found.

| Criteria                                              | <ins>Theoretical</ins> Farthest Spawnpoint from (0, 0) | Distance (blocks) |
| ----------------------------------------------------- | ------------------------------------------------------ | ----------------- |
| Java 1.18+, L<sup>2</sup>/Euclidean distance          | (1599, -2160)                                          | 2687.45           |
| Java 1.18+, L<sup>&infin;</sup>/Chebyshev distance    | z = 2655                                               | 2655              |
| Java 1.18+, L<sup>-&infin;</sup> distance             | x = -1888                                              | 1888              |
| Bedrock 1.18+, L<sup>2</sup>/Euclidean distance       | (0, 2560)                                              | 2560              |
| Bedrock 1.18+, L<sup>&infin;</sup>/Chebyshev distance | z = 2560                                               | 2560              |

The [Forward Checker](./Forward%20Checker/) is usable for "small" ranges of seeds, and (excluding Jereaux's seeds) has been used to find almost all farthest-known 1.18-1.21.1 spawnpoints below. However, the only true way to find the farthest spawnpoint in the entire seedspace would be by deriving worldseeds from their Perlin octaves. With regards to that, the maximum/minimum possible Perlin sample has been found to be &pm;1.0363538112118025; the next step involves creating a table of all Perlin octave configurations' possible ranges.

<table>
	<thead>
		<tr> <th>Distance metric</th> <th>Generation mode</th> <th>Platform and version range</th> <th>Current Farthest-known Spawnpoint from (0, 0)</th> <th>Distance (blocks)</th> <th>Seed</th> <th>Discoverer</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=5>L<sup>2</sup> (Euclidean)</td> <td rowspan=3>Default</td> <td>Java 1.18-1.21.1</td> <td>(-1343, -1136)</td> <td>1759.02</td> <td>2242180942571</td> <td>NelS</td> </tr>
		<tr>                                                                         <td>Bedrock 1.18-1.21.1</td> <td>(1515, 829)</td> <td>1726.98</td> <td>8514982786202962122</td> <td>Jereaux</td> </tr>
		<tr>                                                                         <td>Java 1.21.2+</td> <td>(-1392, -2160)</td> <td>2569.68</td> <td>1100809258860442</td> <td>Zennith</td> </tr>
		<tr>                                              <td rowspan=2>Large Biomes</td> <td>Java 1.18-1.21.1</td> <td>(-1560, -1816)</td> <td>2394.05</td> <td>1100968538880257</td> <td>Zennith</td> </tr>
		<tr>                                                                              <td>Java 1.21.2+</td> <td>(1599, -2160)</td> <td>2687.45</td> <td>530524958</td> <td>NelS</td> </tr>
		<tr> <td rowspan=5>L<sup>&infin;</sup> (Chebyshev)</td> <td rowspan=3>Default</td> <td>Java 1.18-1.21.1</td> <td>x = -1728</td> <td>1728</td> <td>690455132394644</td> <td>Meox</td> </tr>
		<tr>                                                                               <td>Bedrock 1.18-1.21.1</td> <td>x = 1716</td> <td>1716</td> <td>-1942038948813007687</td> <td>Jereaux</td> </tr>
		<tr>                                                                               <td>Java 1.21.2+</td> <td>x = -2560</td> <td>2560</td> <td>9000000126799615201</td> <td>Fragrant_Result_186</td> </tr>
		<tr>                                                     <td rowspan=2>Large Biomes</td> <td>Java 1.18-1.21.1</td> <td>z = -2376</td> <td>2376</td> <td>1100951982918886</td> <td>Zennith</td> </tr>
		<tr>                                                                                     <td>Java 1.21.2+</td> <td>z = 2655</td> <td>2655</td> <td>53854196</td> <td>NelS</td> </tr>
		<tr> <td rowspan=2>L<sup>-&infin;</sup></td> <td>Default</td> <td>Java 1.21.2+</td> <td>x = -1744</td> <td>1744</td> <td>223527517729</td> <td>NelS</td> </tr>
		<tr>                                         <td>Large Biomes</td> <td>Java 1.21.2+</td> <td>x = -1888</td> <td>1888</td> <td>5631362</td> <td>NelS</td> </tr>
	</tbody>
</table>

## Contributors
<table>
	<thead>
		<tr> <th>Contributor</th> <th>First Seed in Checked Range</th> <th>Last Seed in Checked Range</th> <th>Total Seeds Checked</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=6><b>Jereaux</b></td> <td>? (included 136139140363362)</td> <td>?</td> <td rowspan=6>11.401 trillion (1.18-1.21.1 Default)</td> </tr>
		<tr>                                   <td>-1942039172054938501</td>         <td>-1942033628660909384</td>                                       </tr>
		<tr>                                   <td>-2086446383076507923</td>         <td>-2086442811344446008</td>                                       </tr>
		<tr>                                   <td>8514982163198740964</td>          <td>8514982827054712191</td>                                        </tr>
		<tr>                                   <td>-1519791477682421020</td>         <td>-1519790283805697334</td>                                       </tr>
		<tr>                                   <td>-7221474987810836822</td>         <td>-7221474578187462918</td>                                       </tr>
		<tr> <td rowspan=3><b>Meox</b></td> <td>215772259249 (48-bit)</td> <td>215773147572 (48-bit)</td> <td rowspan=3>2.871 trillion (1.18-1.21.1 Default)</td> </tr>
		<tr>                                <td>3366025373172</td>         <td>3426926457792</td>                                                                 </tr>
		<tr>                                <td>690000000000000</td>       <td>692752814164988</td>                                                               </tr>
		<tr> <td rowspan=3><b>Zennith</b></td> <td>1100000000000000</td>                <td>1101229653433314</td> <td rowspan=3>1.229 trillion (1.18-1.21.1 Default)<br>500 billion (1.18-1.21.1 Large Biomes)<br>809 billion (1.21.2+)</td> </tr>
		<tr>                                   <td>1100500000000000 (Large Biomes)</td> <td>1101000000000000 (Large Biomes)</td>                                                 </tr>
		<tr>                                   <td>1100000000000000 (1.21.2+)</td>      <td>1100809258860442 (1.21.2+)</td>                                                      </tr>
		<tr> <td rowspan=5><b>NelS</b></td> <td>0</td>                            <td>320416846294</td> <td rowspan=5>1.476 trillion (1.18-1.21.1 Default)<br>154 billion (1.18-1.21.1 Large Biomes)<br>288 billion (1.21.2+ Default)<br>530 million (1.21.2+ Large Biomes)</td> </tr>
		<tr>                                <td>2210416846294</td>                <td>3366025373172</td>                                                                         </tr>
		<tr>                                <td>6209940093 (Large Biomes)</td>    <td>160265862227 (Large Biomes)</td>                                                           </tr>
		<tr>                                <td>7835601 (1.21.2+)</td>            <td>288769119726 (1.21.2+)</td>                                                                </tr>
		<tr>                                <td>30388 (1.21.2+ Large Biomes)</td> <td>530524958 (1.21.2+ Large Biomes)</td>                                                      </tr>
		<tr> <td><b>Philipp_DE<br />BoySanic<br />Derm<br />Meox<br />SundayMC<br />Acebase<br />MPM<br />[Anonymous]</b></td> <td>320416846294</td> <td>2210416846294</td> <td>1.89 trillion (1.18-1.21.1 Default)</td> </tr>
		<tr> <td><b>Fragrant_Result_186</b></td> <td>9000000000000000000(?) (1.21.2+)</td> <td>9000000126799615201 (1.21.2+)</td> <td>126 billion (1.21.2+)</td> </tr>
		<tr> <td rowspan=6><b>M33_Galaxy</b></td> <td>0 (1.21.2+)</td>                            <td>7835600 (1.21.2+)</td> <td rowspan=6>124 billion (1.21.2+ Default)<br>261 million (1.21.2+ Large Biomes)</td> </tr>
		<tr>                                      <td>17001057347671 (1.21.2+)</td>               <td>17001234463471 (1.21.2+)</td>                                              </tr>
		<tr>                                      <td>12345678912345678(?) (1.21.2+)</td>         <td>12345702767680896 (1.21.2+)</td>                                           </tr>
		<tr>                                      <td>6633969375806510644 (1.21.2+)</td>          <td>6633969475802190251 (1.21.2+)</td>                                         </tr>
		<tr>                                      <td>0 (1.21.2+ Large Biomes)</td>               <td>30387 (1.21.2+ Large Biomes)</td>                                          </tr>
		<tr>                                      <td>100000000000000 (1.21.2+ Large Biomes)</td> <td>100000261949436 (1.21.2+ Large Biomes)</td>                                </tr>
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