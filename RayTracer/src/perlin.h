#pragma once

#include "common.h"

namespace rt
{

class Perlin
{
public:
	Perlin()
	{
		/* Generate `point_count` random Vec3s */
		randvec = new Vec3[point_count];
		for (int i = 0; i < point_count; i++)
		{
			randvec[i] = glm::normalize(RandomVec3(-1,1));
		}

		/* Shuffle (permute) the accessing along each dimension */
		perm_x = PerlinGeneratePermutation();
		perm_y = PerlinGeneratePermutation();
		perm_z = PerlinGeneratePermutation();
	}

	~Perlin()
	{
		delete[] randvec;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
	}

	double Noise(const Point3& p) const
	{
		/* Get the fractional component */
		auto u = p.x - std::floor(p.x);
		auto v = p.y - std::floor(p.y);
		auto w = p.z - std::floor(p.z);

		auto i = int(std::floor(p.x));
		auto j = int(std::floor(p.y));
		auto k = int(std::floor(p.z));
		Vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					/* Using bitwise AND and XOR operators */
					c[di][dj][dk] = randvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}
			}
		}
		
		return PerlinInterpolate(c, u, v, w); /* Note: this can return negative values! */
	}

	double Turbulence(const Point3& p, int depth) const
	{
		double accumulator = 0.0;
		Point3 temp_p = p;
		double weight = 1.0;

		for (int i = 0; i < depth; i++)
		{
			accumulator += weight * Noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return std::fabs(accumulator);
	}

private:
	static const int point_count = 256;
	Vec3* randvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;

	static int* PerlinGeneratePermutation()
	{
		auto p = new int[point_count];

		for (int i = 0; i < point_count; i++)
		{
			p[i] = i;
		}

		Permute(p, point_count);

		return p;
	}

	static void Permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			/* Pick a random index to swap with */
			int target = RandomInt(0, i); /* Should be (0, i) but why so slow?? */

			/* Perform the swap */
			int temp = p[i];
			p[i] = p[target];
			p[target] = temp;
		}
	}

	static double PerlinInterpolate(Vec3 c[2][2][2], double u, double v, double w)
	{
		/* Hermitian smoothing */
		double uu = u * u * (3 - 2 * u);
		double vv = v * v * (3 - 2 * v);
		double ww = w * w * (3 - 2 * w);

		double accumulator = 0.0;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					Vec3 weight_v(u - i, v - j, w - k);
					accumulator += (i * uu + (1 - i) * (1 - uu)) 
								 * (j * vv + (1 - j) * (1 - vv)) 
								 * (k * ww + (1 - k) * (1 - ww)) 
								 * glm::dot(c[i][j][k], weight_v);
				}
			}
		}

		return accumulator;
	}
};

}