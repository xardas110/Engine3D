#include "visualcube.h"
#include <fstream>


VisualCube::VisualCube()
{

}

VisualCube::VisualCube(std::string filnavn) : VisualObject()
{
   readFile(filnavn);
   //mMatrix.setToIdentity();
}

void VisualCube::construct()
{
        const auto s = 0.5f;

        mVertices.push_back(Vertex( -s, -s, -s, 0.f, 0.f, -1.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( s, -s, -s, 0.f, 0.f, -1.f,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( s,  s, -s ,  0.f, 0.f, -1.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( s,  s, -s ,  0.f, 0.f, -1.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( -s,  s, -s ,  0.f, 0.f, -1.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( -s, -s, -s ,  0.f, 0.f, -1.f ,  0.0f, 0.0f ));

        mVertices.push_back(Vertex( -s, -s,  s,  0.f, 0.f, 1.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( s, -s,  s ,  0.f, 0.f, 1.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( s,  s,  s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( s,  s,  s ,  0.f, 0.f, 1.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( -s,  s,  s ,  0.f, 0.f, 1.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( -s, -s,  s ,  0.f, 0.f, 1.f ,  0.0f, 0.0f ));

        mVertices.push_back(Vertex( -s,  s,  s,   -1.f, 0.f, 0.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( -s,  s, -s , -1.f, 0.f, 0.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( -s, -s, -s ,  -1.f, 0.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( -s, -s, -s ,  -1.f, 0.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( -s, -s,  s ,  -1.f, 0.f, 0.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( -s,  s,  s ,  -1.f, 0.f, 0.f ,  1.0f, 0.0f ));

        mVertices.push_back(Vertex( s,  s,  s,   1.f, 0.f, 0.f,   1.0f, 0.0f ));
        mVertices.push_back(Vertex( s,  s, -s ,  1.f, 0.f, 0.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( s, -s, -s ,  1.f, 0.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( s, -s, -s ,  1.f, 0.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( s, -s,  s ,  1.f, 0.f, 0.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( s,  s,  s ,  1.f, 0.f, 0.f ,  1.0f, 0.0f ));

        mVertices.push_back(Vertex( -s, -s, -s,  0.f, -1.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( s, -s, -s ,  0.f, -1.f, 0.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( s, -s,  s ,  0.f, -1.f, 0.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( s, -s,  s ,  0.f, -1.f, 0.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( -s, -s,  s ,  0.f, -1.f, 0.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( -s, -s, -s ,  0.f, -1.f, 0.f ,  0.0f, 1.0f ));

        mVertices.push_back(Vertex( -s,  s, -s,   0.f, 1.f, 0.f ,  0.0f, 1.0f ));
        mVertices.push_back(Vertex( s,  s, -s ,  0.f, 1.f, 0.f ,  1.0f, 1.0f ));
        mVertices.push_back(Vertex( s,  s,  s ,  0.f, 1.f, 0.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( s,  s,  s ,  0.f, 1.f, 0.f ,  1.0f, 0.0f ));
        mVertices.push_back(Vertex( -s,  s,  s ,  0.f, 1.f, 0.f ,  0.0f, 0.0f ));
        mVertices.push_back(Vertex( -s,  s, -s ,  0.f, 1.f, 0.f ,  0.0f, 1.0f ));


        mIndices.resize(36);

        for (auto i = 0; i < mIndices.size(); i+= 6)
        {
            mIndices[i] = i;
            mIndices[i+1] = i+1;
            mIndices[i+2] = i+2;
            mIndices[i+3] = i+3;
            mIndices[i+4] = i+4;
            mIndices[i + 5] = i + 5;
        }

      writeFile("cube.txt");
}

VisualCube::~VisualCube()
{

}

//Siden inn- og utstrøm operatorene er overlastet i Vertex klassen, blir readFile() ganske enkel (husk å inkludere headerfiler for stream klasser):

void VisualCube::readFile(std::string filnavn) {
   std::ifstream inn;
   inn.open(filnavn.c_str());

   if (inn.is_open()) {
       int n;
       Vertex vertex;
       inn >> n;
       mVertices.reserve(n);
       for (int i=0; i<n; i++) {
            inn >> vertex;
            mVertices.push_back(vertex);
       }
       inn.close();
   }
}

void VisualCube::writeFile(std::string filnavn)
{
    std::ofstream os;
    os.open(filnavn.c_str());

    if (os.is_open())
    {
        os << mVertices.size() << std::endl;

        for (const auto vertex : mVertices)
        {
            os << vertex << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to open file: " << filnavn << std::endl;
    }

    os.close();
}

void VisualCube::init(GLint shader)
{
    construct();

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(mIndices[0]), mIndices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_st[0]));
    glBindVertexArray(0);

    mMatrixUniform = shader;
}

void VisualCube::draw()
{
    glBindVertexArray(mVAO);

    //glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
