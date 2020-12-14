#include "points.hpp"
#include "ellipsoid.hpp"

Points::Points() { }

Points::~Points() {
    glDeleteBuffers(1, &_buffer);
}

void Points::init(Camera* camera) {
    _camera = camera;
    
    glm::vec3 start(glm::radians(82.0f), glm::radians(-39.0f), 1500000.0f);
    start = GeodeticToCart(start);
    glm::vec3 stop(glm::radians(118.0f), glm::radians(-34.0f), 1500000.0f);
    stop = GeodeticToCart(stop);
    
    _points.push_back(start);
    _points.push_back(stop);
    
    _shader.init("pointsV.glsl", "pointsF.glsl");
    
    // Generate buffer
    glGenBuffers(1, &_buffer);
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_points.size(), &_points[0], GL_STATIC_DRAW);
    
    // Get attribute locations
    _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
    
    _angle = 0.0f;
}

void Points::render() {
    _angle += 0.2f;
    
    _shader.use();
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    _shader.setMat4("projection", projection);
    
    glm::mat4 view = _camera->getViewMatrix();
    _shader.setMat4("view", view);
    
    glm::mat4 model(1.0f);
    //model = glm::translate(model, _position);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    _shader.setMat4("model", model);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glDrawArrays(GL_POINTS, 0, _points.size());
    
    glDisableVertexAttribArray(_vertLoc);
}
