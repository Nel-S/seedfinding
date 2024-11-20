# Beta 1.8 - 1.12

Between Beta 1.8 and 1.12.2, Minecraft: Java Edition searched for a valid biome in the area ((-256, -256), (256, 256)). In Beta 1.8-1.0, this comprised of forests, swamps, or taigas; after that this comprised of forests, plains, taigas, taiga hills, wooded hills, jungles, or jungle_hills. Then the game looks for a grass block, jumping between -63 and 63 blocks in the x- and z-directions up to 1000 times in its attempts to find one.

| Criteria                               | <ins>Theoretical</ins> Farthest Spawnpoint from (0, 0) | Distance (blocks) | Required Internal java.util.Random State |
| -------------------------------------- | ------------------------------------------------------ | ----------------- | ---------------------------------------- |
| L<sup>2</sup>/Euclidean distance       | (4437, -5060)                                          | 6729.82           | 94569449877293                           |
| L<sup>&infin;</sup>/Chebyshev distance | x/z = -6612                                            | 6612              | 212610048832663 / 104315139551573        |
| L<sup>-&infin;</sup> distance          | z = 4714                                               | 4714              | 264360186047016                          |

Due to terrain and algorithm differences, Beta 1.8-1.8.1, 1.0, 1.1-1.6.4, 1.7-1.8.9, and 1.9-1.12.2 will likely have different farthest spawnpoints. Minor versions may also differ.

<table>
	<thead>
		<tr> <th>Distance metric</th> <th>Generation mode</th> <th>Version range</th> <th>Current Farthest-known Spawnpoint from (0, 0)</th> <th>Distance (blocks)</th> <th>Seed</th> <th>Discoverer</th> </tr>
	</thead>
	<tbody>
		<tr> <td rowspan=8>L<sup>2</sup> (Euclidean)</td> <td rowspan=5>Default</td> <td>Beta 1.8-1.8.1</td> <td>(-6357, 26)</td>   <td>6357.05</td> <td>112580053380563470</td> <td rowspan=24>NelS</td> </tr>
		<tr>                                                                         <td>1.0</td>            <td>(-4836, 4576)</td> <td>6657.82</td> <td>8744117927817498179</td>  </tr>
		<tr>                                                                         <td>1.1-1.6.4</td>      <td>(-6357, 26)</td>   <td>6357.05</td> <td>-6667870660602428914</td> </tr>
		<tr>                                                                         <td>1.7.2-1.8.9</td>    <td>(3435, -3404)</td> <td>4835.95</td> <td>-1333587273300622</td>    </tr>
		<tr>                                                                         <td>1.9-1.12.2</td>     <td>(3435, -3404)</td> <td>4835.95</td> <td>-1333587273300622</td>    </tr>
		<tr>                                              <td rowspan=3>Large Biomes</td> <td>1.3.1-1.6.4</td> <td>(-4904, 4216)</td> <td>6467.13</td> <td>7161689312549113738</td> </tr>
		<tr>                                                                              <td>1.7.2-1.8.9</td> <td>(4180, -4801)</td> <td>6365.68</td> <td>3424306844298200916</td> </tr>
		<tr>                                                                              <td>1.9-1.12.2</td>  <td>(4188, -4793)</td> <td>6364.91</td> <td>48014498653882196</td>   </tr>
		<tr> <td rowspan=8>L<sup>&infin;</sup> (Chebyshev)</td> <td rowspan=5>Default</td> <td>Beta 1.8-1.8.1</td> <td>x = -6357</td> <td>6357</td> <td>112580053380563470</td>   </tr>
		<tr>                                                                               <td>1.0</td>            <td>x = -6357</td> <td>6357</td> <td>9058417763198632462</td>  </tr>
		<tr>                                                                               <td>1.1-1.6.4</td>      <td>x = -6357</td> <td>6357</td> <td>-6667870660602428914</td> </tr>
		<tr>                                                                               <td>1.7.2-1.8.9</td>    <td>z = -3995</td> <td>3995</td> <td>8309987206736090924</td>  </tr>
		<tr>                                                                               <td>1.9-1.12.2</td>     <td>x = -4504</td> <td>4504</td> <td>-6045810962071879154</td> </tr>
		<tr>                                                    <td rowspan=3>Large Biomes</td> <td>1.3.1-1.6.4</td> <td>x = -6357</td> <td>6357</td> <td>4775137300382222</td>  </tr>
		<tr>                                                                                    <td>1.7.2-1.8.9</td> <td>x = -6357</td> <td>6357</td> <td>21945110879732238</td> </tr>
		<tr>                                                                                    <td>1.9-1.12.2</td>  <td>x = -6349</td> <td>6349</td> <td>21945110879732238</td> </tr>
		<tr> <td rowspan=8>L<sup>-&infin;</sup></td> <td rowspan=5>Default</td> <td>Beta 1.8-1.8.1</td> <td>z = 3021</td>  <td>3021</td> <td>4585255578012054873</td>  </tr>
		<tr>                                                                    <td>1.0</td>            <td>z = 4576</td>  <td>4576</td> <td>8744117927817498179</td>  </tr>
		<tr>                                                                    <td>1.1-1.6.4</td>      <td>z = 4351</td>  <td>4351</td> <td>-7197333022247240870</td> </tr>
		<tr>                                                                    <td>1.7.2-1.8.9</td>    <td>z = -3404</td> <td>3404</td> <td>-1333587273300622</td>    </tr>
		<tr>                                                                    <td>1.9-1.12.2</td>     <td>z = -3404</td> <td>3404</td> <td>-1333587273300622</td>    </tr>
		<tr>                                         <td rowspan=3>Large Biomes</td> <td>1.3.1-1.6.4</td> <td>x = 4445</td> <td>4445</td> <td>-427974997362436053</td>  </tr>
		<tr>                                                                         <td>1.7.2-1.8.9</td> <td>z = 4458</td> <td>4458</td> <td>-4466462063201907643</td> </tr>
		<tr>                                                                         <td>1.9-1.12.2</td>  <td>x = 4443</td> <td>4443</td> <td>4799015320154445867</td>  </tr>
	</tbody>
</table>

See [the repository's parent README](../../README.md) for compilation instructions.

## Contributors
- Cubitect - implemented (port of) the spawn algorithm in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - initially created C/CUDA programs, checked 28.59 trillion internal states
- Jereaux - checked 6.93 trillion internal states
- Andrew - made internal state program 170x faster; checked 245 trillion states; determined at most 16647 state advancements can occur across consecutive nextInt(2) ... nextInt(16641) calls