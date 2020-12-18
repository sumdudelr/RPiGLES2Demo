#include "ellipsoid.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

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

std::vector<glm::vec3> GeodeticCurve(glm::vec3 start, glm::vec3 stop, float granularity) {
    std::vector<glm::vec3> result;
    
    granularity = glm::radians(granularity);
    
    // Create an arc between two lat/lon points
    start = GeodeticToCart(start);
    stop = GeodeticToCart(stop);
    glm::vec3 radii(6378137.0f, 6378137.0f, 6356752.314245f);
    glm::vec3 oneOverRadiiSquared(
        1.0f / (radii.x * radii.x),
        1.0f / (radii.y * radii.y),
        1.0f / (radii.z * radii.z));
    
    glm::vec3 normal = glm::normalize(glm::cross(start, stop));
    float theta = std::acos(glm::dot(glm::normalize(start), glm::normalize(stop)));
    int n = std::max((theta / granularity) - 1.0f, 0.0f);
    result.push_back(start);
    for (int i = 1; i <= n; i++) {
        float phi = (i * granularity);
        result.push_back(glm::rotate(start, phi, normal));
    }
    result.push_back(stop);
    
    return result;
}
