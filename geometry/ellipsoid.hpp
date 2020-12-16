#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <cmath>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

glm::vec3 RotateAroundAxis(glm::vec3 vec, glm::vec3 axis, float theta);

glm::vec3 GeodeticToCart(glm::vec3 geodetic);

#endif
