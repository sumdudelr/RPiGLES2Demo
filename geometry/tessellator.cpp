#include "tessellator.hpp"

int NumberOfVertices(int numberOfSubdivisions) {
    int numberOfVertices = 0;
    for (int i = 0; i < numberOfSubdivisions; i++) {
        numberOfVertices += std::pow(4, i);
    }
    numberOfVertices = 4 + (12 * numberOfVertices);
    
    return numberOfVertices;
}

int NumberOfTriangles(int numberOfSubdivisions) {
    int numberOfTriangles = 0;
    for (int i = 0; i <= numberOfSubdivisions; i++) {
        numberOfTriangles += std::pow(4, i);
    }
    numberOfTriangles *= 4;
    
    return numberOfTriangles;
}

void Subdivide(Mesh& mesh, int t1, int t2, int t3, int level) {
    // WGS-84
    glm::vec3 radii(6378137.0f, 6378137.0f, 6356752.314245f);
    glm::vec3 oneOverRadiiSquared(
        1.0f / (radii.x * radii.x),
        1.0f / (radii.y * radii.y),
        1.0f / (radii.z * radii.z));
        
    if (level > 0) {
        glm::vec3 n01 = glm::normalize((mesh.attributes[t1].positions + mesh.attributes[t2].positions) * 0.5f);
        glm::vec3 n12 = glm::normalize((mesh.attributes[t2].positions + mesh.attributes[t3].positions) * 0.5f);
        glm::vec3 n20 = glm::normalize((mesh.attributes[t3].positions + mesh.attributes[t1].positions) * 0.5f);
        
        glm::vec3 p01 = n01 * radii;
        glm::vec3 p12 = n12 * radii;
        glm::vec3 p20 = n20 * radii;
        
        glm::vec3 d01 = glm::normalize(p01 * oneOverRadiiSquared);
        glm::vec3 d12 = glm::normalize(p12 * oneOverRadiiSquared);
        glm::vec3 d20 = glm::normalize(p20 * oneOverRadiiSquared);
        
        Attributes a01 = {p01, d01};
        Attributes a12 = {p12, d12};
        Attributes a20 = {p20, d20};
        
        mesh.attributes.push_back(a01);
        mesh.attributes.push_back(a12);
        mesh.attributes.push_back(a20);
        
        int i01 = mesh.attributes.size() - 3;
        int i12 = mesh.attributes.size() - 2;
        int i20 = mesh.attributes.size() - 1;
        
        // Subdivide input triangle into four triangles
        level--;
        Subdivide(mesh, t1, i01, i20, level);
        Subdivide(mesh, i01, t2, i12, level);
        Subdivide(mesh, i01, i12, i20, level);
        Subdivide(mesh, i20, i12, t3, level);
    }
    else {
        mesh.indices.push_back(t1);
        mesh.indices.push_back(t2);
        mesh.indices.push_back(t3);
    }
}

Mesh EllipseTessellator(int numberOfSubdivisions) {
    // WGS-84
    glm::vec3 radii(6378137.0f, 6378137.0f, 6356752.314245f);
    glm::vec3 oneOverRadiiSquared(
        1.0f / (radii.x * radii.x),
        1.0f / (radii.y * radii.y),
        1.0f / (radii.z * radii.z));
    
    Mesh mesh;
    
    int numVertices = NumberOfVertices(numberOfSubdivisions);
    int numIndices = 3 * NumberOfTriangles(numberOfSubdivisions);
    
    mesh.attributes.reserve(numVertices);
    mesh.indices.reserve(numIndices);
    
    // Initial tetrahedron
    float negativeRootTwoOverThree = -std::sqrt(2.0f) / 3.0f;
    float negativeOneThird = -1.0f / 3.0f;
    float rootSixOverThree = std::sqrt(6.0f) / 3.0f;
    
    glm::vec3 n0(0.0f, 0.0f, 1.0f);
    glm::vec3 n1(0.0f, 2.0f * std::sqrt(2.0f) / 3.0f, negativeOneThird);
    glm::vec3 n2(-rootSixOverThree, negativeRootTwoOverThree, negativeOneThird);
    glm::vec3 n3(rootSixOverThree, negativeRootTwoOverThree, negativeOneThird);
    
    glm::vec3 p0 = n0 * radii;
    glm::vec3 p1 = n1 * radii;
    glm::vec3 p2 = n2 * radii;
    glm::vec3 p3 = n3 * radii;
    
    glm::vec3 d0 = glm::normalize(p0 * oneOverRadiiSquared);
    glm::vec3 d1 = glm::normalize(p1 * oneOverRadiiSquared);
    glm::vec3 d2 = glm::normalize(p2 * oneOverRadiiSquared);
    glm::vec3 d3 = glm::normalize(p3 * oneOverRadiiSquared);
    
    Attributes a0 = {p0, d0};
    Attributes a1 = {p1, d1};
    Attributes a2 = {p2, d2};
    Attributes a3 = {p3, d3};
    
    mesh.attributes.push_back(a0);
    mesh.attributes.push_back(a1);
    mesh.attributes.push_back(a2);
    mesh.attributes.push_back(a3);
    
    Subdivide(mesh, 0, 1, 2, numberOfSubdivisions);
    Subdivide(mesh, 0, 2, 3, numberOfSubdivisions);
    Subdivide(mesh, 0, 3, 1, numberOfSubdivisions);
    Subdivide(mesh, 1, 3, 2, numberOfSubdivisions);
    
    return mesh;
}
