#include "renderer.hpp"
#include "camera.hpp"
#include "globe.hpp"
#include "lines.hpp"

#include <csignal>

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
    
    float aspect = (float)render.screen_wid_ / (float)render.screen_hei_;
    
    Camera camera(aspect, glm::vec3(0.0f, 0.0f, 3.0f));
    camera.zoomToTarget(6378137.0f);
    
    Globe globe;
    globe.init(&camera);
    
    Lines lines;
    lines.init(&camera);
    
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        globe.render();
        lines.render();
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
    render.cleanUp();
}
