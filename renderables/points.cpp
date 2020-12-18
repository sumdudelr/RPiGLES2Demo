#include "points.hpp"
#include "ellipsoid.hpp"

Points::Points() { }

Points::~Points() {
    glDeleteBuffers(1, &_buffer);
}

void Points::init(Camera* camera, std::vector<Point> points) {
    _camera = camera;
    
    _points = points;
    
    for (auto& p : _points) {
        p.coordinates = GeodeticToCart(p.coordinates);
    }
    
    _shader.init("renderables/shaders/pointsV.glsl", "renderables/shaders/pointsF.glsl");
    
    // Generate buffer
    glGenBuffers(1, &_buffer);
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point)*_points.size(), &_points[0], GL_STATIC_DRAW);
    
    // Get attribute locations
    _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
    _colorLoc = glGetAttribLocation(_shader.ID, "Color");
    _sizeLoc = glGetAttribLocation(_shader.ID, "Size");
}

void Points::render() {
    _shader.use();
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    glm::mat4 view = _camera->getViewMatrix();
    glm::mat4 mvp = projection * view;
    _shader.setMat4("mvp", mvp);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(_colorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(_colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)sizeof(glm::vec3));
    
    // Size attribute
    glEnableVertexAttribArray(_sizeLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(_sizeLoc, 1, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));
    
    glDrawArrays(GL_POINTS, 0, _points.size());
    
    glDisableVertexAttribArray(_vertLoc);
    glDisableVertexAttribArray(_colorLoc);
    glDisableVertexAttribArray(_sizeLoc);
}
