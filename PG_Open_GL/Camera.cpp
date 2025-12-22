#include "Camera.hpp"

namespace gps
{
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, cameraUp));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));
    }

    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed, bool godMode) {
        
        switch (direction) {
        case MOVE_FORWARD:
            if (!godMode)
            {
                glm::vec3 horizontalFront = glm::normalize(glm::vec3(cameraFrontDirection.x, 0.0f, cameraFrontDirection.z));
                cameraPosition += horizontalFront * speed;
                break;
            }
            else {
                glm::vec3 front = glm::normalize(cameraFrontDirection);
                cameraPosition += front * speed;
                break;
            }
        case MOVE_BACKWARD:
            if (!godMode)
            {
                glm::vec3 horizontalBack = glm::normalize(glm::vec3(cameraFrontDirection.x, 0.0f, cameraFrontDirection.z));
                cameraPosition -= horizontalBack * speed;
                break;
            }
            else {
                glm::vec3 front = glm::normalize(cameraFrontDirection);
                cameraPosition -= front * speed;
                break;
            }
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        }
        cameraTarget = cameraPosition + cameraFrontDirection;
    }
   
    void Camera::rotate(float pitch, float yaw)
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);

        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}