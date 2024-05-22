#pragma once

#include "common.h"

#include "camera.h"
#include "interaction.h"
#include "shapes.h"
#include "utils.h"

namespace RayTracer {

std::vector<unsigned char> RayTrace(Camera* camera); /* TODO: scene, also replace with camera class that stores width, height, yfov? */

} /* namespace RayTracer */