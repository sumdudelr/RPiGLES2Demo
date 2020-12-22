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

double gstime(double jdut1) {
    double tut1 = (jdut1 - 2451545.0) / 36525.0;
    
    double temp = -6.2e-6 * tut1 * tut1 + 0.093104 * tut1 * tut1
        + (876600.0 * 3600.0 + 8640184.812866) * tut1 + 67310.54841;
        
    temp = std::remainder(glm::radians(temp)/240.0, 2*M_PI);
    
    if (temp < 0.0) temp = temp + 2 * M_PI;
    
    return temp;
}

glm::dmat3 polarm(double xp, double yp, double ttt) {
    double cosxp = std::cos(xp);
    double sinxp = std::sin(xp);
    double cosyp = std::cos(yp);
    double sinyp = std::sin(yp);
    
    glm::dmat3 result{
        cosxp,
        sinxp * sinyp,
        sinxp * cosyp,
        0.0,
        cosyp,
        -sinyp,
        -sinxp,
        cosxp * sinyp,
        cosxp * cosyp
    };
        
    return result;
}

glm::dvec3 teme2ecef(double rteme[3], double ttt, double jdut1, double xp, double yp) {
    double gmst = gstime(jdut1); // find gmst
    
    // find omega from nutation theory
    double omega = 125.04452222 + (-6962890.5390 * ttt +
        7.455 * ttt * ttt + 0.008 * ttt * ttt * ttt) / 3600.0;
    
    omega = glm::radians(std::remainder(omega, 360.0));
    
    // find mean ast
    double gmstg = gmst
        + 0.00264*M_PI / (3600*180)*std::sin(omega)
        + 0.000063*M_PI / (3600*180)*std::sin(2.0 * omega);
    
    gmstg = std::remainder(gmstg, 2.0*M_PI);
    
    glm::dmat3 st{
        std::cos(gmstg),
        std::sin(gmstg),
        0.0,
        -std::sin(gmstg),
        std::cos(gmstg),
        0.0,
        0.0,
        0.0,
        1.0
    };
        
    glm::dmat3 pm = polarm(xp, yp, ttt);
    
    // might need to transpose st and pm
    glm::dvec3 rpef = st * glm::dvec3(rteme[0], rteme[1], rteme[2]);
    glm::dvec3 recef = pm * rpef;

    return recef;
}
