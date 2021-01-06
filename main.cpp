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
#include <thread>
#include <mutex>

#include "third_party/SGP4.h"

static volatile int terminate;

void signal_handler(int signal) {
    if (signal == SIGINT)
        terminate = 1;
}

TLE tle;
Renderer render;
Camera* camera;
Globe globe;
//~ Lines lines;
//~ Label label;
Points point;
std::mutex dataMutex;

void renderLoop() {
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
        // Rotate the camera around the globe
        camera->rotateAroundZ(0.3f);
        
        {
            const std::lock_guard<std::mutex> lock(dataMutex);
            globe.render();
            //~ lines.render();
            //~ label.render();
            point.render();
            render.updateScreen();
        }
        
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
    } while (!terminate);
    render.cleanUp();
}

void TLELoop() {
    do {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        
        // Find the current position of the sun
        std::time_t current = std::time(nullptr);
        globe.updateSun(current);
        
        //~ std::vector<Label::Point> labels;
        //~ std::vector<Lines::Line> curves;
        //~ tle.updateTLE(labels, curves);
        //~ const std::lock_guard<std::mutex> lock(dataMutex);
        //~ lines.update(curves);
        //~ label.update(labels);
        
        std::vector<Points::Point> points;
        tle.updateTLE(points);
        const std::lock_guard<std::mutex> lock(dataMutex);
        point.update(points);
        
        std::cout << "Updated Orbit Data!" << std::endl;
    } while (!terminate);
}

int main() {
    // Set up a signal handler for program exit
    std::signal(SIGINT, signal_handler);
    
    //~ std::vector<Label::Point> labels;
    //~ std::vector<Lines::Line> curves;
    std::vector<Points::Point> points;
    //~ tle.readTLE("gps-ops.txt");
    //~ tle.readTLE("BOBCAT-1.TLE");
    tle.readTLE("active.txt");
    //~ tle.updateTLE(labels, curves);
    tle.updateTLE(points);
    
    render.initialize();
    
    camera = new Camera((float)render.screen_wid_, (float)render.screen_hei_, glm::vec3(0.0f, 3.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    camera->zoomToTarget(6378137.0f * 8.0f);
    
    globe.init(camera);
    
    //~ lines.init(camera, curves);
    
    //~ label.init(camera, labels);
    
    point.init(camera, points);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::thread u(TLELoop);
    renderLoop();
    
    u.join();
    
    delete camera;
}
