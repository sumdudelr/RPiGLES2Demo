#include "tle.hpp"

#include "ellipsoid.hpp"

#include <fstream>
#include <cmath>
#include <ctime>

TLE::TLE() {}

TLE::~TLE() {}

void TLE::readTLE(const char* filename) {
    std::ifstream file(filename);
    char title[130]; char line1[130]; char line2[130];
    // Read TLE entries until EOF
    while (!file.eof()) {
        file.getline(title, 130);
        file.getline(line1, 130);
        file.getline(line2, 130);
        
        double startmfe, stopmfe, deltamin;
        elsetrec satrec;
        SGP4Funcs::twoline2rv(line1, line2, 'c', 'e', 'a', wgs84, startmfe, stopmfe, deltamin, satrec);
        
        // Trim trailing spaces from title
        int end = std::strlen(title) - 1;
        while(end > 0 && std::isspace((unsigned char)title[end])) end--;
        title[end+1] = '\0';
        
        Sat sat = { title, satrec };
        sats_.push_back(sat);
    }
}

void TLE::updateTLE() {
    double conv = M_PI / (180.0*3600.0);
    double xp = -0.140682 * conv;
    double yp = 0.333309 * conv;
    double dut1 = -0.4399619;
    
    std::vector<Label::Point> labels;
    std::vector<Lines::Line> lines;
    
    std::time_t current = std::time(nullptr);
    
    int year, mon, day, hr, minute; double sec;
    double r[3], v[3];
    
    for (auto& sat : sats_) {
        SGP4Funcs::invjday_SGP4(sat.satrec.jdsatepoch, sat.satrec.jdsatepochF, year, mon, day, hr, minute, sec);
        
        std::tm jt{};
        jt.tm_year = year - 1900;
        jt.tm_mon = mon - 1;
        jt.tm_mday = day;
        jt.tm_hour = hr;
        jt.tm_min = minute;
        jt.tm_sec = (int)sec;
        std::time_t epoch = timegm(&jt);
        double diff = std::difftime(current, epoch) / 60.0;
        
        double period = M_PI/sat.satrec.no_kozai; // aprrox. 1/2 orbial period
        double startmfe = diff - period;
        double stopmfe = diff + period;
        
        // Get position for the current point in time
        SGP4Funcs::sgp4(sat.satrec, diff, r, v);
        double jd = sat.satrec.jdsatepoch;
        double jdfrac = sat.satrec.jdsatepochF + diff/1440.0;
        if (jdfrac < 0.0) {
            jd = jd - 1.0;
            jdfrac = jdfrac + 1.0;
        }
        
        double jdut1 = jd;
        double jdut1frac = jdfrac + dut1/86400.0;
        double ttt = (jdut1-2451545.0)/36525.0;
        
        glm::dvec3 ecef = teme2ecef(r, ttt, jdut1+jdut1frac, xp, yp) * glm::dvec3(1000.0);
        
        Label::Point point{
            (glm::vec3)ecef,
            glm::vec3(1.0f, 0.0f, 0.0f),
            5.0f,
            sat.name.c_str(),
            glm::vec3(1.0f, 1.0f, 1.0f)
        };
        labels.push_back(point);
        
        // Calculate for the rest of the line
        std::vector<glm::vec3> orbit;
        double tsince = startmfe;
        while (tsince < stopmfe) {
            SGP4Funcs::sgp4(sat.satrec, tsince, r, v);
            jd = sat.satrec.jdsatepoch;
            jdfrac = sat.satrec.jdsatepochF + tsince/1440.0;
            if (jdfrac < 0.0) {
                jd = jd - 1.0;
                jdfrac = jdfrac + 1.0;
            }
            
            jdut1 = jd;
            jdut1frac = jdfrac + dut1/86400.0;
            ttt = (jdut1-2451545.0)/36525.0;
            
            ecef = teme2ecef(r, ttt, jdut1+jdut1frac, xp, yp) * glm::dvec3(1000.0);
            
            orbit.push_back(ecef);
            
            tsince++;
        }
        Lines::Line line{
            orbit,
            glm::vec3(1.0f, 1.0f, 0.0f),
            3.0f
        };
        lines.push_back(line);
    }
    
    labels_ = labels;
    lines_ = lines;
}
