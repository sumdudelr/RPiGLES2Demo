#ifndef LINES_H
#define LINES_H

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

class Lines {
public:
    // Use this struct to pass information about a line
    struct Line {
        // Vector of points that make up the line (in ECEF)
        std::vector<glm::vec3> points;
        
        // Color of the line
        glm::vec3 color;
        
        // Width of the line (pixels)
        float width;
    };

    Lines();
    ~Lines();
    void init(Camera* camera, std::vector<Line> lines);
    void render();
    
private:
    Shader _shader;
    std::vector<GLuint> _buffers;
    
    // Attribute locations
    GLint _vertLoc;
    
    std::vector<Line> _curvePositions;
    
    // Pointer to shared camera
    Camera* _camera;
};
#endif
