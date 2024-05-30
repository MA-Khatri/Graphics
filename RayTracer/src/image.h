#pragma once

#include <string>

namespace rt
{

class Image
{
public:
	~Image();

	Image();
	Image(const char* image_filename);

	bool Load(const std::string& filename);

	int Width() const;
	int Height() const;

	/* Returns the address of the three RGB bytes of the pixel at x, y.
	If there is no image data, returns magenta (255, 0, 255) */
	const unsigned char* PixelData(int x, int y) const;

private:
	const int bytes_per_pixel = 3;
	float* fdata = nullptr; /* Linear floating point pixel data */
	unsigned char* bdata = nullptr; /* Linear 8-bit pixel data */
	int image_width = 0;
	int image_height = 0;
	int bytes_per_scanline = 0;

	/* Return the value clamped to [low, high) */
	static int Clamp(int x, int low, int high);

	static unsigned char FloatToByte(float value);

	/* Convert the linear floating point pixel data to bytes,
	storing the resulting byte data in the `bdata` member */
	void ConvertToBytes();
};

}