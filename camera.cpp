#include "camera.hpp"

Camera::Camera(
    float aspect,
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch) {
        
    aspect_ = aspect;
    
    front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    zoom_ = 45.0f;
    nearClip_ = 0.1f;
    farClip_ = 100.0f;
    
    position_ = position;
    worldUp_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(zoom_, aspect_, nearClip_, farClip_);
}

void Camera::zoomToTarget(float radius) {
    float sin = std::sin(glm::radians(zoom_) * 0.5f);
    float distance = radius / sin;
    
    position_ = glm::vec3(distance) * glm::normalize(position_);
    nearClip_ = distance - radius  * 2;
    farClip_ = distance + radius * 2;
}

void Camera::updateCameraVectors() {
    // Calculate the new front vector
    front_ = glm::normalize(glm::vec3(
        std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_)),
        std::sin(glm::radians(pitch_)),
        std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_))));
    // Also recalculate the right and up vectors
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
