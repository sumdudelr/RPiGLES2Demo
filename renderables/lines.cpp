#include "lines.hpp"
#include "ellipsoid.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

Lines::Lines() { }

Lines::~Lines() {
    glDeleteBuffers(1, &_buffers[0]);
}

void Lines::init(Camera* camera, std::vector<glm::vec3> points) {
    _camera = camera;
    
    _curvePositions = points;
     
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
}

void Lines::render() {
    _shader.use();
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    _shader.setMat4("projection", projection);
    
    glm::mat4 view = _camera->getViewMatrix();
    _shader.setMat4("view", view);
    
    glm::mat4 model(1.0f);
    _shader.setMat4("model", model);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glDrawArrays(GL_LINE_STRIP, 0, _curvePositions.size());
    
    glDisableVertexAttribArray(_vertLoc);
}
