#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum Sampling_Method {HALTON, SOBOL, SIMPLICAL, NUM_SAMPLING_METHODS};

typedef struct {
	double first, second;
} Pair;

void* shgo(void* func, Pair *bounds, size_t boundsCount, void **args, void **constraints, int n, int iters, int sampling_method) {
	if (sampling_method >= NUM_SAMPLING_METHODS) {
		fprintf(stderr, "shgo: Invalid sampling_method \"%" PRIu8 "\".\n", sampling_method);
		exit(1);
	}
	for (size_t i = 0; i < boundsCount; ++i) {
		if      (bounds[i].first  ==  INFINITY) bounds[i].first  =  1e50;
		else if (bounds[i].first  == -INFINITY) bounds[i].first  = -1e50;
		if      (bounds[i].second ==  INFINITY) bounds[i].second =  1e50;
		else if (bounds[i].second == -INFINITY) bounds[i].second = -1e50;
		if (bounds[i].first > bounds[i].second) {
			fprintf(stderr, "shgo: bounds[%zd] has a lower bound greater than its upper bound.\n", i);
			exit(1);
		}
	}
}



double lerp(double weight, double a, double b) {
	return a + weight * (b - a);
}

double indexedLerp(int index, double a, double b, double c) {
	switch (index) {
		case 0 : return  a + b;
		case 1 : return -a + b;
		case 2 : return  a - b;
		case 3 : return -a - b;
		case 4 : return  a + c;
		case 5 : return -a + c;
		case 6 : return  a - c;
		case 7 : return -a - c;
		case 8 : return  b + c;
		case 9 : return -b + c;
		case 10: return  b - c;
		case 11: return -b - c;
		default: exit(1);
	}
}

double perlin(const double *x, void **args) {
	double *Is = *(double **)args;
	double t1 = x[0] * x[0] * x[0] * (x[0] * (x[0] * 6. - 15.) + 10.);
    double l1 = lerp(t1, indexedLerp(Is[0], x[0], x[1], x[2]), indexedLerp(Is[1], x[0]-1, x[1], x[2]));
    double l3 = lerp(t1, indexedLerp(Is[2], x[0], x[1]-1, x[2]), indexedLerp(Is[3], x[0]-1, x[1]-1, x[2]));
    double l5 = lerp(t1, indexedLerp(Is[4], x[0], x[1], x[2]-1), indexedLerp(Is[5], x[0]-1, x[1], x[2]-1));
    double l7 = lerp(t1, indexedLerp(Is[6], x[0], x[1]-1, x[2]-1), indexedLerp(Is[7], x[0]-1, x[1]-1, x[2]-1));
    double t2 = x[1] * x[1] * x[1] * (x[1] * (x[1] * 6. - 15.) + 10.);
    l1 = lerp(t2, l1, l3);
    l5 = lerp(t2, l5, l7);
    return lerp(x[2] * x[2] * x[2] * (x[2] * (x[2] * 6. - 15.) + 10.), l1, l5);
}