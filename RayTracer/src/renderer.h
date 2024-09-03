#pragma once

#include <algorithm>
#include <execution>

#include "common.h"
#include "hittable.h"
#include "material.h"
#include "cameras.h"
#include "scene.h"
#include "pdf.h"


namespace rt 
{

/* Render the provided scene with the provided camera */
std::vector<unsigned char> Render(const Scene& scene, Camera& camera);

/* Trace the given ray through the scene */
Color TraceRay(const Ray& ray_in, int depth, const Scene& scene);

/* Determine the color the provided pixel index given the scene and camera */
void PixelColor(std::vector<unsigned char>& rendered_image, unsigned int i, unsigned int j, const Scene& scene, Camera& camera);

}

