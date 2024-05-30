#include "image.h"

#include <iostream>

/* This should be necessary? but its already defined in the parent OpenGL project */
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif

#ifndef STBI_FAILURE_USERMSG
#define STBI_FAILURE_USERMSG
#endif

#include "stb_image/stb_image.h"

namespace rt
{
Image::~Image()
{
	delete[] bdata;
	//STBI_FREE(fdata);
	free(fdata);
}

Image::Image(const char* image_filename)
{
	/* Search for and load in the provided image filename */
	auto filename = std::string(image_filename);

	/* Most likely location... */
	/* Note: this is hard-coded! Should replace this with a better solution */
	if (Load("../RayTracer/res/images/" + filename)) return;

	//if (Load(filename)) return;
	//if (Load("images/" + filename)) return;
	//if (Load("../images/" + filename)) return;
	//if (Load("../../images/" + filename)) return;
	//if (Load("../../../images/" + filename)) return;
	//if (Load("../../../../images/" + filename)) return;
	//if (Load("../../../../../images/" + filename)) return;
	//if (Load("../../../../../../images/" + filename)) return;

	//if (Load(filename)) return;
	//if (Load("res/images/" + filename)) return;
	//if (Load("../res/images/" + filename)) return;
	//if (Load("../../res/images/" + filename)) return;
	//if (Load("../../../res/images/" + filename)) return;
	//if (Load("../../../../res/images/" + filename)) return;
	//if (Load("../../../../../res/images/" + filename)) return;
	//if (Load("../../../../../../res/images/" + filename)) return;

	std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
}

Image::Image()
{

}

bool Image::Load(const std::string& filename)
{
	/* Loads the linear (gamma=1) image data from the given filename.
	Returns true if load succeeded. Pixels are stored as floats in [0.0f, 1.0f]
	in a contiguous array. */

	auto n = bytes_per_pixel; /* Dummy out parameter: original components per pixel */
	fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);

	if (fdata == nullptr) return false;

	bytes_per_scanline = image_width * bytes_per_pixel;
	ConvertToBytes();
	return true;
}

int Image::Width() const
{
	return (fdata == nullptr) ? 0 : image_width;
}

int Image::Height() const
{
	return (fdata == nullptr) ? 0 : image_height;
}

const unsigned char* Image::PixelData(int x, int y) const
{
	static unsigned char magenta[] = { 255, 0, 255 };
	if (bdata == nullptr) return magenta;

	x = Clamp(x, 0, image_width);
	y = Clamp(y, 0, image_height);

	return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
}

int Image::Clamp(int x, int low, int high)
{
	if (x < low) return low;
	if (x < high) return x;
	return high - 1;
}

unsigned char Image::FloatToByte(float value)
{
	if (value <= 0.0) return 0;
	if (1.0 <= value) return 255;
	return static_cast<unsigned char>(256.0 * value);
}

void Image::ConvertToBytes()
{
	int total_bytes = image_width * image_height * bytes_per_pixel;
	bdata = new unsigned char[total_bytes];

	/* Iterate through all pixels, converting from [0.0f, 1.0f] to [0, 255] bytes */
	auto* bptr = bdata;
	auto* fptr = fdata;
	for (int i = 0; i < total_bytes; i++, fptr++, bptr++)
	{
		*bptr = FloatToByte(*fptr);
	}
}

}