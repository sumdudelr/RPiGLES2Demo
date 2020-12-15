#ifndef LABEL_H
#define LABEL_H

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Label {
public:
    Label();
    ~Label();
    void init(Camera* camera);
    void render();
    
private:
    struct Attrib {
        glm::vec2 vertex;
        glm::vec2 texCoord;
    };
    
    glm::vec3 _position;
    glm::vec3 _positionShift;

    Shader _shader;
    GLuint _buffers[2];
    
    // Attribute locations
    GLint _vertLoc;
    GLint _texcLoc;
    
    std::vector<Attrib> _vertices;
    int _numCharAttrib;
    
    GLuint _tex;
    
    float _angle;
    
    // Pointer to shared camera
    Camera* _camera;
};
#endif
