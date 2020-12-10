#ifndef SPHERETESSELLATOR_H
#define SPHERETESSELLATOR_H

#include <cmath>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
//#include <glm/gtc/half_float.hpp>

struct Attributes {
    glm::vec3 positions;
    glm::vec3 normals;
    glm::vec2 textureCoord;
};

struct Mesh {
    std::vector<Attributes> attributes;
    std::vector<ushort> indices;
};

int NumberOfVertices(int numberOfSubdivisions);
int NumberOfTriangles(int numberOfSubdivisions);
glm::vec2 ComputeTextureCoordinate(glm::vec3 position);
void Subdivide(Mesh& mesh, int t1, int t2, int t3, int level);
Mesh EllipseTessellator(int numberOfSubdivisions);

#endif
