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

const float Inf = std::numeric_limits<float>::infinity();
const float Pi = 3.1415926535897932385;


/* ========================= */
/* === Utility Functions === */
/* ========================= */

/* Converts degrees to radians */
inline float DegreesToRadians(float degrees)
{
	return degrees * Pi / 180.0f;
}

/* Returns a random real (float) in [0, 1) */
inline float RandomFloat()
{
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	thread_local static std::mt19937_64 generator;
	return distribution(generator);
}

/* Returns a random real (float) in [min, max) */
inline float RandomFloat(float min, float max)
{
	static std::uniform_real_distribution<float> distribution(min, max);
	thread_local static std::mt19937_64 generator;
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

} /* namespace rt */

/* ====================== */
/* === Common Headers === */
/* ====================== */
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"