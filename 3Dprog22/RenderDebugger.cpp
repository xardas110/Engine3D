#include "RenderDebugger.h"
#include "vertex.h"
#include "RenderEngine.h"
#include "Include/glm/glm.hpp"

RenderDebugger* RenderDebugger::Get()
{
    static RenderDebugger renderDebugger;
    return &renderDebugger;
}

RenderDebugger::RenderDebugger()
{
    initializeOpenGLFunctions();
    GenerateDebugBox();
    GenerateDebugPoint();
    GenerateDebugSphere(3);
    GenerateDebugSegment();
    InitShader();
    InitSSBO();
}

void RenderDebugger::InitShader()
{
    auto* re = RenderEngine::Get();
    re->GetShader(ShaderManager::Debug, shader);
    re->BindShader(shader);
    colorUniform = re->GetUniformLocation("color");
    re->SetVec3(colorUniform, {1.f, 0.f, 0.f});

}

void RenderDebugger::InitSSBO()
{
    auto* re = RenderEngine::Get();
    auto* sm = re->GetSSBOManager();
    ssbo = sm->CreateSSBO("DebugElements", MAX_DEBUG_ELEMENTS * sizeof(GpuData), 6);
    sm->BindShaderToBlock(shader.shaderId, ssbo.id, 6, "instancedMatrices");
}

void RenderDebugger::Render(float deltaTime)
{
    DrawDebugPoints(deltaTime);
    DrawDebugBoxes(deltaTime);
    DrawDebugSegments(deltaTime);
    DrawDebugSphere(deltaTime);
}

void RenderDebugger::AddDebugPoint(float timer, const glm::vec3& a, const glm::vec3& color, float size, bool bDepthTest)
{
    DebugPoint point;
    point.a = a;
    point.color = color;
    point.depthPass = bDepthTest;
    point.duration = timer;
    point.width = size;

    debugPoints.emplace_back(point);
}

void RenderDebugger::AddDebugSegment(float timer, const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, float size, bool bDepthTest)
{
    DebugSegment segment;
    segment.duration = timer;
    segment.a = a;
    segment.b = b;
    segment.color = color;
    segment.width = size;
    segment.depthPass = bDepthTest;

    mMutex.lock();
    debugSegments.emplace_back(segment);
    mMutex.unlock();
}

void RenderDebugger::AddDebugBox(float timer, const glm::vec3& c, const glm::vec3& e, const glm::quat& rot, const glm::vec3 color, float size, bool bDepthTest)
{
    DebugBox box;
    box.duration = timer;
    box.c = c;
    box.e = e;
    box.rot = rot;
    box.color = color;
    box.width = size;
    box.depthPass = bDepthTest;

    debugBoxes.emplace_back(box);
}

void RenderDebugger::AddDebugBoundingBox(float timer, const glm::vec3& min, const glm::vec3& max, const glm::vec3 color, float size, bool bDepthTest)
{
    DebugBox box;
    box.duration = timer;
    box.c = (min + max) * 0.5f;
    box.e = (max - min) * 0.5f;
    box.rot = glm::identity<glm::quat>();
    box.color = color;
    box.width = size;
    box.depthPass = bDepthTest;

    debugBoxes.emplace_back(box);
}

void RenderDebugger::AddDebugSphere(float timer, const glm::vec3& c, float r, const glm::vec3& color, float size, bool bDepthTest)
{
    DebugSphere sphere;
    sphere.duration = timer;
    sphere.c = c;
    sphere.r = r;
    sphere.color = color;
    sphere.width = size;
    sphere.depthPass = bDepthTest;

    debugSpheres.emplace_back(sphere);
}

void RenderDebugger::DrawDebugPoints(float deltaTime)
{
    std::vector<GpuData> gpuDataVec;
    for (auto it = debugPoints.begin(); it != debugPoints.end();)
    {
        auto& point = *it;
        if (point.duration >= 0.f)
        {
            GpuData gpuData;
            gpuData.model = glm::translate(gpuData.model, point.a);
            gpuData.color = { point.color, 1.f };

            gpuDataVec.emplace_back(gpuData);

            point.duration -= deltaTime;
            ++it;
            continue;
        }
        it = debugPoints.erase(it);
    }

    auto* re = RenderEngine::Get();
    re->BindShader(shader);
    re->BindSSBO(ssbo);
    re->BindSSBOData(0, gpuDataVec.size() * sizeof(GpuData), gpuDataVec.data());

    re->DrawElementsInstanced(point, GL_POINT, gpuDataVec.size());
}

