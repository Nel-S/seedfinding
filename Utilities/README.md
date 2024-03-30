# Utilities
This is a library of 1.18+ climate- and spawn-related constants, functions, and lookup tables.

Some of these functions are already present in the [Cubiomes](https://github.com/Cubitect/cubiomes) library, but are made faster here (such as by aborting early if a certain threshold is passed, etc.). Others are new.

In the future, a makefile will be developed to statically compile this library; until then, `Utilities/Climates.c`, `Utilities/Spawn.c`, and `"Utilities/Spawn Lookup Table.c"` must be manually specified when compiling.