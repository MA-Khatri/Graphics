#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shader.h"
#include "gl_utils.h"

class Camera
{
private:
	/* Width and Height of the viewport / window */
	int width;
	int height;

	/* Is the left mouse key pressed? */ 
	bool pressed = false;

	/* Camera movement speed */
	float speed = 0.01f;
	/* Camera rotation sensitivity */
	float sensitivity = 100.0f;

public:
	/* The position of the camera */
	glm::vec3 position;
	/* The direction the camera is looking.Internally, the lookAt point is position + this orientation */ 
	glm::vec3 orientation;
	/* The up vector(think orientation of the viewport) */
	glm::vec3 up;
	/* The camera matrix (initialized to identity matrix) -- will store the combined view-projection matrix */
	glm::mat4 matrix = glm::mat4(1.0f);
	/* The matrix representing the camera's position and orientation */
	glm::mat4 view_matrix = glm::mat4(1.0f);
	/* The matrix representing the projection of the camera */
	glm::mat4 projection_matrix = glm::mat4(1.0f);
	/* The vertical field of view (in degrees) */
	float vfov;

public:
	Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up);
	~Camera();

	/* Updates the camera matrix */ 
	void Update(float FOVdeg, float nearPlane, float farPlane, int inWidth, int inHeight);

	/* Handles camera movement inputs */
	void Inputs(GLFWwindow* window);
};