void RenderDebugger::DrawDebugSphere(float deltatime)
{
    std::vector<GpuData> gpuDataVec;

    for (auto it = debugSpheres.begin(); it != debugSpheres.end();)
    {
        auto& sphere = *it;
        if (sphere.duration >= 0.f)
        {

            GpuData gpuData;
            gpuData.model = glm::translate(gpuData.model, sphere.c);
            gpuData.model = glm::scale(gpuData.model, glm::vec3(sphere.r * 2));
            gpuData.color = { sphere.color, 1.f };

            gpuDataVec.emplace_back(gpuData);

            sphere.duration -= deltatime;
            ++it;
            continue;
        }
        it = debugSpheres.erase(it);
    }

    auto* re = RenderEngine::Get();
    re->BindSSBO(ssbo);
    re->BindSSBOData(0, gpuDataVec.size() * sizeof(GpuData), gpuDataVec.data());
    re->DrawElementsInstanced(sphere, GL_TRIANGLES, gpuDataVec.size());
}

void RenderDebugger::DrawDebugBoxes(float deltatime)
{
    std::vector<GpuData> gpuDataVec;

    for (auto it = debugBoxes.begin(); it != debugBoxes.end();)
    {
        auto& box = *it;
        if (box.duration >= 0.f)
        {

            GpuData gpuData;

            Transform trans;
            trans.SetPosition(box.c);
            trans.SetScale(box.e * 2.f);
            trans.SetRotation(box.rot);

            gpuData.model = trans.GetTransform();
            gpuData.color = { box.color, 1.f };

            gpuDataVec.emplace_back(gpuData);

            box.duration -= deltatime;
            ++it;
            continue;
        }
        it = debugBoxes.erase(it);
    }

    auto* re = RenderEngine::Get();
    re->BindShader(shader);

    re->BindSSBO(ssbo);
    re->BindSSBOData(0, gpuDataVec.size() * sizeof(GpuData), gpuDataVec.data());
    re->DrawElementsInstanced(box, GL_LINES, gpuDataVec.size());
}

void RenderDebugger::DrawDebugSegments(float deltatime)
{
    std::vector<GpuData> gpuDataVec;
    for (auto it = debugSegments.begin(); it != debugSegments.end();)
    {
        auto& segment = *it;
        if (segment.duration >= 0.f)
        {
            GpuData gpuData;

            const glm::vec3 lineMiddle = (segment.a + segment.b) * 0.5f;
            const glm::vec3 lineVector = segment.b - segment.a;
            const float lineLength = glm::length(lineVector);
            const float lineHalfLength = lineLength * 0.5f;

            const glm::vec3 UnitDir = lineVector / lineLength;
            const glm::vec3 UpDir = { 0.f, 1.f, 0.f };
            const glm::vec3 RightDir = glm::cross(glm::vec3(UnitDir), glm::vec3(UpDir));

            gpuData.model[0] = { RightDir , 0.f };
            gpuData.model[1] = { UpDir, 0.f };
            gpuData.model[2] = { UnitDir * lineHalfLength, 0.f }; // line is uploaded in gpu as pointing towards -z axis
            gpuData.model[3] = { lineMiddle, 1.f };
            gpuData.color = { segment.color, 1.f };

            segment.duration -= deltatime;

            gpuDataVec.emplace_back(gpuData);

            ++it;
            continue;
        }
        it = debugSegments.erase(it);
    }

    auto* re = RenderEngine::Get();
    re->BindSSBO(ssbo);
    re->BindSSBOData(0, gpuDataVec.size() * sizeof(GpuData), gpuDataVec.data());
    re->BindShader(shader);
    re->DisableCulling();
    re->DisableBlending();
    re->EnableDepthTest();
    re->DrawElementsInstanced(segment, GL_LINES, gpuDataVec.size());
}

void RenderDebugger::GenerateDebugSphere(int n)
{
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

    sphere.vao = VAO;
    sphere.indices = indices.size();
}

void RenderDebugger::GenerateDebugBox()
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

    box.vao = VAO;
    box.indices = 24;
}

void RenderDebugger::GenerateDebugPoint()
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

    point.vao = VAO;
    point.indices = indices.size();
}

void RenderDebugger::GenerateDebugSegment()
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

    segment.vao = VAO;
    segment.indices = 2;
}

void RenderDebugger::ClearContainers()
{
    debugPoints.clear();
    debugBoxes.clear();
    debugSegments.clear();
    debugSpheres.clear();
}
