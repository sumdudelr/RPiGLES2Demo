#include "renderer.hpp"
#include "camera.hpp"
#include "globe.hpp"
#include "lines.hpp"
#include "points.hpp"
#include "label.hpp"
#include "ellipsoid.hpp"
#include "tle.hpp"

#include <csignal>
#include <ctime>
#include <locale>

#include "third_party/SGP4.h"

static volatile int terminate;

void signal_handler(int signal) {
    if (signal == SIGINT)
        terminate = 1;
}

int main() {
    // Set up a signal handler for program exit
    std::signal(SIGINT, signal_handler);
    
    TLE tle;
    tle.readTLE("BOBCAT-1.TLE");
    tle.updateTLE();
    
    Renderer render;
    render.initialize();
    
    Camera camera((float)render.screen_wid_, (float)render.screen_hei_, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    camera.zoomToTarget(6378137.0f * 1.25f);
    
    Globe globe;
    globe.init(&camera);
    
    Lines lines;
    lines.init(&camera, tle.lines_);
    
    Label label;
    label.init(&camera, tle.labels_);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Rotate the camera around the globe
        camera.rotateAroundZ(0.3f);
        
        globe.render();
        lines.render();
        label.render();
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
