#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Camera attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;
    // Euler angles
    float yaw_;
    float pitch_;
    // Clip space parameters
    float zoom_; // fov
    float aspect_;
    float nearClip_;
    float farClip_;
    
    // Constructor with vectors
    Camera(
        float aspect = 1.0f,
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f);
        
    // Returns view matrix calculated using Euler angles and the lookAt matrix
    glm::mat4 getViewMatrix();
    
    // Returns projection matrix calculated from clip space parameters
    glm::mat4 getProjectionMatrix();
    
    // Move the camera to have a certain object in view
    // Also adjusts the clip space to reasonable values for the new position
    void zoomToTarget(float radius);
    
private:
    // Calculates the front vector from the camera's (updated) Euler angles
    void updateCameraVectors();
};

#endif
