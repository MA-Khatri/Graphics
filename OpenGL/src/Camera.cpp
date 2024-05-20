#include "camera.h"

Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
	Camera::orientation = orientation;
	Camera::up = up;
}

Camera::~Camera()
{

}

void Camera::Update(float FOVdeg, float nearPlane, float farPlane, int inWidth, int inHeight)
{
	// Initialize the view and projection matrices to the identity matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	width = inWidth;
	height = inHeight;

	// Set the view and projection matrices using the lookAt and perspective glm functions
	view = glm::lookAt(position, position + orientation, up);

	//if (width >= height) projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);
	//else glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);
	projection = glm::perspective(glm::radians(FOVdeg), (float)(width) / float(height), nearPlane, farPlane);

	matrix = projection * view;
}

void Camera::Inputs(GLFWwindow* window)
{
	// WASD keys for basic motion front/back, strafe left/right
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}

	// SPACE/CTRL moves up/down along up vector
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += speed * -up;
	}

	// Holding down shift increases speed
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.2f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.05f;
	}

	// Mouse drag for orientation
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hide the cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Check if pressed
		if (!pressed)
		{
			glfwSetCursorPos(window, width / 2, height / 2);
			pressed = true;
		}

		// Get mouse position
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Mouse drag amounts for rotation
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Get new orientation for the camera
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Bound the up/down tilt between -5 to 5 radians
		if (!(glm::angle(newOrientation, up) <= glm::radians(5.0f) or glm::angle(newOrientation, -up) <= glm::radians(5.0f)))
		{
			orientation = newOrientation;
		}

		// Right/Left rotate (allowed to fully spin around)
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		glfwSetCursorPos(window, width / 2, height / 2);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		pressed = false;
	}
}