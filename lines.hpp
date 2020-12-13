#ifndef LINES_H
#define LINES_H

#include "shader.hpp"

#include <vector>

class Lines {
public:
    Lines();
    ~Lines();
    void init(int screenWidth, int screenHeight);
    void render();
    
private:
    Shader _shader;
    GLuint _buffers[2];
    
    // Attribute locations
    GLint _vertLoc;
    
    std::vector<glm::vec3> _curvePositions;
    
    float _angle;
    
    // Information from renderer
    float _aspect;
};
#endif
