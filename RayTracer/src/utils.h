#pragma once

#include <vector>

namespace rt {

/* Vertically flips provided image */
std::vector<unsigned char> FlipImage(std::vector<unsigned char> image, unsigned int width, unsigned int height);

/* Saves image to PPM file */
int DumpPPM(std::vector<unsigned char> image, unsigned int width, unsigned int height);

} /* namespace rt */