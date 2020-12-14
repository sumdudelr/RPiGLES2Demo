#include "ellipsoid.hpp"

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

glm::vec3 GeodeticToCart(glm::vec3 geodetic) {
    float cosLatitude = std::cos(geodetic.y);
    
    glm::vec3 n(cosLatitude * std::cos(geodetic.x),
        cosLatitude * std::sin(geodetic.x),
        std::sin(geodetic.y));
    glm::vec3 k = n * glm::vec3(6378137.0f * 6378137.0f);
    float gamma = std::sqrt((k.x * n.x) + (k.y * n.y) + (k.z * n.z));
    
    glm::vec3 rSurface = k / glm::vec3(gamma);
    return rSurface + (glm::vec3(geodetic.z) * n);
}
