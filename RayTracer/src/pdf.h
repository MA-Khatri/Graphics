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
	HittablePDF(Hittable& objects, const Point3& origin)
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
	Hittable& objects;
	Point3 origin;
};


class MixturePDF : public PDF
{
public:
	/* Generate a mixture PDF from two PDFs */
	MixturePDF(std::shared_ptr<PDF> pdf0, std::shared_ptr<PDF> pdf1)
	{
		pdfs.push_back(pdf0);
		pdfs.push_back(pdf1);

		length = (int)pdfs.size();
		inv_length = 1.0 / (double)pdfs.size();
	}

	/* Generate a mixture PDF from an arbitrary length vector of PDFs */
	MixturePDF(std::vector<std::shared_ptr<PDF>> pdfs) 
		: pdfs(pdfs) 
	{
		length = (int)pdfs.size();
		inv_length = 1.0 / (double)pdfs.size();
	}


	double Value(const Vec3& direction) const override
	{
		double value = 0.0;
		for (const auto& pdf : pdfs)
		{
			value += inv_length * pdf->Value(direction);
		}
		return value;
	}

	Vec3 Generate() const override
	{
		int index = (int)(RandomDouble() * length);
		return pdfs[index]->Generate();
	}

private:
	std::vector<std::shared_ptr<PDF>> pdfs;
	int length;
	double inv_length;
};


}