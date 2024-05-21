#pragma once

#include "common.h"

#include "camera.h"
#include "interaction.h"
#include "shapes.h"
#include "utils.h"

namespace RayTracer {

std::vector<unsigned char> RayTrace(unsigned int width, unsigned int height, float yfov); /* TODO: scene, also replace with camera class that stores width, height, yfov? */

} /* namespace RayTracer */