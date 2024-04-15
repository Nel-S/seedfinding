#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// The original hash code for testing purposes.
int64_t hashCode(int32_t x, int32_t y, int32_t z) {
	int64_t l = ((int64_t)(x * 3129871)) ^ ((int64_t)z * 116129781LL) ^ ((int64_t)y);
	l = l * l * 42317861LL + l * 11LL;
	return l >> 16;
}

// Returns gcd(a, 2^n).
uint64_t gcd(int64_t a, uint8_t n) {
	uint64_t count = 0;
	for (uint64_t or = a | (1ULL << n); !(or & 1); or >> 1) ++count;
	return 1ULL << count;
}

// Returns a four-element array containing the integers x satisfying x^2 = a (mod 2^n) for n >= 3, or NULL if no such integers exist.
int64_t *lift(int64_t a, uint8_t n) {
	if (n < 3) {
		printf("ERROR: lift() does not support values of n less than 3 (received %d).\n", n);
		exit(1);
	}
	// If a != 1 (mod 8), no solution for i exists
	if (a & 0b111 != 1) return NULL;

	static const int64_t nEquals2Solutions[] = {1, 3}; // i's satisfying i^2 = a = 1 (mod 2^2)
	int64_t values[2 * sizeof(nEquals2Solutions)/sizeof(*nEquals2Solutions)]; // Might need to add padding zeroes
	for (int i = 0; i < sizeof(nEquals2Solutions)/sizeof(*nEquals2Solutions); ++i) {
		values[i] = nEquals2Solutions[i];
	}
	// First two solutions are obtained by progressively solving i^2 = a (mod 2^k), which is done by adding 2^(k - 2) if (a - values[i]^2)/2^(k - 1) (which is guaranteed to be an integer since i^2 = a (mod 2^(k - 1)) -> 2^(k - 1) divides a - i^2) is odd or doing nothing otherwise
	for (int k = 4; k <= n; ++k) {
		for (int i = 0; i < sizeof(values)/sizeof(*values); ++i) {
			values[i] += (((a - values[i] * values[i]) >> (k - 1)) & 1) * (1ULL << (k - 2));
		}
	}
	// Last two solutions, meanwhile, are the first two solutions plus 2^(n - 1)
	for (int i = 0; i < sizeof(values)/sizeof(*values)/2; ++i) {
		values[i + sizeof(values)/sizeof(*values)/2] = values[i] + (1ULL << (n - 1));
	}
	return values;
}

// Returns an array containing the integers x satisfying ax^2 + bx + c = 0 (mod 2^n), or NULL if no such integers exist.
size_t *quad(int64_t a, int64_t b, int64_t c, uint8_t n, int64_t *values) {
	// Attempts to solve b^2 - 4ac = x^2 (mod 2^n) for x
	int64_t *rads = lift(b * b - 4 * a * c, n);
	// If no such modular roots exist, no overall solutions exist
	if (!rads) return NULL;
	for (int i = 0; i < 4; ++i) {
		// Solve 2ax = -b +- rads[i] (mod 2^n) -> ax = (rads[i] - b)/2 (mod 2^(n - 1))
		// If gcd(2a, 2^n) cannot divide -b + rads[i] (i.e. -b + rads[i] != 0 (mod gcd(2a, 2^n))), the current root has no associated solution
		int64_t fullRad = rads[i] - b;
		// if (fullRad & (gcd(2 * a, n) - 1)) continue;
	}
}

