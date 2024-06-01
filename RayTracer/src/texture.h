#pragma once

#include "common.h"
#include "perlin.h"
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
	
	Color Value(double u, double v, const Point3& p) const override;

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

	Color Value(double u, double v, const Point3& p) const override;

private:
	Image image;
};

class NoiseTexture : public Texture
{
public:
	NoiseTexture() {};

	Color Value(double u, double v, const Point3& p) const override;

private:
	Perlin noise;
};

}