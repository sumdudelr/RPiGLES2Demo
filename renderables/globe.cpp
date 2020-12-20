#include "globe.hpp"

#include "stb_image.h"
#include "stb_image_resize.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 16 byte header + 1024x1024x3 image w/ 6x compression
#define TEXTURE_SIZE_BYTES 16+1024*1024*3/6

Globe::Globe() {
    
}

Globe::~Globe() {
    glDeleteTextures(1, &_tex);
    glDeleteBuffers(2, &_buffers[0]);
}

void Globe::init(Camera* camera) {
    _camera = camera;
    
    _mesh = EllipseTessellator(6);
    _shader.init("renderables/shaders/globeV.glsl", "renderables/shaders/globeF.glsl");
    
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
    
    // Set up cube map texture
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
    // Wrapping & filtering options
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    static const char* faces[] = {
        "data/NE2_50M_SR_W_4096_front.pkm",
        "data/NE2_50M_SR_W_4096_back.pkm",
        "data/NE2_50M_SR_W_4096_right.pkm",
        "data/NE2_50M_SR_W_4096_left.pkm",
        "data/NE2_50M_SR_W_4096_top.pkm",
        "data/NE2_50M_SR_W_4096_bottom.pkm"
    };
    
    // Load images
    char buffer[TEXTURE_SIZE_BYTES]; 
    for (uint i = 0; i < 6; i++) {
        std::ifstream fin(faces[i], std::ios::in | std::ios::binary);
        fin.read(buffer, TEXTURE_SIZE_BYTES);
        glCompressedTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0,
            GL_ETC1_RGB8_OES,
            1024,
            1024,
            0,
            TEXTURE_SIZE_BYTES - 16,
            &buffer[16]);
    }
}

void Globe::render() {
    _shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
    _shader.setInt("texture1", 0);
    
    glm::mat4 projection = _camera->getProjectionMatrix();
    glm::mat4 view = _camera->getViewMatrix();
    _shader.setMat4("vp", projection * view);
    
    // Inverse transpose of model
    glm::mat3 ipModel = glm::transpose(glm::inverse(glm::mat4(1.0f)));
    _shader.setMat3("ipModel", ipModel);
    
    // Light position
    glm::vec3 light(-1.0f,1.0f,0.0f);
    _shader.setVec3("lightPos", light);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glVertexAttribPointer(_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(_normLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glVertexAttribPointer(_normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)sizeof(glm::vec3));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[0]);
    glDrawElements(GL_TRIANGLES, _mesh.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    
    glDisableVertexAttribArray(_vertLoc);
    glDisableVertexAttribArray(_normLoc);
}
