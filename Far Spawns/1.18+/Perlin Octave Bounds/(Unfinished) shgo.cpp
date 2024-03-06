#include <array>
#include <cinttypes>
#include <cstdlib>
#include <utility>
#include <vector>

enum Sampling_Method {HALTON, SOBOL, SIMPLICAL};

template <typename T>
void* shgo(void* func, std::vector<std::pair<double, double>> bounds, void **args, void **constraints = nullptr, int *n = nullptr, int iters = 1, ) {

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

double perlin(const std::array<double, 3> x, void **args) {
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