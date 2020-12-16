#include "camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(
    float width,
    float height,
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch) {
        
    width_ = width;
    height_ = height;
    aspect_ = width_ / height_;
    
    front_ = glm::vec3(0.0f, 0.0f, 0.0f);
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
    return glm::lookAt(position_, front_, up_);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(zoom_), aspect_, nearClip_, farClip_);
}

glm::mat4 Camera::getOrthoMatrix() {
    return glm::ortho(-width_ / 2.0f, width_ / 2.0f, -height_ / 2.0f, height_ / 2.0f);
}

void Camera::zoomToTarget(float radius) {
    float sin = std::sin(glm::radians(zoom_) * 0.5f);
    float hypotDistance = radius / sin;
    position_ = glm::vec3(hypotDistance) * glm::normalize(position_);
    
    float tan = std::tan(glm::radians(zoom_) * 0.5f);
    float distance = radius / tan;
    nearClip_ = distance - (radius * 2);
    farClip_ = distance + (radius * 2);
}

void Camera::rotateAroundZ(float angle) {
    position_ = glm::rotateZ(position_, glm::radians(angle));
}

glm::vec2 Camera::getScreenPoint(glm::vec3 point) {
    // The clip-space coordinates as would be calculated in the vertex shader
    glm::vec4 p(point, 1.0f);
    glm::mat4 model(1.0f);
    p = getProjectionMatrix() * getViewMatrix() * model * p;
    
    // Perform clipping
    p = p / glm::vec4(p.w);
    return glm::vec2(p.x * width_ / 2.0f, p.y * height_ / 2.0f);
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
