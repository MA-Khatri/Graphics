#include "texture.h"

namespace rt
{

Color CheckerTexture::Value(double u, double v, const Point3& p) const
{
	auto xInt = int(std::floor(inv_scale * p.x));
	auto yInt = int(std::floor(inv_scale * p.y));
	auto zInt = int(std::floor(inv_scale * p.z));

	bool isEven = (xInt + yInt + zInt) % 2 == 0;

	return isEven ? even->Value(u, v, p) : odd->Value(u, v, p);
}

}