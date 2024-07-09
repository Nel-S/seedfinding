# Mushrooms

These are files relating to finding a normal biomes mushroom-island spawn in Java 1.18+.

The spawn algorithm used in those versions heavily discourages coordinates with extremely low continentalnesses (which is the defining feature of mushroom islands), so having the algorithm directly choose a coordinate on a mushroom island is *de facto* impossible. However, since the last stage of the spawn algorithm can move spawnpoints from water onto land at most five chunks away, achieving a second-stage spawnpoint within five chunks of a mushroom island, with no other land blocks in that radius, will theoretically result in a mushroom island spawn just the same.

The current closest candidate found is 4000088715174867, whose spawn displacement bounding box falls **one** block short of the mushroom island.<br>

A Cubiomes Viewer session file was created to search for these, which was later converted into a (possibly bugged) C program to run on Colin's computing cluster. Later, Jereaux sped up the search conditions and combined it with a filter also looking for far spawnpoints, which has been used by the majority of contributors.
## Contributors
<table>
	<thead>
		<tr> <th>Contributor</th> <th>First Seed in Checked Range</th> <th>Last Seed in Checked Range</th> <th>Total Seeds Checked</th> </tr>
	</thead>
	<tbody>
        <tr> <td><b>Pidge</b></td> <td>4000000000000000</td> <td>4003673549577810</td> <td>3.673 trillion</td> </tr>
		<tr> <td><b>Jereaux</b></td> <td>8810000000000000000</td> <td>8810001174887528740</td> <td>1.174 trillion</td> </tr>
		<tr> <td><b>Meox</b></td> <td>6900000000000000</td> <td>6901084674656939</td> <td>1.084 trillion</td> </tr>
        <tr> <td rowspan=2><b>NelS</b></td> <td>0</td>                    <td>146188867239</td> <td rowspan=2>682 billion</td> </tr>
		<tr>                                <td>-9197754073709551616</td> <td>-9197753537675933132</td>                        </tr>
		<tr> <td rowspan=2><b>ShowMe7</b></td> <td>146188867240</td>     <td>222008539926</td> <td rowspan=2>297 billion</td> </tr>
		<tr>                                   <td>1099511627776(?)</td> <td>1321052383728</td>                               </tr>
		<tr> <td><b>Colin</b></td> <td>146188867240</td> <td>1245700495016</td> <td>1.099 trillion (Possibly bugged)</td> </tr>
	</tbody>
</table>

### Other Contributions
- Cubitect - implemented (ports of) the spawn, climates, and biome-mapping algorithms in [Cubiomes](https://github.com/Cubitect/cubiomes)
- NelS - created Cubiomes Viewer session and C program
- Jereaux - created faster Cubiomes Viewer session