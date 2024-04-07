#include "Spawn.h"

// The maximum fitness that each climate by itself can equate to.
const double U_MAX_CLIMATE_FITNESSES[NP_MAX] = {169783107.122, 56953151.7314, 1521389678.6949422, 227197622.444, 0, 384556370.747};

double U_getFitness(const Pos *coord, const double *temperature, const double *humidity, const double *continentalness, const double *erosion, const double *weirdness) {
	double fitness;
	U_getFitnessBounded(coord, temperature, humidity, continentalness, erosion, weirdness, NULL, &fitness);
	return fitness;
}

bool U_getFitnessBounded(const Pos *coord, const double *temperature, const double *humidity, const double *continentalness, const double *erosion, const double *weirdness, const double *upperBound, double *fitness) {
	// Distance
	uint64_t squaredEuclid = IF_EXISTS(coord, (uint64_t)coord->x * coord->x + (uint64_t)coord->z * coord->z);
	*fitness = (squaredEuclid*squaredEuclid)/390625.;
	if (upperBound && *fitness >= *upperBound) return false;
	// Continentalness
	double offset = IF_EXISTS(continentalness, *continentalness < 0 ? min(*continentalness + 0.11, 0) : max(*continentalness - 1, 0));
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Weirdness
	offset = IF_EXISTS(weirdness, max(fabs(*weirdness) < 0.16 ? 0.16 - fabs(*weirdness) : fabs(*weirdness) - 1, 0));
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Erosion
	offset = IF_EXISTS(erosion, max(fabs(*erosion) - 1, 0));
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Temperature
	offset = IF_EXISTS(temperature, max(fabs(*temperature) - 1, 0));
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Humidity
	offset = IF_EXISTS(humidity, max(fabs(*humidity) - 1, 0));
	*fitness += 100000000*offset*offset;
	return !upperBound || *fitness < *upperBound;
}

double U_sampleAndGetFitness(const Pos *coord, PerlinNoise *oct) {
	double fitness = -INFINITY;
	U_sampleAndGetFitnessBounded(coord, oct, NULL, &fitness);
	return fitness;
}

bool U_sampleAndGetFitnessBounded(const Pos *coord, PerlinNoise *oct, const double *upperBound, double *fitness) {
	if (!coord) return false;
	// Distance
	uint64_t squaredEuclid = (uint64_t)coord->x * coord->x + (uint64_t)coord->z * coord->z;
	*fitness = (squaredEuclid*squaredEuclid)/390625.;
	if (upperBound && *fitness >= *upperBound) return false;
	double px = coord->x, pz = coord->z;
	U_sampleClimate(NP_SHIFT, oct, &px, &pz);
	// Continentalness
	double sample = U_sampleClimate(NP_CONTINENTALNESS, oct, &px, &pz);
	double offset = sample < 0 ? min(sample + 0.11, 0) : max(sample - 1, 0);
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Weirdness
	sample = U_sampleClimate(NP_WEIRDNESS, oct, &px, &pz);
	offset = max(fabs(sample) < 0.16 ? 0.16 - fabs(sample) : fabs(sample) - 1, 0);
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Erosion
	sample = U_sampleClimate(NP_EROSION, oct, &px, &pz);
	offset = max(fabs(sample) - 1, 0);
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Temperature
	sample = U_sampleClimate(NP_TEMPERATURE, oct, &px, &pz);
	offset = max(fabs(sample) - 1, 0);
	*fitness += 100000000*offset*offset;
	if (upperBound && *fitness >= *upperBound) return false;
	// Humidity
	sample = U_sampleClimate(NP_HUMIDITY, oct, &px, &pz);
	offset = max(fabs(sample) - 1, 0);
	*fitness += 100000000*offset*offset;
	return !upperBound || *fitness < *upperBound;
}

double U_getEffectiveDistance(const double fitness) {
	if (fitness < 0) return -INFINITY;
	return 25*pow(fitness, 1./4);
}

double U_getEffectiveTemperature(const double fitness) {
	if (fitness < 0) return -INFINITY;
	return 1 + sqrt(fitness)/10000;
}

double U_getEffectiveHumidity(const double fitness) {
	return U_getEffectiveTemperature(fitness);
}

double U_getEffectiveContinentalness(const double fitness) {
	if (fitness < 0) return -INFINITY;
	return -0.11 - sqrt(fitness)/10000;
}

double U_getEffectiveErosion(const double fitness) {
	return U_getEffectiveTemperature(fitness);
}

double U_getEffectiveWeirdnessOuter(const double fitness) {
	return U_getEffectiveTemperature(fitness);
}

double U_getEffectiveWeirdnessInner(const double fitness) {
	if (fitness < 0) return INFINITY;
	if (fitness > 2560000) return -INFINITY;
	return 0.16 - sqrt(fitness)/10000;
}

