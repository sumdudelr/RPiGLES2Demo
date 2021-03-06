#ifndef LABEL_H
#define LABEL_H

#include "shader.hpp"
#include "camera.hpp"
#include "points.hpp"

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Label {
public:
    // Use this struct to pass coordinates and their corresponding name
    struct Point {
        // Coordinates in LLH
        glm::vec3 coordinates;
        
        // Color for the point
        glm::vec3 pointColor;
        
        // Size for the point (pixels)
        float pointSize;
        
        // Name string for the point -- No label if NULL
        const char* name;
        
        // Text color
        glm::vec3 textColor;
    };
    
    Label();
    ~Label();
    void init(Camera* camera, std::vector<Point> points);
    void update(std::vector<Point> points);
    void render();
    
private:
    struct Attrib {
        glm::vec2 vertex;
        glm::vec2 texCoord;
    };
    
    // Used internally for keeping track of points
    struct P {
        // Coordinates in ECEF
        glm::vec3 position;
        
        // Number of elements in attribute buffer (for characters)
        int numCharAttrib;
        
        // Shift so the middle is 40 px above 0,0
        glm::vec2 shift;
    };
    
    std::vector<P> _points;
    Points _ppoints;

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
