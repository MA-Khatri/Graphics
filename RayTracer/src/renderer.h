#pragma once

#include "common.h"
#include "hittable.h"
#include "material.h"
#include "cameras.h"

#include <algorithm>
#include <execution>

namespace rt 
{

/* Render the provided scene with the provided camera */
std::vector<unsigned char> Render(const Hittable& world, Camera& camera);

/* Trace the given ray through the world */
Color TraceRay(const Ray& ray_in, int depth, const Hittable& world);

/* Determine the color the provided pixel index given the world and camera */
void PixelColor(std::vector<unsigned char>& rendered_image, unsigned int i, unsigned int j, const Hittable& world, Camera& camera);

}

