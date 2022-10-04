#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <istream>
#include "HelperMath.h"
#include <ostream>
#include "Include/glm/vec2.hpp"
#include "Include/glm/vec3.hpp"

#define MAX_BONES_PR_VERTEX 4

class Vertex
{
public:
 friend std::ostream& operator<< (std::ostream&, const Vertex&);

     //! Overloaded ostream operator which reads all vertex data from an open textfile stream
 friend std::istream& operator>> (std::istream&, Vertex&);


     float m_xyz[3];       	// position i 3D
     float m_normal[3];    // normal in 3D or rgb colors
     float m_st[2];        	// texture coordinates if used

  public:
     Vertex();
     Vertex(float x, float y, float z, float r, float g, float b);
     Vertex(float x, float y, float z, float r, float g, float b, float u, float v);

};
struct StaticMeshVertex
{
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 texCord{};
    glm::vec3 tangent{};
    glm::vec3 bitTangent{};
};
struct SkeletalMeshVertex
{
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 texCord{};
    glm::vec3 tangent{};
    glm::vec3 bitTangent{};
	
    int id[MAX_BONES_PR_VERTEX] = {};
    float weight[MAX_BONES_PR_VERTEX] = {};

    void AddBoneData(int boneId, float weightId)
    {
	    for (auto i = 0; i< 4; i++)
	    {
		    if (Cmp(weight[i], 0.f))
		    {
                id[i] = boneId;
                weight[i] = weightId;
                return;
		    }
	    }
    }
};


#endif // VERTEX_H