// bool U_spawnStageBounded(PerlinNoise *oct, const double fitnessLowerBound, const int *firstStageChosenCoordIndex, int *chosenCoordIndex, double *chosenFitness) {
// 	static const int *TABLES[] = {U_SPAWN_FIRST_STAGE_VALS, U_SPAWN_SECOND_STAGE_VALS};
// 	const int *CHOSEN_TABLE = firstStageChosenCoordIndex ? U_SPAWN_SECOND_STAGE_VALS[*firstStageChosenCoordIndex] : U_SPAWN_FIRST_STAGE_VALS;
// 	double bestFitness = INFINITY;
// 	// TODO: Continue as soon as an individual samplePerlin pushes fitness over fitness?
// 	for (size_t i = 0; i < sizeof(CHOSEN_TABLE)/sizeof(*CHOSEN_TABLE); ++i) {
// 		double fitness = CHOSEN_TABLE[i][U_spawn_table_fitness];
// 		if (fitness >= bestFitness) continue;
// 		Pos pos = {CHOSEN_TABLE[i][U_spawn_table_x], CHOSEN_TABLE[i][U_spawn_table_z]};
// 		if (!U_sampleAndGetFitnessBounded(&pos, oct, &bestFitness, &fitness)) continue;
// 		if (chosenCoordIndex) *chosenCoordIndex = i;
// 		if (*chosenFitness) *chosenFitness = fitness;
// 		if (bestFitness < fitnessLowerBound) return false;
// 	}
// 	return true;
// }

bool U_firstStageSpawnBounded(PerlinNoise *oct, const double fitnessLowerBound, int *chosenCoordIndex, double *chosenFitness) {
	double bestFitness = INFINITY;
	// TODO: Continue as soon as an individual samplePerlin pushes fitness over fitness?
	for (size_t i = 0; i < sizeof(U_SPAWN_FIRST_STAGE_VALS)/sizeof(*U_SPAWN_FIRST_STAGE_VALS); ++i) {
		double fitness = U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_fitness];
		if (fitness >= bestFitness) continue;
		Pos pos = {U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_x], U_SPAWN_FIRST_STAGE_VALS[i][U_spawn_table_z]};
		if (!U_sampleAndGetFitnessBounded(&pos, oct, &bestFitness, &fitness)) continue;
		if (chosenCoordIndex) *chosenCoordIndex = i;
		if (*chosenFitness) *chosenFitness = fitness;
		bestFitness = fitness;
		if (bestFitness < fitnessLowerBound) return false;
	}
	return true;
}

bool U_secondStageSpawnBounded(PerlinNoise *oct, const int firstStageChosenCoordIndex, const double firstStageChosenFitness, const double fitnessLowerBound, int *chosenCoordIndex, double *chosenFitness) {
	double bestFitness = firstStageChosenFitness;
	// TODO: Continue as soon as an individual samplePerlin pushes fitness over fitness?
	for (size_t i = 0; i < sizeof(U_SPAWN_SECOND_STAGE_VALS[firstStageChosenCoordIndex])/sizeof(*U_SPAWN_SECOND_STAGE_VALS[firstStageChosenCoordIndex]); ++i) {
		double fitness = U_SPAWN_SECOND_STAGE_VALS[firstStageChosenCoordIndex][i][U_spawn_table_fitness];
		if (fitness >= bestFitness) continue;
		Pos pos = {U_SPAWN_SECOND_STAGE_VALS[firstStageChosenCoordIndex][i][U_spawn_table_x], U_SPAWN_SECOND_STAGE_VALS[firstStageChosenCoordIndex][i][U_spawn_table_z]};
		if (!U_sampleAndGetFitnessBounded(&pos, oct, &bestFitness, &fitness)) continue;
		if (chosenCoordIndex) *chosenCoordIndex = i;
		if (*chosenFitness) *chosenFitness = fitness;
		bestFitness = fitness;
		if (bestFitness < fitnessLowerBound) return false;
	}
	return true;
}

bool U_firstStageSpawnBounded_noTable(PerlinNoise *oct, const double fitnessLowerBound, Pos *chosenCoord, double *chosenFitness) {
	double fitness, bestFitness = INFINITY;
	for (double rad = 0.; rad <= 2048.; rad += 512.) {
		for (double ang = 0.; ang <= U_TWO_PI; ang += rad ? 512./rad : INFINITY) {
			Pos pos = {sin(ang) * rad, cos(ang) * rad};
			if (!U_sampleAndGetFitnessBounded(&pos, oct, &bestFitness, &fitness)) continue;
			if (chosenCoord) {
				chosenCoord->x = pos.x;
				chosenCoord->z = pos.z;
			}
			bestFitness = fitness;
			if (*chosenFitness) *chosenFitness = fitness;
			if (bestFitness < fitnessLowerBound) return false;
		}
	}
	return true;
}

bool U_secondStageSpawnBounded_noTable(PerlinNoise *oct, const Pos *firstStageChosenCoord, const double firstStageChosenFitness, const double fitnessLowerBound, Pos *chosenCoord, double *chosenFitness) {
	double fitness, bestFitness = firstStageChosenFitness;
	for (double rad = 32.; rad <= 512.; rad += 32.) {
		for (double ang = 0.; ang <= U_TWO_PI; ang += 32./rad) {
			Pos pos = {firstStageChosenCoord->x + (int)(sin(ang) * rad), firstStageChosenCoord->z + (int)(cos(ang) * rad)};
			if (!U_sampleAndGetFitnessBounded(&pos, oct, &bestFitness, &fitness)) continue;
			if (chosenCoord) {
				chosenCoord->x = pos.x;
				chosenCoord->z = pos.z;
			}
			bestFitness = fitness;
			if (*chosenFitness) *chosenFitness = fitness;
			if (bestFitness < fitnessLowerBound) return false;
		}
	}
	return true;
}