#pragma once

#include "common.h"

namespace rt
{

class Transform
{
public:
	/* Stores the model matrix of the Object. */
	Mat4 model_to_world = Mat4(1.0);

	/* Stores the world to model transformation matrix. The ray's origin and
	direction will be multiplied by this matrix to transform it to model space. 
	It is the inverse of the model matrix */
	Mat4 world_to_model = Mat4(1.0);

	/* Stores the inverse transpose of the model matrix which is used to update surface normals. 
	This is so that normals for non-uniform scaling stay perpendicular to the surface of
	the object. */
	Mat4 normal_to_world = Mat4(1.0);

public:
	Transform() {}
	Transform(Mat4 model_to_world) : model_to_world(model_to_world), world_to_model(glm::inverse(model_to_world)), normal_to_world(glm::inverseTranspose(model_to_world)) {}

	/* Transform ray from world space to model space */
	Ray WorldToModel(const Ray& world_ray) const
	{
		Point3 origin = world_to_model * Vec4(world_ray.origin, 1.0);
		Vec3 direction = world_to_model * Vec4(world_ray.direction, 0.0);
		return Ray(origin, direction, world_ray.time);
	}

	Ray ModelToWorld(const Ray& model_ray) const
	{
		Point3 origin = model_to_world * Vec4(model_ray.origin, 1.0);
		Vec3 direction = model_to_world * Vec4(model_ray.direction, 0.0);
		return Ray(origin, direction, model_ray.time);
	}

	/* Transform the normal from model space to world space */
	Vec3 GetWorldNormal(const Vec3& model_normal) const
	{
		return normal_to_world * Vec4(model_normal, 0.0);
	}

	void SetIdentity()
	{
		model_to_world = Mat4(1.0);
		world_to_model = Mat4(1.0);
		normal_to_world = Mat4(1.0);
	}

	void Translate(const Vec3& v)
	{
		model_to_world *= glm::translate(v);
		UpdateMatrices();
	}

	void Translate(double x, double y, double z)
	{
		model_to_world *= glm::translate(Vec3(x, y, z));
		UpdateMatrices();
	}

	void Scale(const Vec3& s)
	{
		model_to_world *= glm::scale(s);
		UpdateMatrices();
	}

	void Scale(double x, double y, double z)
	{
		model_to_world *= glm::scale(Vec3(x, y, z));
		UpdateMatrices();
	}

	void Scale(double s)
	{
		model_to_world *= glm::scale(Vec3(s));
		UpdateMatrices();
	}

	void Rotate(double deg, const Vec3& axis)
	{
		double rad = glm::radians(deg);
		model_to_world *= glm::rotate(rad, axis);
		UpdateMatrices();
	}

private:
	/* Update the world_to_model and normal_to_world matrices based on current model_to_world */
	inline void UpdateMatrices()
	{
		world_to_model = glm::inverse(model_to_world);
		normal_to_world = glm::transpose(world_to_model); /* i.e., inverse transpose of model_to_world */

		//normal_to_world = model_to_world;

		//normal_to_world[0][3] = 0.0;
		//normal_to_world[1][3] = 0.0;
		//normal_to_world[2][3] = 0.0;

		//normal_to_world[3][0] = 0.0;
		//normal_to_world[3][1] = 0.0;
		//normal_to_world[3][2] = 0.0;

		//normal_to_world[3][3] = 1.0;

		//normal_to_world = glm::inverseTranspose(normal_to_world);
	}

};

} /* namespace rt */