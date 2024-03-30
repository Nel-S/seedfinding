# Templates
These are templated `main()` functions one can link this repository's programs to.

Alternatively, you can create your own `main()` function and link the programs in this repository to it. For compatibility, custom main functions must
1. call `initGlobals()` before beginning the search;
2. call `checkSeeds()` for each worker to begin their search, while passing the memory address of the worker's index (counted as 0, 1, ..., `localNumberOfWorkers` - 1) to the function; and
3. implement the output method `outputValues(const uint64_t *seeds, const void *otherValues, const size_t count)` (which receives a `count`-element array of seeds and a `count`-element array of related values).
Otherwise, one may setup their main function as they wish. Almost all programs will provide the global constants `GLOBAL_START_SEED`, `GLOBAL_SEEDS_TO_CHECK`, `GLOBAL_NUMBER_OF_WORKERS`, `FILEPATH`, and the constant flag `TIME_PROGRAM`, but these can be ignored if one wishes. Similarly, all workers will call the variables `localStartSeed`, `localSeedsToCheck`, and `localNumberOfWorkers`; these are initialized to `GLOBAL_START_SEED`, `GLOBAL_SEEDS_TO_CHECK`, and `GLOBAL_NUMBER_OF_WORKERS`, respectively, but may be freely changed.

## Acknowledgements
This program relies heavily on the [Cubiomes](https://github.com/Cubitect/cubiomes) library, which was created by [Cubitect](https://github.com/Cubitect) and released under the MIT License.