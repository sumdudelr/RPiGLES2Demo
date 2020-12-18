#ifndef LINES_H
#define LINES_H

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

class Lines {
public:
    Lines();
    ~Lines();
    void init(Camera* camera, std::vector<std::vector<glm::vec3>> points);
    void render();
    
private:
    Shader _shader;
    std::vector<GLuint> _buffers;
    
    // Attribute locations
    GLint _vertLoc;
    
    std::vector<std::vector<glm::vec3>> _curvePositions;
    
    // Pointer to shared camera
    Camera* _camera;
};
#endif
