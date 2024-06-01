#pragma once

#include "common.h"

namespace rt
{

class Perlin
{
public:
	Perlin()
	{
		/* Generate `point_count` random floats (doubles in this case) */
		randfloat = new double[point_count];
		for (int i = 0; i < point_count; i++)
		{
			randfloat[i] = RandomDouble();
		}

		/* Shuffle (permute) the accessing along each dimension */
		perm_x = PerlinGeneratePermutation();
		perm_y = PerlinGeneratePermutation();
		perm_z = PerlinGeneratePermutation();
	}

	~Perlin()
	{
		delete[] randfloat;
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

		/* Hermitian smoothing */
		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		auto i = int(std::floor(p.x));
		auto j = int(std::floor(p.y));
		auto k = int(std::floor(p.z));
		double c[2][2][2];

		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					/* Using bitwise AND and XOR operators */
					c[di][dj][dk] = randfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}
			}
		}
		
		return TrilinearInterpolate(c, u, v, w);
	}

private:
	static const int point_count = 256;
	double* randfloat;
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

	static double TrilinearInterpolate(double c[2][2][2], double u, double v, double w)
	{
		double accumulator = 0.0;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					accumulator += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) * c[i][j][k];
				}
			}
		}

		return accumulator;
	}
};

}