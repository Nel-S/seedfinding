#ifndef _SPAWN_H
#define _SPAWN_H

#include "Climates.h"

enum {U_spawn_table_x, U_spawn_table_z, U_spawn_table_fitness, U_spawn_table_entrySize};
extern const int U_SPAWN_FIRST_STAGE_VALS[66][U_spawn_table_entrySize];
extern const double U_MAX_CLIMATE_FITNESSES[NP_MAX], U_SPAWN_SECOND_STAGE_VALS[66][862][U_spawn_table_entrySize];

// Returns the fitness of a point, given its coordinate and its climate values.
// All parameters are nullable if desired.
double U_getFitness(const Pos *coord, const double *temperature, const double *humidity, const double *continentalness, const double *erosion, const double *weirdness);
// Calculates the fitness of a point, aborting early (and returning false) if the fitness ever rises above `*upperBound`.
// Otherwise returns true and stores the fitness value in `*fitness`.
// All parameters except `*fitness` are nullable if desired.
bool U_getFitnessBounded(const Pos *coord, const double *temperature, const double *humidity, const double *continentalness, const double *erosion, const double *weirdness, const double *upperBound, double *fitness);

// Samples each climate to return the fitness of a point.
// All climates must have been initialized beforehand (with e.g. `U_initAllClimates()`.)
double U_sampleAndGetFitness(const Pos *coord, PerlinNoise *oct);

// Samples each climate to calculate the fitness of a point, aborting early (and returning false) if the fitness ever rises above `*upperBound`.
// Otherwise returns true and stores the fitness value in `*fitness`.
// All climates must have been initialized beforehand (with e.g. `U_initAllClimates()`.)
bool U_sampleAndGetFitnessBounded(const Pos *coord, PerlinNoise *oct, const double *upperBound, double *fitness);

// Returns the Euclidean distance that would equate to the provided fitness value.
double U_getEffectiveDistance(const double fitness);

// Returns the positive temperature that would equate to the provided fitness value.
// The equivalent negative temperature can be obtained by negating the output.
double U_getEffectiveTemperature(const double fitness);

// Returns the positive humidity that would equate to the provided fitness value.
// The equivalent negative humidity can be obtained by negating the output.
double U_getEffectiveHumidity(const double fitness);

// Returns the **negative** continentalness that would equate to the provided fitness value.
// The equivalent positive continentalness would be the same value as `U_getEffectiveTemperature(fitness)`.
double U_getEffectiveContinentalness(const double fitness);

// Returns the positive erosion that would equate to the provided fitness value.
// The equivalent negative erosion can be obtained by negating the output.
double U_getEffectiveErosion(const double fitness);

// Returns the positive weirdness > 1 that would equate to the provided fitness value.
// The equivalent negative weirdness < -1 can be obtained by negating the output.
double U_getEffectiveWeirdnessOuter(const double fitness);

// Returns the positive weirdness < 0.16 that would equate to the provided fitness value, or `INFINITY` if none exists.
// The equivalent negative weirdness > -0.16 can be obtained by negating the output.
double U_getEffectiveWeirdnessInner(const double fitness);

// Emulates the first stage of the spawn algorithm, aborting early (and returning false) if the current chosen point's fitness ever drops below `fitnessLowerBound`.
// Otherwise returns true and stores the chosen coordinate index in `*chosenCoordIndex` fitness value in `*fitness`.
bool U_firstStageSpawnBounded(PerlinNoise *oct, const double fitnessLowerBound, int *chosenCoordIndex, double *chosenFitness);

// `U_firstStageSpawnBounded()`, but derives the coordinates manually instead of using a lookup table. For environments with limited memory.
bool U_firstStageSpawnBounded_noTable(PerlinNoise *oct, const double fitnessLowerBound, Pos *chosenCoord, double *chosenFitness);

// Emulates the second stage of the spawn algorithm, aborting early (and returning false) if the current chosen point's fitness ever drops below `fitnessLowerBound`.
// Otherwise returns true and stores the chosen coordinate index fitness value in `*fitness`.
bool U_secondStageSpawnBounded(PerlinNoise *oct, const int firstStageChosenCoordIndex, const double firstStageChosenFitness, const double fitnessLowerBound, int *chosenCoordIndex, double *chosenFitness);

// `U_secondStageSpawnBounded()`, but derives the coordinates manually instead of using a lookup table. For environments with limited memory.
bool U_secondStageSpawnBounded_noTable(PerlinNoise *oct, const Pos *firstStageChosenCoord, const double firstStageChosenFitness, const double fitnessLowerBound, Pos *chosenCoord, double *chosenFitness);
#endif