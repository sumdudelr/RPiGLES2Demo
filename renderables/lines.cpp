#include "lines.hpp"
#include "ellipsoid.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

Lines::Lines() { }

Lines::~Lines() {
    glDeleteBuffers(1, &_buffers[0]);
}

void Lines::init(Camera* camera, std::vector<std::vector<glm::vec3>> points) {
    _camera = camera;
    
    _curvePositions = points;
     
     _shader.init("renderables/shaders/linesV.glsl", "renderables/shaders/linesF.glsl");
     
     _buffers.resize(_curvePositions.size());
     // Generate buffers
     glGenBuffers(_buffers.size(), &_buffers[0]);
     for (size_t i = 0; i < _buffers.size(); i++) {
         // Vertex buffer
         glBindBuffer(GL_ARRAY_BUFFER, _buffers[i]);
         glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_curvePositions[i].size(), &_curvePositions[i][0], GL_STATIC_DRAW);
     }
     
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
    
    for (size_t i= 0; i < _buffers.size(); i++) {
        // Position attribute
        glEnableVertexAttribArray(_vertLoc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers[i]);
        glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        
        glDrawArrays(GL_LINE_STRIP, 0, _curvePositions[i].size());
        
        glDisableVertexAttribArray(_vertLoc);
    }
}
