/*
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 13.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;  //0 <= yaw < 2 * pi
	GLfloat Pitch;  //-pi/2 <= pitch <= pi/2 
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat yaw = YAW, GLfloat pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, 
		GLfloat upX, GLfloat upY, GLfloat upZ, 
		GLfloat yaw, GLfloat pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (direction == FORWARD)
			this->Position += this->Front * velocity;
		if (direction == BACKWARD)
			this->Position -= this->Front * velocity;
		if (direction == LEFT)
			this->Position -= this->Right * velocity;
		if (direction == RIGHT)
			this->Position += this->Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}


private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};
*/

/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMoveDirection
{
	CAMERA_FORWARD,
	CAMERA_BACKWARD,
	CAMERA_LEFT,
	CAMERA_RIGHT
};

/* CLASS: camera */
class Camera
{
public:

	/****************
	** Default constructor & Constructor
	**
	** Notice that the parameters @yaw and @pitch are ANGLE variables.
	** @param `yaw` determines the horizontal offset of the camera direction
	** @param `pitch` determines the vertical offset of the camera direction
	**
	** Therefore, these two parameters define the front direction of camera, The calculation
	** of this front direction is implemented in the function `genCameraView`.
	** DEFAULT: the front direction is (0.0f, 0.0f, -1.0f).
	******************/
	Camera(const glm::vec3& _position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3& _up = glm::vec3(0.0f, 1.0f, 0.0f),
		const GLfloat _yaw = -90.0f,
		const GLfloat _pitch = 0.0f,
		const GLfloat _moveSpeed = 5.0f,
		const GLfloat _mouseSensitivity = 0.25f,
		const GLfloat _fovy = 45.0f)
		: position(_position),
		up(_up),
		yaw(_yaw),
		pitch(_pitch),
		moveSpeed(_moveSpeed),
		mouseSensitivity(_mouseSensitivity),
		fovy(_fovy)
	{
		this->worldUp = up;
		// Do initialization when constructing
		genCameraView();
	}

	/* Returns the private members
	** We set the camera settings private, because they are not supposed to be editted easily
	** If they need to be editted, call the `set*` functions, which makes sure you edit them
	** on purpose, instead of unconsciously. */
	const glm::mat4 getViewMat() { return glm::lookAt(position, position + front, up); }
	const glm::vec3 getPosition() { return position; }
	const glm::vec3 getFront() { return front; }
	const glm::vec3 getUp() { return up; }
	const glm::vec3 getRight() { return right; }
	const glm::vec3 getWorldUp() { return worldUp; }
	const GLfloat getYaw() { return yaw; }
	const GLfloat getPitch() { return pitch; }
	const GLfloat getMoveSpeed() { return moveSpeed; }
	const GLfloat getMouseSensitivity() { return mouseSensitivity; }
	const GLfloat getFovy() { return fovy; }
	

	/* Reset some private members */
	void setWorldUp(const glm::vec3& _worldUp) { worldUp = _worldUp; }
	void setPosition(const glm::vec3& _position) { position = _position; }
	void setMoveSpeed(const GLfloat _moveSpeed) { moveSpeed = _moveSpeed; }
	void setMouseSensitivity(const GLfloat _mouseSensitivity) { mouseSensitivity = _mouseSensitivity; }
	void setFovy(const GLfloat _fovy) { fovy = _fovy; }
	void setYawPitch(const GLfloat _yaw,
		const GLfloat _pitch)
	{
		yaw = _yaw;
		pitch = _pitch;
		genCameraView();
	}


	/* Move camera along the given direction */
	void translate(GLfloat deltaTime, CameraMoveDirection direction)
	{
		// Get the velocity of the movement
		GLfloat velocity = moveSpeed * deltaTime;
		if (direction == CAMERA_FORWARD) position += front * velocity;
		if (direction == CAMERA_BACKWARD) position -= front * velocity;
		if (direction == CAMERA_LEFT) position -= right * velocity;
		if (direction == CAMERA_RIGHT) position += right * velocity;
	}

	/* Rotate the camera
	** Change the front direction by editting the Eular angles */
	void rotate(GLfloat yaw_offset, GLfloat pitch_offset)
	{
		// Change the Eular angles of this camera configuration
		yaw += yaw_offset * mouseSensitivity;
		pitch += pitch_offset * mouseSensitivity;

		// update the pitch! Make sure the pitch is in right area
		// Attention: the value of @yaw has no limitation!
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		// Regenerate the view settings
		genCameraView();
	}

	/* Mouse scroll-wheel event.
	** Zoom the current canvas by changing the fovy */
	void zoom(GLfloat fovy_offset)
	{
		// Note that the fovy cannot be too small or too big
		if (fovy >= 1.0f && fovy <= 45.0f) fovy -= fovy_offset;
		if (fovy <= 1.0f) fovy = 1.0f;
		if (fovy >= 45.0f) fovy = 45.0f;
	}




private:

	/* PRIVATE MEMBER:
	** This function generates the view vectors of the camera
	** @param front: The front direction of the camera
	** @param up: The world up direction of the camera
	** @param right: The right direction of the camera */
	void genCameraView()
	{
		// Calculate the new front vector with given value of yaw and pitch
		glm::vec3 new_front;
		new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		new_front.y = sin(glm::radians(pitch));
		new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(new_front);

		// update the other two vectors
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	/* The position of camera */
	glm::vec3 position;

	/* The front direction (camera space)
	** This member is generated by @yaw and @pitch (Eular Angles)
	** See the function `genCameraView` */
	glm::vec3 front;

	/* The UP direction (camera space)
	** DEFAULT: (0.0f, 1.0f, 0.0f)
	** Not supposed to be editted in general cases. */
	glm::vec3 up;

	/* The right direction (camera space)
	** Determined by @front and @up directions:
	** The normalized vector of cross of @front and @up vectors */
	glm::vec3 right;

	/* The up vector in world space, this vector is used to compute "right" mentioned above 
	** This vector is not "up" mentioned above */
	glm::vec3 worldUp;


	/* EULAR ANGLES
	** @param `yaw` determines the horizontal offset of the camera direction
	** @param `pitch` determines the vertical offset of the camera direction
	** These two Eular angles determines the front direction @front */
	GLfloat yaw, pitch;

	/* The speed of the movement */
	GLfloat moveSpeed;

	/* The sensitivity of mouse */
	GLfloat mouseSensitivity;

	/* The fovy angle of camera
	** Specifies the field of view angle, in degrees, in the y direction */
	GLfloat fovy;


};

#endif
