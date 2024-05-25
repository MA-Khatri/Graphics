#include "utils.h"

#include <fstream>

namespace rt {

std::vector<unsigned char> FlipImage(std::vector<unsigned char> image, unsigned int width, unsigned int height)
{
	std::vector<unsigned char> flipped;

	for (int j = height; j > 0; j--)
	{
		for (int i = 0; i < (int)width; i++)
		{
			int pixel = 3 * ((j-1) * width + i);
			flipped.push_back(image[pixel + 0]);
			flipped.push_back(image[pixel + 1]);
			flipped.push_back(image[pixel + 2]);
		}
	}

	return flipped;
}

int DumpPPM(std::vector<unsigned char> image, unsigned int width, unsigned int height)
{
	std::ofstream fout("output.ppm");
	if (fout.fail()) return -1;

	fout << "P3\n" << width << ' ' << height << "\n255\n";
	for (int j = 0; j < (int)height; j++) {
		for (int i = 0; i < (int)width; i++) {
			int pixel = 3 * (j * width + i);
			unsigned char r = image[pixel + 0];
			unsigned char g = image[pixel + 1] ;
			unsigned char b = image[pixel + 2];

			fout << r << ' ' << g << ' ' << b << '\n';
		}
	}
	fout.close();

	return 0;
}

} /* namespace rt */