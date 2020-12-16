#ifndef POINTS_H
#define POINTS_H

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

#include <glm/vec3.hpp>

class Points {
public:
    // Use this struct to pass coordinates and their corresponding color
    struct Point {
        // Coordinates in LLH
        glm::vec3 coordinates;
        
        // Color for the point
        glm::vec3 color;
        
        // Size for the point (pixels)
        float size;
    };
    
    Points();
    ~Points();
    void init(Camera* camera, std::vector<Point> points);
    void render();
    
private:
    Shader _shader;
    GLuint _buffer;
    
    // Attribute locations
    GLint _vertLoc;
    GLint _colorLoc;
    GLint _sizeLoc;
    
    std::vector<Point> _points;
    
    // Pointer to shared camera
    Camera* _camera;
};

#endif
