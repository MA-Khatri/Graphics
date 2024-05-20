#pragma once

#include "common.h"
#include "interaction.h"
#include "shapes.h"
#include "utils.h"

std::vector<unsigned char> RayTrace(unsigned int width, unsigned int height, float yfov); /* TODO: scene, also replace with camera class that stores width, height, yfov? */