size_t inverseHashCode(int64_t hash, char known1, int32_t known1Value, char known2, int32_t known2Value, int64_t *output, size_t outputCapacity) {
	if (known1 == known2) {
		printf("ERROR: inverseHashCode has identical coordinate types for 'known1' (%c) and 'known2' (%c).\n", known1, known2);
		exit(1);
	}

	size_t currentAnswer = 0;
	hash <<= 16;
	// Iterates over the possible lower bits of hash.
	for (int i = 0; i < 1ULL << 3; i += 2) { // l must be even in order for 42317861*l^2 + 11*l = l_new (mod 2^64) to have a solution
		// Restores hash
		int64_t l = hash | i;
		// Then solves x^2 = b^2 - 4al (mod 2^64) for x (for this hash x is guarenteed to have 4 solutions because l's evenness means b^2 - 4al = 1 (mod 8); if it didn't equal 1 modulo 8, no solutions for x would exist)
		int64_t values[] = {1, 3}; // x's satisfying x^2 = 1 (mod 2^2)
		for (int i = 0; i < sizeof(values)/sizeof(*values); ++i) {
			// The first two solutions are obtained by progressively solving x^2 = a (mod 2^k) for each k, which is done by adding 2^(k - 2) if (a - values[i]^2)/2^(k - 1) (which is guaranteed to be an integer since x^2 = a (mod 2^(k - 1)) -> 2^(k - 1) divides a - x^2) is odd, or doing nothing otherwise
			for (int k = 4; k <= 64; ++k) {
				values[i] += (((11 * 11 + 4 * (42317861 * l) - values[i] * values[i]) >> (k - 1)) & 1) * (1ULL << (k - 2));
			}
			values[i] -= 11; // Since values[i] consists of adding powers of 2 to originally-odd numbers, this is guarenteed to be even.
			// The last two solutions, meanwhile, are the first two solutions plus 2^(n - 1).
			for (uint64_t j = 0; j <= 1; ++j) {
				// Then since gcd(2*42317861, 2^64) = 2 and fullRad is guarenteed to be even, we divide our original equation 2ax = fullRad (mod 2^64) by 2, giving ax = fullRad/2 (mod 2^63). This has the unique solution of fullRad/2 multiplied by the multiplicative inverse of 42317861 modulo 2^63, 1871464906053100461.
				int64_t a = 1871464906053100461 * (values[i] + (j << 63))/2;
				for (uint64_t j = 0; j <= 1; ++j) {
					int64_t fullA = a + (j << 63);
					// Currently the logic above produces two correct answers and six incorrect ones for every value. Until I get that figured out, here's a check to eliminate the incorrect values.
					if (l != fullA * fullA * 42317861LL + fullA * 11LL) continue;
					// printf("(%" PRId64 ", %" PRId64 " = %" PRId64 ")\n", fullA, l, fullA * fullA * 42317861LL + fullA * 11LL);

					switch (known1) {
					case 'x':
						fullA ^= ((int64_t)(known1Value * 3129871));
						break;
					case 'y':
						fullA ^= (int64_t)known1Value;
						// printf("(%" PRId64 ")\n", fullA);
						break;
					case 'z':
						fullA ^= ((int64_t)known1Value * 116129781LL);
						break;
					default:
						printf("ERROR: inverseHashCode received invalid 'known1' parameter (%c).", known1);
						exit(1);
					}
					
					switch (known2) {
					case 'x':
						fullA ^= ((int64_t)(known2Value * 3129871));
						break;
					case 'y':
						fullA ^= (int64_t)known2Value;
						break;
					case 'z':
						fullA ^= ((int64_t)known2Value * 116129781LL);
						// printf("(%" PRId64 ")\n", fullA);
						break;
					default:
						printf("ERROR: inverseHashCode received invalid 'known2' parameter (%c).", known2);
						exit(1);
					}
					output[currentAnswer] = fullA;
					++currentAnswer;
					if (currentAnswer >= outputCapacity) return currentAnswer;
				}
			}
		}
	}
	return currentAnswer;
}

int main() {
	int32_t x = 10, y = 0, z = 27;
	int64_t hash = hashCode(x, y, z);

	int64_t array[65536];
	int count = inverseHashCode(hash, 'y', y, 'z', z, array, sizeof(array)/sizeof(*array));
	for (size_t i = 0; i < count; ++i) {
		// printf("x = %" PRId64 " (%" PRId64 " vs. %" PRId64 ")\n", array[i], hash, hashCode(array[i], y, z));
		if (array[i] == x) {
			printf("x = %" PRId64 " (%" PRId64 " vs. %" PRId64 ")\n", array[i], hash, hashCode(array[i], y, z));
		}
	}
	return 0;
}