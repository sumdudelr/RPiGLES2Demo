#include "renderer.hpp"
#include "shader.hpp"
#include "tessellator.hpp"

#include <csignal>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "stb_image.h"
#include "stb_image_resize.h"

static volatile int terminate;

void signal_handler(int signal) {
    if (signal == SIGINT)
        terminate = 1;
}

int main() {
    // Set up a signal handler for program exit
    std::signal(SIGINT, signal_handler);
    
    Renderer render;
    render.initialize();
    
    Shader shader("vertshader.glsl", "fragshader.glsl");
    
    glm::vec3 globePosition(0.0f,  0.0f,  0.0f);
    
    Mesh mesh = EllipseTessellator(6);
    
    // Generate buffers
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort)*mesh.indices.size(), &mesh.indices[0], GL_STATIC_DRAW);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Attributes)*mesh.attributes.size(), &mesh.attributes[0], GL_STATIC_DRAW);
    
    // Get location for buffer attributes
    GLint vPos = glGetAttribLocation(shader.ID, "aPos");
    //~ GLint nPos = glGetAttribLocation(shader.ID, "u_Norm");
    GLint tPos = glGetAttribLocation(shader.ID, "aTexCoord");
    
    // Create the textures
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Load and generate the texture
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true); // flip images along y-axis
    unsigned char *data = stbi_load("NE2_50M_SR_W_4096.jpg", &width, &height, &nrChannels, 0);
    int texWidth = 1080;
    int texHeight = 720;
    unsigned char data2[texWidth*texHeight*nrChannels];
    if (data) {
        stbir_resize_uint8(data, width, height, 0, &data2[0], texWidth, texHeight, 0, nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture 1" << std::endl;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    
    float angle = 0.0f;
    
    do {
        angle += 0.2f;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        shader.setInt("texture1", 0);
        
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)render.screen_wid_ / (float)render.screen_hei_,
            0.1f,
            100.0f);
        shader.setMat4("projection", projection);
        
        glm::mat4 view(1.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::lookAt(
            glm::vec3(0.0f,0.0f,3.0f),
            glm::vec3(0.0f,0.0f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f));
        shader.setMat4("view", view);
        
        
        glm::mat4 model(1.0f);
        model = glm::translate(model, globePosition);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
    
        // Position attribute
        glEnableVertexAttribArray(vPos);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)0);
        
        // Normal attribute
        //~ glEnableVertexAttribArray(nPos);
        //~ glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //~ glVertexAttribPointer(nPos, 3, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)sizeof(glm::vec3));
        
        // Texture coord attribute
        glEnableVertexAttribArray(tPos);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(tPos, 2, GL_FLOAT, GL_FALSE, sizeof(Attributes), (void*)(sizeof(glm::vec3)+sizeof(glm::vec3)));
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        
        glDisableVertexAttribArray(vPos);
        //~ glDisableVertexAttribArray(nPos);
        glDisableVertexAttribArray(tPos);
        
        render.updateScreen();
        
        // Check for errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL Error! " << err << std::endl;
            if (err == GL_OUT_OF_MEMORY)
                std::cerr << "Out of memory!" << std::endl;
            if (err == GL_INVALID_VALUE)
                std::cerr << "Invalid value!" << std::endl;
            terminate = true;
        }
    } while(!terminate);
    
    // Clean up
    glDeleteTextures(1, &texture1);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    render.cleanUp();
}
