#include "lines.hpp"
#include "ellipsoid.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

Lines::Lines() { }

Lines::~Lines() {
    glDeleteBuffers(1, &_buffers[0]);
}

void Lines::init(Camera* camera) {
    _camera = camera;
    
    // Create an arc between two lat/lon points
    glm::vec3 start(glm::radians(82.0f), glm::radians(-39.0f), 1500000.0f);
    start = GeodeticToCart(start);
    glm::vec3 stop(glm::radians(118.0f), glm::radians(-34.0f), 1500000.0f);
    stop = GeodeticToCart(stop);
    glm::vec3 radii(6378137.0f, 6378137.0f, 6356752.314245f);
    glm::vec3 oneOverRadiiSquared(
        1.0f / (radii.x * radii.x),
        1.0f / (radii.y * radii.y),
        1.0f / (radii.z * radii.z));
    
    float granularity = glm::radians(1.0f);
    glm::vec3 normal = glm::normalize(glm::cross(start, stop));
    float theta = std::acos(glm::dot(glm::normalize(start), glm::normalize(stop)));
    int n = std::max((theta / granularity) - 1.0f, 0.0f);
    _curvePositions.push_back(start);
    for (int i = 1; i <= n; i++) {
        float phi = (i * granularity);
        _curvePositions.push_back(glm::rotate(start, phi, normal));
    }
    _curvePositions.push_back(stop);
     
     _shader.init("renderables/shaders/linesV.glsl", "renderables/shaders/linesF.glsl");
     
     // Generate buffers
     glGenBuffers(1, &_buffers[0]);
     // Vertex buffer
     glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
     glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_curvePositions.size(), &_curvePositions[0], GL_STATIC_DRAW);
     
     // Get attribute locations
     _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
     
     // Set the line width
     glLineWidth(3.0f);
     
     _angle = 0.0f;
}

void Lines::render() {
    //~ _angle += 0.2f;
    
    _shader.use();
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    _shader.setMat4("projection", projection);
    
    glm::mat4 view = _camera->getViewMatrix();
    _shader.setMat4("view", view);
    
    glm::mat4 model(1.0f);
    //model = glm::translate(model, _position);
    //~ model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    _shader.setMat4("model", model);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glDrawArrays(GL_LINE_STRIP, 0, _curvePositions.size());
    
    glDisableVertexAttribArray(_vertLoc);
}
