#include "renderer.hpp"
#include "camera.hpp"
#include "globe.hpp"
#include "lines.hpp"
#include "points.hpp"
#include "label.hpp"
#include "ellipsoid.hpp"

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
    
    // Points and labels
    std::vector<Label::Point> pp = {
        {
            glm::vec3(glm::radians(-82.0f), glm::radians(39.0f), 1500000.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            5.0f,
            "Athens, OH",
            glm::vec3(1.0f, 1.0f, 1.0f)
        },
        {
            glm::vec3(glm::radians(-118.0f), glm::radians(34.0f), 1500000.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            5.0f,
            "Los Angeles, CA",
            glm::vec3(1.0f, 1.0f, 1.0f)
        },
        {
            glm::vec3(glm::radians(-84.5f), glm::radians(39.1f), 1000.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            5.0f,
            nullptr,
            glm::vec3(0.0f)
        }
    };
    
    // Compute geodetic curve
    std::vector<Lines::Line> curves = {
        {
            GeodeticCurve(
                glm::vec3(glm::radians(-82.0f), glm::radians(39.0f), 1500000.0f),
                glm::vec3(glm::radians(-118.0f), glm::radians(34.0f), 1500000.0f),
                1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            3.0f
        }
    };
    
    // Compute orbit from TLE
    std::vector<glm::vec3> orbit;
    char title[130]; char line1[130]; char line2[130];
    std::ifstream file("BOBCAT-1.TLE");
    file.getline(title, 130);
    file.getline(line1, 130);
    file.getline(line2, 130);
    file.close();
    double startmfe, stopmfe, deltamin;
    elsetrec satrec;
    SGP4Funcs::twoline2rv(line1, line2, 'c', 'e', 'a', wgs84, startmfe, stopmfe, deltamin, satrec);
    double r[3], v[3];
    SGP4Funcs::sgp4(satrec, 0, r, v);

    /* Convert Julian day to UTC
     * The Julian epoch = Jan 1 
     * The Julian date of any instant is the Julian day number plus the fraction
     * of a day since the preceding noon in Universal Time.
     */
    int year, mon, day, hr, minute; double sec;
    SGP4Funcs::invjday_SGP4(satrec.jdsatepoch, satrec.jdsatepochF, year, mon, day, hr, minute, sec);
    std::tm jt{};
    jt.tm_year = year - 1900;
    jt.tm_mon = mon - 1;
    jt.tm_mday = day;
    jt.tm_hour = hr;
    jt.tm_min = minute;
    jt.tm_sec = (int)sec;
    std::time_t epoch = timegm(&jt);
    std::time_t current = std::time(nullptr);
    double diff = std::difftime(current, epoch);
    char buf[64];
    strftime(buf, sizeof(buf), "%c %Z\n", std::gmtime(&epoch));
    std::cout << "TLE epoch: " << buf;
    strftime(buf, sizeof(buf), "%c %Z\n", std::gmtime(&current));
    std::cout << "Current:   " << buf;
    
    double tsince = diff / 60.0; // start at the current time
    stopmfe = tsince + 60.0; // stop 1 hour later
    while (tsince < stopmfe) {
        SGP4Funcs::sgp4(satrec, tsince, r, v);
        double jd = satrec.jdsatepoch;
        double jdfrac = satrec.jdsatepochF + tsince/1440.0;
        if (jdfrac < 0.0) {
            jd = jd - 1.0;
            jdfrac = jdfrac + 1.0;
        }
        
        double conv = M_PI / (180.0*3600.0);
        double lod = 0.0015563;
        double xp = -0.140682 * conv;
        double yp = 0.333309 * conv;
        double dut1 = -0.4399619;
        double jdut1 = jd;
        double jdut1frac = jdfrac + dut1/86400.0;
        double ttt = (jdut1-2451545.0)/36525.0;
        double ddpsi = -0.056487 * conv;
        double ddeps = -0.002100 * conv;
        
        glm::dvec3 ecef = teme2ecef(r, ttt, jdut1, xp, yp) * glm::dvec3(1000.0, -1000.0, -1000.0);
        
        orbit.push_back(ecef);
        
        tsince += deltamin / 10.0;
    }
    
    std::vector<Lines::Line> ls =
    {
        {
            orbit,
            glm::vec3(1.0f, 0.0f, 0.0f),
            3.0f
        }
    };
            
    
    Renderer render;
    render.initialize();
    
    Camera camera((float)render.screen_wid_, (float)render.screen_hei_, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    camera.zoomToTarget(6378137.0f * 1.25f);
    
    Globe globe;
    globe.init(&camera);
    
    Lines lines;
    lines.init(&camera, ls);
    
    Label label;
    label.init(&camera, pp);
    
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
