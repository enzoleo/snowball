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
    Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat _yaw = -90.0f,
           GLfloat _pitch = 0.0f,
           GLfloat _moveSpeed = 3.0f,
           GLfloat _mouseSensitivity = 0.25f,
           GLfloat _fovy = 45.0f)
        : position(_position),
          up(_up),
          yaw(_yaw),
          pitch(_pitch),
          moveSpeed(_moveSpeed),
          mouseSensitivity(_mouseSensitivity),
          fovy(_fovy)
    {
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
    const GLfloat getYaw() { return yaw; }
    const GLfloat getPitch() { return pitch; }
    const GLfloat getMoveSpeed() { return moveSpeed; }
    const GLfloat getMouseSensitivity() { return mouseSensitivity; }
    const GLfloat getFovy() { return fovy; }

    /* Reset some private members */
    void setPosition(const glm::vec3 _position) { position = _position; }
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
    void move(GLfloat deltaTime, CameraMoveDirection direction)
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
        yaw_offset *= mouseSensitivity;
        pitch_offset *= mouseSensitivity;

        yaw   += yaw_offset;
        pitch += pitch_offset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

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

        // Update the other two vectors
        right = glm::normalize(glm::cross(front, up));
        up = glm::normalize(glm::cross(right, front));
    }

    /* The position of camera */
    glm::vec3 position;

    /* The front direction of camera
    ** This member is generated by @yaw and @pitch (Eular Angles)
    ** See the function `genCameraView` */
    glm::vec3 front;

    /* The UP direction
    ** DEFAULT: (0.0f, 1.0f, 0.0f)
    ** Not supposed to be editted in general cases. */
    glm::vec3 up;

    /* The right direction of the camera
    ** Determined by @front and @up directions:
    ** The normalized vector of cross of @front and @up vectors */
    glm::vec3 right;
    
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
