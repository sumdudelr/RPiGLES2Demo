#include "lines.hpp"
#include "ellipsoid.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

Lines::Lines() { }

Lines::~Lines() {
    glDeleteBuffers(1, &_buffers[0]);
}

void Lines::init(Camera* camera, std::vector<Line> lines) {
    _camera = camera;
    
    _curvePositions = lines;
     
     _shader.init("renderables/shaders/linesV.glsl", "renderables/shaders/linesF.glsl");
     
     _buffers.resize(_curvePositions.size());
     // Generate buffers
     glGenBuffers(_buffers.size(), &_buffers[0]);
     for (size_t i = 0; i < _buffers.size(); i++) {
         // Vertex buffer
         glBindBuffer(GL_ARRAY_BUFFER, _buffers[i]);
         glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_curvePositions[i].points.size(), &_curvePositions[i].points[0], GL_STATIC_DRAW);
     }
     
     // Get attribute locations
     _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
}

void Lines::render() {
    _shader.use();
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    glm::mat4 view = _camera->getViewMatrix();
    glm::mat4 mvp = projection * view;
    _shader.setMat4("mvp", mvp);
    
    for (size_t i= 0; i < _buffers.size(); i++) {
        // Set color
        _shader.setVec3("color", _curvePositions[i].color);
        
        // Set line width
        glLineWidth(_curvePositions[i].width);
        
        // Position attribute
        glEnableVertexAttribArray(_vertLoc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers[i]);
        glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        
        glDrawArrays(GL_LINE_STRIP, 0, _curvePositions[i].points.size());
        
        glDisableVertexAttribArray(_vertLoc);
    }
}
