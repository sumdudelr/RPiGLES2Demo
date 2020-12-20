#ifndef GLOBE_H
#define GLOBE_H

#include "renderer.hpp"
#include "shader.hpp"
#include "tessellator.hpp"
#include "camera.hpp"

class Globe {
public:
    Globe();
    ~Globe();
    void init(Camera* camera);
    void render();
    
private:
    glm::vec3 _position;
    Mesh _mesh;
    Shader _shader;
    GLuint _buffers[2];
    
    // Attribute locations
    GLint _vertLoc;
    GLint _normLoc;
    
    // Texture
    GLuint _tex;
    
    // Pointer to shared camera
    Camera* _camera;
};

#endif
