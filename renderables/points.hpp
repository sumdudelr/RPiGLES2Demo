#ifndef POINTS_H
#define POINTS_H

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

#include <glm/vec3.hpp>

class Points {
public:
    Points();
    ~Points();
    void init(Camera* camera);
    void render();
    
private:
    Shader _shader;
    GLuint _buffer;
    
    // Attribute locations
    GLint _vertLoc;
    
    std::vector<glm::vec3> _points;
    
    float _angle;
    
    // Pointer to shared camera
    Camera* _camera;
};

#endif
