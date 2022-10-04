#include "visualobject.h"

VisualObject::VisualObject() {  }
VisualObject::~VisualObject() {
   //glDeleteVertexArrays( 1, &mVAO );
   //glDeleteBuffers( 1, &mVBO );
}

const glm::mat4 &VisualObject::GetTranslation() const
{
    return mMatrix;
}

void VisualObject::SetTransform(const glm::mat4 &transform)
{
    mMatrix = transform;
}

void VisualObject::SetPosition(const glm::vec3 &newPos)
{
    mMatrix[3][0] = newPos.x;
    mMatrix[3][1] = newPos.y;
    mMatrix[3][2] = newPos.z;
}

void VisualObject::SetScale(const glm::vec3 &newScale)
{
    mMatrix[0][0] = newScale.x;
    mMatrix[1][1] = newScale.y;
    mMatrix[2][2] = newScale.z;
}

glm::vec3 VisualObject::GetPosition() const
{
    return mMatrix[3];
}

void VisualBoundingBox::init(GLint matrixUniform)
{
    unsigned int VBO, EBO;
    std::vector<Vertex> verts;

    const auto s = 0.5f;

    verts.push_back(Vertex( -s, -s, -s, 0.f, 0.f, 1.f ,  0.0f, 0.0f ));
    verts.push_back(Vertex( -s, s, -s, 0.f, 0.f, 1.f,  1.0f, 0.0f ));
    verts.push_back(Vertex( s,  s, -s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));
    verts.push_back(Vertex( s,  -s, -s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));

    verts.push_back(Vertex( -s, -s, s, 0.f, 0.f, 1.f ,  0.0f, 0.0f ));
    verts.push_back(Vertex( -s, s, s, 0.f, 0.f, 1.f,  1.0f, 0.0f ));
    verts.push_back(Vertex( s,  s, s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));
    verts.push_back(Vertex( s,  -s, s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));

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
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(mVAO);
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

}

void VisualBoundingBox::draw()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
