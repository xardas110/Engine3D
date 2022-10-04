#include "MeshManager.h"
#include <filesystem>
#include <iostream>
#include <World.h>
#include "Include/glm/vec3.hpp"

MeshManager::MeshManager()
{
    /// <summary>
    /// Meshes that should presist trough the lifetime of the application/world
    /// </summary>
    CreateBox();
    CreateSkybox();
    CreateDebugBoundingBox();
    CreateTriangle();
    CreateOctahedronBall(1);
    CreateSegment();
    CreatePlane();
    CreatePoint();
    CreateTerrainPatch();
    CreateQuad();
    CreateUnitQuad();
}

void MeshManager::CreateBox()
{
    unsigned int VAO, VBO, EBO;
	
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
	
    const auto s = 0.5f;
    /// <summary>
    /// Only need 8 corners, too lazy to fix right now, this is old code
    /// </summary>
    vertices.emplace_back(Vertex(-s, -s, -s, 0.f, 0.f, -1.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(s, -s, -s, 0.f, 0.f, -1.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(s, s, -s, 0.f, 0.f, -1.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, s, -s, 0.f, 0.f, -1.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, s, -s, 0.f, 0.f, -1.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, -s, -s, 0.f, 0.f, -1.f, 0.0f, 0.0f));
             
    vertices.emplace_back(Vertex(-s, -s, s, 0.f, 0.f, 1.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(s, -s, s, 0.f, 0.f, 1.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(s, s, s, 0.f, 0.f, 1.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, s, s, 0.f, 0.f, 1.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, s, s, 0.f, 0.f, 1.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, -s, s, 0.f, 0.f, 1.f, 0.0f, 0.0f));
             
    vertices.emplace_back(Vertex(-s, s, s, -1.f, 0.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, s, -s, -1.f, 0.f, 0.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, -s, -s, -1.f, 0.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, -s, -s, -1.f, 0.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(-s, -s, s, -1.f, 0.f, 0.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, s, s, -1.f, 0.f, 0.f, 1.0f, 0.0f));
             
    vertices.emplace_back(Vertex(s, s, s, 1.f, 0.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(s, s, -s, 1.f, 0.f, 0.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, -s, -s, 1.f, 0.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(s, -s, -s, 1.f, 0.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(s, -s, s, 1.f, 0.f, 0.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(s, s, s, 1.f, 0.f, 0.f, 1.0f, 0.0f));
             
    vertices.emplace_back(Vertex(-s, -s, -s, 0.f, -1.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(s, -s, -s, 0.f, -1.f, 0.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, -s, s, 0.f, -1.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(s, -s, s, 0.f, -1.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, -s, s, 0.f, -1.f, 0.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, -s, -s, 0.f, -1.f, 0.f, 0.0f, 1.0f));
             
    vertices.emplace_back(Vertex(-s, s, -s, 0.f, 1.f, 0.f, 0.0f, 1.0f));
    vertices.emplace_back(Vertex(s, s, -s, 0.f, 1.f, 0.f, 1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, s, s, 0.f, 1.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(s, s, s, 0.f, 1.f, 0.f, 1.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, s, s, 0.f, 1.f, 0.f, 0.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, s, -s, 0.f, 1.f, 0.f, 0.0f, 1.0f));

    indices.resize(36);

    for (size_t i = 0; i < indices.size(); i += 6)
    {
	     indices[i] = i;
	     indices[i + 1] = i + 2;
	     indices[i + 2] = i + 1;
	     indices[i + 3] = i + 3;
	     indices[i + 4] = i + 4;
	     indices[i + 5] = i + 5;
    }

    initializeOpenGLFunctions();
	
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = indices.size();
	
   /*
    std::vector<glm::vec3> vData;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        auto& v = vertices[i];
        vData.push_back({ v.m_xyz[0],v.m_xyz[1],v.m_xyz[2] });
    }
   
    mesh.mVertices = vData;
     */

    meshContainer[MeshType::Mesh][boxString] = mesh;
}

void MeshManager::CreateTriangle()
{
    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    const auto s = 0.5f;

    vertices.emplace_back(Vertex(-s, 0.f, -s,       0.f, 0.f, 1.f,     0.0f, 0.0f));
    vertices.emplace_back(Vertex(s, 0.f, -s,        0.f, 0.f, 1.f,     1.0f, 0.0f));
    vertices.emplace_back(Vertex(0.f, 0.f, s,       0.f, 0.f, 1.f,     0.5f, 1.0f));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = indices.size();

    meshContainer[MeshType::Mesh][triangleString] = mesh;
}

void MeshManager::CreatePlane()
{
    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    const auto s = 0.5f;
    /// <summary>
    /// Only need 8 corners, too lazy to fix right now, this is old code
    /// </summary>
    vertices.emplace_back(Vertex(-s, 0.f, s,    0.f, 1.f, 0.f,  0.0f, 1.0f));
    vertices.emplace_back(Vertex(s, 0.f, s,     0.f, 1.f, 0.f,  1.0f, 1.0f));
    vertices.emplace_back(Vertex(s, 0.f, -s,    0.f, 1.f, 0.f,  1.0f, 0.0f));
    vertices.emplace_back(Vertex(-s, 0.f, -s,   0.f, 1.f, 0.f,  0.0f, 0.0f));

    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);

    indices.emplace_back(0);
    indices.emplace_back(2);
    indices.emplace_back(3);
	
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = indices.size();

    meshContainer[MeshType::Mesh][planeString] = mesh;
}

void MeshManager::CreatePoint()
{
    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(Vertex(0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.0f, 0.0f));
    indices.push_back(0);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = indices.size();
	
    meshContainer[MeshType::Mesh][pointString] = mesh;
}

Mesh MeshManager::CreateOctahedronBall(int n)
{
    
    std::string name = sphereString + std::to_string(n);
    if (meshContainer[MeshType::Mesh].find(name) != meshContainer[MeshType::Mesh].end())
    {
        return meshContainer[MeshType::Mesh][name];
    }

    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    const auto s = 1.f;

    auto CreateTriangle = [&vertices, &indices](const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
    {
        const auto triangleNormal = -glm::cross(glm::normalize(v2 - v1), glm::normalize(v3 - v1));
        vertices.emplace_back(Vertex(v1.x, v1.y, v1.z, triangleNormal.x, triangleNormal.y, triangleNormal.z, 0.f, 0.f));
        vertices.emplace_back(Vertex(v2.x, v2.y, v2.z, triangleNormal.x, triangleNormal.y, triangleNormal.z, 1.f, 0.f));
        vertices.emplace_back(Vertex(v3.x, v3.y, v3.z, triangleNormal.x, triangleNormal.y, triangleNormal.z, 0.5f, 1.f));
        indices.push_back(indices.size());
        indices.push_back(indices.size());
        indices.push_back(indices.size());
    };
	
    std::function< void(const glm::vec3&, const glm::vec3&, const glm::vec3&, int) > SubDivide =
        [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n)
    {
        if (n > 0)
        {
            const auto v1 = glm::normalize(a + b);
            const auto v2 = glm::normalize(a + c);
            const auto v3 = glm::normalize(c + b);
            SubDivide(a, v1, v2, n - 1);
            SubDivide(c, v2, v3, n - 1);
            SubDivide(b, v3, v1, n - 1);
            SubDivide(v3, v2, v1, n - 1);
       }
        else
        {
            CreateTriangle(a, b, c);
        }
    };

    glm::vec3 v0{ 0, s, 0 };
    glm::vec3 v1{ s, 0, 0 };
    glm::vec3 v2{ 0, 0, s };
    glm::vec3 v3{ -s, 0, 0 };
    glm::vec3 v4{ 0, 0, -s };
    glm::vec3 v5{ 0, -s, 0 };


    SubDivide(v0, v1, v2, n);
    SubDivide(v0, v2, v3, n);
    SubDivide(v0, v3, v4, n);
    SubDivide(v0, v4, v1, n);
    SubDivide(v5, v2, v1, n);
    SubDivide(v5, v3, v2, n);
    SubDivide(v5, v4, v3, n);
    SubDivide(v5, v1, v4, n);

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = indices.size();
    
    meshContainer[MeshType::Mesh][name] = mesh;
    
    return mesh;
}

void MeshManager::CreateSkybox()
{
    initializeOpenGLFunctions();

    unsigned VAO{ 0 }, VBO{0};
	
    const float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)NULL);
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = 0;
	
    meshContainer[MeshType::Mesh][skyboxString] = mesh;
}

void MeshManager::CreateSegment()
{
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;

    const auto s = 1.f;

    vertices.emplace_back(Vertex(0, 0.f, s, 0.f, 1.f, 0.f, -0.5f, -0.5f));
    vertices.emplace_back(Vertex(0.f, 0.f, -s, 0.f, 1.f, 0.f, 0.5f, 0.5f));

    unsigned int indices[] =
    {
        0U, 1U
    };

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = 2;

    meshContainer[MeshType::Mesh][segmentString] = mesh;
}

void MeshManager::CreateDebugBoundingBox()
{
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> verts;

    const auto s = 0.5f;

    verts.emplace_back(Vertex(-s, -s, -s, 0.f, 0.f, 1.f, 0.0f, 0.0f));
    verts.emplace_back(Vertex(-s, s, -s, 0.f, 0.f, 1.f, 1.0f, 0.0f));
    verts.emplace_back(Vertex(s, s, -s, 0.f, 0.f, 1.f, 1.0f, 1.0f));
    verts.emplace_back(Vertex(s, -s, -s, 0.f, 0.f, 1.f, 1.0f, 1.0f));

    verts.emplace_back(Vertex(-s, -s, s, 0.f, 0.f, 1.f, 0.0f, 0.0f));
    verts.emplace_back(Vertex(-s, s, s, 0.f, 0.f, 1.f, 1.0f, 0.0f));
    verts.emplace_back(Vertex(s, s, s, 0.f, 0.f, 1.f, 1.0f, 1.0f));
    verts.emplace_back(Vertex(s, -s, s, 0.f, 0.f, 1.f, 1.0f, 1.0f));

    unsigned int indices[26] =
    {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        3, 7,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        6, 7
    };

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = 24;

    meshContainer[MeshType::Mesh][boundingBoxString] = mesh;
}

void MeshManager::CreateQuad(float s)
{ 
    unsigned int VAO, VBO, EBO;

    float vertices[]
    {
        -s, -s, 0,      0,0,
        s, -s, 0,       1,0,
        s, s, 0,        1,1,
        -s, s, 0,       0,1
    };

    int indices[]
    {
        0,1,2,
        0,2,3
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);

    auto strideSize = 20;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideSize, (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideSize, (void*)12);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = 6;

    meshContainer[MeshType::Mesh][quadString] = mesh; 
}

void MeshManager::CreateUnitQuad(float s)
{
    unsigned int VAO, VBO, EBO;

    float vertices[]
    {
        -s, -s, 0,      0,0,
        s, -s, 0,       1,0,
        s, s, 0,        1,1,
        -s, s, 0,       0,1
    };

    int indices[]
    {
        0,1,2,
        0,2,3
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);

    auto strideSize = 20;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideSize, (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideSize, (void*)12);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.indices = 6;

    meshContainer[MeshType::Mesh][unitQuad] = mesh;
}

void MeshManager::CreateTerrainPatch()
{
    unsigned int VBO, VAO;
  
    float vertices[] =
    {
        0.f,0.f,
        0.333f,0.f,
        0.666f,0.f,
        1.f,0.f,

        0.f,0.333f,
        0.333f,0.333f,
        0.666f,0.333f,
        1.f,0.333f,

        0.f,0.666f,
        0.333f,0.666f,
        0.666f,0.666f,
        1.f,0.666f,

        0.f,1.f,
        0.333f,1.f,
        0.666f,1.f,
        1.f,1.f,
    };

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)NULL);
    glPatchParameteri(GL_PATCH_VERTICES, 16);

    Mesh mesh;
    mesh.vbo = VBO;
    mesh.vao = VAO;

    meshContainer[MeshType::Mesh][terrainPatchString] = mesh;
}

void MeshManager::InsertMesh(const std::string& name, Mesh& mesh, MeshType::Type type)
{
    meshContainer[type][name] = mesh;
}

bool MeshManager::HasMesh(const std::string& name, MeshType::Type type)
{
	if (meshContainer[type].find(name) != meshContainer[type].end())
	{
        return true;
	}
    return false;
}

bool MeshManager::GetMesh(const std::string& name, Mesh& outMesh, MeshType::Type type)
{
	if (meshContainer[type].find(name) != meshContainer[type].end())
	{
        outMesh = meshContainer[type][name];
        return true;
	}
    return false;
}

MeshManager::~MeshManager()
{
	for (auto i = 0; i< MeshType::Size; i++)
	{
		for (const auto& mesh : meshContainer[i])
		{
	        glDeleteVertexArrays(1, &mesh.second.vao);
		}
    }
}

Mesh MeshManager::GetSegmentMesh()
{
    return meshContainer[MeshType::Mesh][segmentString];
}

Mesh MeshManager::GetTriangleMesh()
{
    return meshContainer[MeshType::Mesh][triangleString];
}

Mesh MeshManager::GetBoxMesh()
{
    return meshContainer[MeshType::Mesh][boxString];
}

Mesh MeshManager::GetPlaneMesh()
{
    return meshContainer[MeshType::Mesh][planeString];
}

Mesh MeshManager::GetPointMesh()
{
    return meshContainer[MeshType::Mesh][pointString];
}

Mesh MeshManager::GetTerrainPatchMesh()
{
    return meshContainer[MeshType::Mesh][terrainPatchString];
}

Mesh MeshManager::GetQuadMesh()
{
    return meshContainer[MeshType::Mesh][quadString];
}

Mesh MeshManager::GetUnitQuad()
{
    return meshContainer[MeshType::Mesh][unitQuad];
}

bool MeshManager::GetSkyboxMesh(Mesh& outMesh)
{
    return GetMesh(skyboxString, outMesh, MeshType::Mesh);
}
