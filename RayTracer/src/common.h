#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>


namespace rt 
{
/* ================= */
/* === Constants === */
/* ================= */

const double Inf = std::numeric_limits<double>::infinity();
const double Pi = 3.1415926535897932385;
const double InvPi = 1.0 / Pi;
const double Eps = 1e-8;


/* ========================= */
/* === Utility Functions === */
/* ========================= */

/* Converts degrees to radians */
inline double DegreesToRadians(double degrees)
{
	return degrees * Pi / 180.0;
}

/* Returns a random real (double) in [0, 1) */
inline double RandomDouble()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	thread_local static std::mt19937_64 generator;
	return distribution(generator);
}

/* Returns a random real (double) in [min, max) */
inline double RandomDouble(double min, double max)
{
	static std::uniform_real_distribution<double> distribution(min, max);
	thread_local static std::mt19937_64 generator;
	return distribution(generator);
}

/* Returns a random integer in [min, max] */
inline int RandomInt(int min, int max)
{
	static std::uniform_int_distribution<int> distribution(min, max);
	thread_local static std::mt19937 generator;
	return distribution(generator);
}

/* Returns the next power of two that is greater than or equal to x */
inline unsigned int RoundUpPower2(unsigned int x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

/* Convert the provided pixel value to be gamma corrected */
inline double LinearToGamma(double linear_component)
{
	if (linear_component > 0.0) return std::sqrt(linear_component);
	return 0.0;
}

} /* namespace rt */

/* ====================== */
/* === Common Headers === */
/* ====================== */
#include "math.h"
#include "ray.h"
#include "interval.h"
