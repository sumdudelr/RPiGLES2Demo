#include "globe.hpp"

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
    
    Globe globe;
    globe.init(render.screen_wid_, render.screen_hei_);
    
    //~ // Create an arc between two lat/lon points
    //~ glm::vec3 start(glm::radians(40.0f), glm::radians(40.0f), 0.0f);
    //~ glm::vec3 stop(glm::radians(120.0f), glm::radians(-30.0f), 0.0f);
    //~ glm::vec3 radii(6378137.0f, 6378137.0f, 6356752.314245f);
    //~ glm::vec3 oneOverRadiiSquared(
        //~ 1.0f / (radii.x * radii.x),
        //~ 1.0f / (radii.y * radii.y),
        //~ 1.0f / (radii.z * radii.z));
    
    //~ glm::vec3 normal = glm::normalize(glm::cross(start, stop));
    //~ float theta = std::acos(glm::dot(glm::normalize(start), glm::normalize(stop)));
    //~ int n = std::max((theta / 1.0f) - 1.0f, 0.0f);
    //~ std::vector<glm::vec3> curvePositions;
    //~ curvePositions.push_back(start);
    //~ for (int i = 1; i <= n; i++) {
        //~ float phi = (i * 1.0f);
        //~ glm::vec3 a = RotateAroundAxis(start, normal, phi);
        //~ float beta = 1.0f / std::sqrt(
            //~ (a.x * a.x) * oneOverRadiiSquared.x +
            //~ (a.y * a.y) * oneOverRadiiSquared.y +
            //~ (a.z * a.z) * oneOverRadiiSquared.z);
        //~ curvePositions.push_back(glm::vec3(beta) * a);
    //~ }
    //~ curvePositions.push_back(stop);
    
    do {
        globe.render();
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
