#ifndef SPHERETESSELLATOR_H
#define SPHERETESSELLATOR_H

#include <cmath>
#include <vector>

#include <glm/glm.hpp>

struct Mesh {
    std::vector<glm::vec3> positions;
    std::vector<ushort> indices;
};

int NumberOfVerticies(int numberOfSubdivisions) {
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
    if (level > 0) {
        mesh.positions.push_back(glm::normalize((mesh.positions[t1] + mesh.positions[t2]) * 0.5f));
        mesh.positions.push_back(glm::normalize((mesh.positions[t2] + mesh.positions[t3]) * 0.5f));
        mesh.positions.push_back(glm::normalize((mesh.positions[t3] + mesh.positions[t1]) * 0.5f));
        
        int i01 = mesh.positions.size() - 3;
        int i12 = mesh.positions.size() - 2;
        int i20 = mesh.positions.size() - 1;
        
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

Mesh SphereTessellator(int numberOfSubdivisions) {
    Mesh mesh;
    
    // Initial tetrahedron
    float negativeRootTwoOverThree = -std::sqrt(2.0f) / 3.0f;
    float negativeOneThird = -1.0f / 3.0f;
    float rootSixOverThree = std::sqrt(6.0f) / 3.0f;
    
    mesh.positions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    mesh.positions.push_back(glm::vec3(0.0f, 2.0f * std::sqrt(2.0f) / 3.0f, negativeOneThird));
    mesh.positions.push_back(glm::vec3(-rootSixOverThree, negativeRootTwoOverThree, negativeOneThird));
    mesh.positions.push_back(glm::vec3(rootSixOverThree, negativeRootTwoOverThree, negativeOneThird));
    
    Subdivide(mesh, 0, 1, 2, numberOfSubdivisions);
    Subdivide(mesh, 0, 2, 3, numberOfSubdivisions);
    Subdivide(mesh, 0, 3, 1, numberOfSubdivisions);
    Subdivide(mesh, 1, 3, 2, numberOfSubdivisions);
    
    return mesh;
}

#endif
