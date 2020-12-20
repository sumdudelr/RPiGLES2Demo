#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <cmath>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

glm::vec3 RotateAroundAxis(glm::vec3 vec, glm::vec3 axis, float theta);

glm::vec3 GeodeticToCart(glm::vec3 geodetic);

std::vector<glm::vec3> GeodeticCurve(glm::vec3 start, glm::vec3 stop, float granularity);

// Convert from TEME to ECEF
glm::dvec3 teme2ecef(double rteme[3], double ttt, double jdut1, double xp, double yp);

#endif
