#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <cmath>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

glm::vec3 RotateAroundAxis(glm::vec3 vec, glm::vec3 axis, float theta) {
    float u = axis.x;
    float v = axis.y;
    float w = axis.z;
    
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);
    
    float ms = std::pow(u, 2.0f) + std::pow(v, 2.0f) + std::pow(w, 2.0f);
    float m = std::sqrt(ms);
    
    float x = ((u * (u * vec.x + v * vec.y + w * vec.z)) +
        (((vec.x * (v * v + w * w)) - (u * (v * vec.y + w * vec.z))) * cosTheta) +
        (m * ((-w * vec.y) + (v * vec.z)) * sinTheta)) / ms;
    float y = ((v * (u * vec.x + v * vec.y + w * vec.z)) +
        (((vec.y * (u * u + w * w)) - (v * (u * vec.x + w * vec.z))) * cosTheta) +
        (m * ((w * vec.x) - (u * vec.z)) * sinTheta)) / ms;
    float z = ((w * (u * vec.x + v * vec.y + w * vec.z)) +
        (((vec.z * (u * u + v * v)) - (w * (u * vec.x + v * vec.y))) * cosTheta) +
        (m * (-(v * vec.x) + (u * vec.y)) * sinTheta)) / ms;
    glm::vec3 result(x, y, z);
    
    return result;
}

#endif
