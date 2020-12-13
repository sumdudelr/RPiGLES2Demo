#include "globe.hpp"

#include "stb_image.h"
#include "stb_image_resize.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

Globe::Globe() {
    
}

Globe::~Globe() {
    glDeleteTextures(1, &_tex);
    glDeleteBuffers(2, &_buffers[0]);
}

void Globe::init(int screenWidth, int screenHeight) {
    _aspect = (float)screenWidth / (float)screenHeight;
    
    _position = glm::vec3(0.0f, 0.0f, 0.0f);
    _mesh = EllipseTessellator(6);
    _shader.init("globeV.glsl", "globeF.glsl");
    
    // Generate buffers
    glGenBuffers(2, &_buffers[0]);
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort)*_mesh.indices.size(), &_mesh.indices[0], GL_STATIC_DRAW);
    // Attribute buffer
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Attributes)*_mesh.attributes.size(), &_mesh.attributes[0], GL_STATIC_DRAW);
    
    // Get attribute locations
    _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
    _normLoc = glGetAttribLocation(_shader.ID, "Norm");
    _texcLoc = glGetAttribLocation(_shader.ID, "Texc");
    
    // Set up texture
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    // Wrapping & filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Load image
    int oWidth, oHeight, nrChannels;
    unsigned char* data = stbi_load("NE2_50M_SR_W_4096.jpg", &oWidth, &oHeight, &nrChannels, 0);
    // Scale the image down
    int nWidth = 1080;
    int nHeight = 720;
    unsigned char data2[nWidth*nHeight*nrChannels];
    stbir_resize_uint8(data, oWidth, oHeight, 0, &data2[0], nWidth, nHeight, 0, nrChannels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    
    _angle = 0.0f;
}

void Globe::render() {
    _angle += 0.2f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);
    _shader.setInt("texture1", 0);
    
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        _aspect,
        0.1f,
        100.0f);
    _shader.setMat4("projection", projection);
    
    glm::mat4 view(1.0f);
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = glm::lookAt(
        glm::vec3(0.0f,0.0f,3.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f));
    _shader.setMat4("view", view);
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, _position);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    _shader.setMat4("model", model);
    
    // Inverse transpose of model
    glm::mat3 ipModel = glm::transpose(glm::inverse(model));
    _shader.setMat3("ipModel", ipModel);
    
    // Light position
    glm::vec3 light(-2.0f,0.0f,2.0f);
    _shader.setVec3("lightPos", light);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(_normLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glVertexAttribPointer(_normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)sizeof(glm::vec3));
    
    // Texture coord attribute
    glEnableVertexAttribArray(_texcLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glVertexAttribPointer(_texcLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)(sizeof(glm::vec3)+sizeof(glm::vec3)));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[0]);
    glDrawElements(GL_TRIANGLES, _mesh.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    
    glDisableVertexAttribArray(_vertLoc);
    glDisableVertexAttribArray(_normLoc);
    glDisableVertexAttribArray(_texcLoc);
}
