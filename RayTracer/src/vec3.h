#pragma once

namespace rt {

class Vec3 {
public:
	double e[3];

	Vec3() : e{ 0,0,0 } {}
	Vec3(double e0) : e{ e0, e0, e0 } {}
	Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	Vec3& operator+=(const Vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	Vec3& operator*=(double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	Vec3& operator/=(double t) {
		return *this *= 1 / t;
	}

	double Length() const {
		return std::sqrt(LengthSquared());
	}

	double LengthSquared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	double NearZero() const
	{
		/* Return true if the vector is close to zero in all dimensions */
		double s = 1e-8;
		return ((std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s));
	}

	static Vec3 Random()
	{
		return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
	}

	static Vec3 Random(double min, double max)
	{
		return Vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
	}
};


/* Point3 is just an alias for Vec3, but useful for geometric clarity in the code. */
using Point3 = Vec3;



/* Vector Utility Functions */
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3& v) {
	return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3& v, double t) {
	return t * v;
}

inline Vec3 operator/(const Vec3& v, double t) {
	return (1 / t) * v;
}

inline bool Equal(const Vec3& u, const Vec3& v)
{
	return ((u.e[0] == v.e[0]) && (u.e[1] == v.e[1]) && (u.e[2] == v.e[2]));
}

inline double Dot(const Vec3& u, const Vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline Vec3 Cross(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 Normalize(const Vec3& v) {
	return v / v.Length();
}

inline Vec3 RandomInUnitDisk()
{
	while (true)
	{
		Vec3 p = Vec3(RandomDouble(-1.0, 1.0), RandomDouble(-1.0, 1.0), 0.0);
		if (p.LengthSquared() < 1) return p;
	}
}

inline Vec3 RandomInUnitSphere()
{
	while (true)
	{
		auto p = Vec3::Random(-1, 1);
		if (p.LengthSquared() < 1) return p;
	}
}

inline Vec3 RandomUnitVector()
{
	return Normalize(RandomInUnitSphere());
}

inline Vec3 RandomOnHemisphere(const Vec3& normal)
{
	Vec3 on_unit_sphere = RandomUnitVector();

	/* In the same hemisphere as the normal? */
	if (Dot(on_unit_sphere, normal) > 0.0) return on_unit_sphere;
	else return -on_unit_sphere;
}

inline Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * Dot(v, n) * n;
}

inline Vec3 Refract(const Vec3& incident_dir, const Vec3& normal, double eta_in_over_out)
{
	double cos_theta = std::fmin(Dot(-incident_dir, normal), 1.0);
	Vec3 r_out_perp = eta_in_over_out * (incident_dir + cos_theta * normal);
	Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.LengthSquared())) * normal;
	return r_out_perp + r_out_parallel;
}

} /* namespace RT */