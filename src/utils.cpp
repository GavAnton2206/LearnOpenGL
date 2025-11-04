#include "utils.h"

#include <random>

double random(float randomMax) {
	return randomMax * (double)rand() / (double)RAND_MAX;
}