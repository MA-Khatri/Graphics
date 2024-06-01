#include "texture.h"

namespace rt
{

Color SolidColor::Value(double u, double v, const Point3& p) const
{
	return albedo;
}


Color CheckerTexture::Value(double u, double v, const Point3& p) const
{
	auto xInt = int(std::floor(inv_scale * p.x));
	auto yInt = int(std::floor(inv_scale * p.y));
	auto zInt = int(std::floor(inv_scale * p.z));

	bool isEven = (xInt + yInt + zInt) % 2 == 0;

	return isEven ? even->Value(u, v, p) : odd->Value(u, v, p);
}


Color ImageTexture::Value(double u, double v, const Point3& p) const
{
	/* If we have no texture data, then return solid cyan as a debugging aid */
	if (image.Height() <= 0) return Color(0.0, 1.0, 1.0);

	/* Clamp input texture coords to [0,1] x [0,1] */
	u = Interval(0.0, 1.0).Clamp(u);
	v = 1.0 - Interval(0.0, 1.0).Clamp(v); /* Flip V to image coords */

	/* Convert the UV coords to image pixel coords and get corresponding pixel data */
	auto i = int(u * image.Width());
	auto j = int(v * image.Height());
	auto pixel = image.PixelData(i, j);

	double color_scale = 1.0 / 255.0;
	return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}


Color NoiseTexture::Value(double u, double v, const Point3& p) const
{
	//return Color(1.0, 1.0, 1.0);
	return Color(1.0, 1.0, 1.0) * noise.Noise(p);
}

}