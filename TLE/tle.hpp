#ifndef TLE_H
#define TLE_H

#include <vector>
#include <string>

#include "SGP4.h"

#include "label.hpp"
#include "lines.hpp"

class TLE {
public:
    TLE();
    ~TLE();
    
    void readTLE(const char* filename);
    void updateTLE(std::vector<Label::Point> &labels, std::vector<Lines::Line> &lines);
    void updateTLE(std::vector<Points::Point> &points);
    
private:
    struct Sat {
        std::string name;
        elsetrec satrec;
    };
    std::vector<Sat> sats_;
    
};

#endif
