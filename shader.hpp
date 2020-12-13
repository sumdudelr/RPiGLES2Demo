#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <GLES2/gl2.h>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    // The program ID
    GLuint ID;
    
    // Constructor
    Shader(){};
    
    // Reads and builds the shader
    void init(const char* vertexPath, const char* fragmentPath);
    
    // Use/activate the shader
    void use();
    
    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
};

#endif
