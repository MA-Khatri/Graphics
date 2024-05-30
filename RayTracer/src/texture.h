#pragma once

#include "common.h"
#include "image.h"

namespace rt
{

class Texture
{
public:
	virtual ~Texture() = default;

	virtual Color Value(double u, double v, const Point3& p) const = 0;
};


class SolidColor : public Texture
{
public:
	SolidColor(const Color& albedo) : albedo(albedo) {}
	SolidColor(double r, double g, double b) : albedo(Color(r, g, b)) {}
	
	Color Value(double u, double v, const Point3& p) const override { return albedo; }

private:
	Color albedo;
};


class CheckerTexture : public Texture
{
public:
	CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) 
		: inv_scale(1.0 / scale), even(even), odd(odd) {}
	CheckerTexture(double scale, const Color& c1, const Color& c2)
		: inv_scale(1.0 / scale), even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)) {}

	Color Value(double u, double v, const Point3& p) const override;

private:
	double inv_scale; /* 1 / scale of the texture */
	std::shared_ptr<Texture> even; /* Texture applied to even components of the grid */
	std::shared_ptr<Texture> odd; /* Texture applied to odd components of the grid */
};

class ImageTexture : public Texture
{
public:
	ImageTexture(const char* filename) : image(filename) {}

	Color Value(double u, double v, const Point3& p) const override
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

private:
	Image image;
};

}