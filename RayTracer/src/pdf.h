#pragma once

#include "common.h"
#include "hittable.h"

#include <vector>

namespace rt
{

/* Probability Distribution Function class */
class PDF
{
public:
	virtual ~PDF() {}

	virtual double Value(const Vec3& direction) const = 0;
	virtual Vec3 Generate() const = 0;
};


class SpherePDF : public PDF
{
public:
	SpherePDF() {}

	double Value(const Vec3& direction) const override
	{
		return 1.0 / (4.0 * Pi);
	}

	Vec3 Generate() const override
	{
		return RandomUnitVector();
	}
};


class CosinePDF : public PDF
{
public:
	CosinePDF(const Vec3& w)
	{
		onb = OrthonormalBasis(w);
	}

	double Value(const Vec3& direction) const override
	{
		double cosine_theta = glm::dot(glm::normalize(direction), onb.w);
		return std::fmax(0.0, cosine_theta / Pi);
	}

	Vec3 Generate() const override
	{
		return onb.Local(RandomCosineDirection());
	}

private:
	OrthonormalBasis onb;
};


class HittablePDF : public PDF
{
public:
	HittablePDF(const Hittable& objects, const Point3& origin)
		: objects(objects), origin(origin) {}


	double Value(const Vec3& direction) const override
	{
		return objects.PDF_Value(origin, direction);
	}

	Vec3 Generate() const override
	{
		return objects.Random(origin);
	}

private:
	const Hittable& objects;
	Point3 origin;
};


class MixturePDF : public PDF
{
public:
	/* Generate a mixture PDF from two PDFs */
	MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1)
	{
		p.clear();
		p.push_back(p0);
		p.push_back(p1);

		length = p.size();
		inv_length = 1.0 / (double)p.size();
	}

	/* Generate a mixture PDF from an arbitrary length vector of PDFs */
	MixturePDF(std::vector<std::shared_ptr<PDF>> p) 
		: p(p) 
	{
		length = p.size();
		inv_length = 1.0 / (double)p.size();
	}


	double Value(const Vec3& direction) const override
	{
		double value = 0.0;
		for (const auto& p_i : p)
		{
			value += inv_length * p_i->Value(direction);
		}
		return value;
	}

	Vec3 Generate() const override
	{
		/* Generate a random int as in index into the PDFs */
		int index = (int)(length * RandomDouble()); /* For some reason, random int is not working?? :( */
		return p[index]->Generate();
	}

private:
	std::vector<std::shared_ptr<PDF>> p;
	int length;
	double inv_length;
};


}