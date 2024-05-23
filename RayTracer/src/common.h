#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>


namespace RayTracer {

/* C++ Std Usings */
using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

/* Constants */
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;


/* Utility Functions */
inline double DegreesToRadians(double degrees)
{
	return degrees * pi / 180.0;
}

inline double RandomDouble()
{
	/* Returns a random real in [0, 1) */
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	thread_local static std::mt19937_64 generator;
	return distribution(generator);
}

inline double RandomDouble(double min, double max)
{
	/* Returns a random real in [min, max) */
	static std::uniform_real_distribution<double> distribution(min, max);
	thread_local static std::mt19937_64 generator;
	return distribution(generator);
}

inline unsigned int RoundUpPower2(unsigned int x)
{
	/* Returns the next power of two that is greater than or equal to x */
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

} /* namespace RayTracer */

/* Common Headers */
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"