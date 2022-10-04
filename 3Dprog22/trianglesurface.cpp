#include "trianglesurface.h"
#include <iostream>
#include <fstream>

TriangleSurface::TriangleSurface()
{
   construct();
}

TriangleSurface::TriangleSurface(std::string filnavn) : VisualObject()
{
   readFile(filnavn);
   // mMatrix.setToIdentity();
}

float TriangleSurface::GetHeightAt(glm::vec3 pos)
{

    float z = cos(M_PI*pos.x)*sin(M_PI*pos.y);

    return z;
}

void TriangleSurface::construct()
{
    float xmin=-25.0f, xmax=25.0f, ymin=-25.0f, ymax=25.0f, h=2.5f;

      for (auto x=xmin; x<xmax; x+=h)
          for (auto y=ymin; y<ymax; y+=h)
          {
              float z = cos(M_PI*x)*sin(M_PI*y);
              mVertices.push_back(Vertex{x,y,z,x,y,z});
              z = cos(M_PI*(x+h))*sin(M_PI*y);
              mVertices.push_back(Vertex{x+h,y,z,x,y,z});
              z = cos(M_PI*x)*sin(M_PI*(y+h));
              mVertices.push_back(Vertex{x,y+h,z,x,y,z});
              mVertices.push_back(Vertex{x,y+h,z,x,y,z});
              z = cos(M_PI*(x+h))*sin(M_PI*y);
              mVertices.push_back(Vertex{x+h,y,z,x,y,z});
              z = cos(M_PI*(x+h))*sin(M_PI*(y+h));
              mVertices.push_back(Vertex{x+h,y+h,z,x,y,z});
          }

      writeFile("TriangleSurface.txt");
}

TriangleSurface::~TriangleSurface()
{

}

//Siden inn- og utstrøm operatorene er overlastet i Vertex klassen, blir readFile() ganske enkel (husk å inkludere headerfiler for stream klasser):

void TriangleSurface::readFile(std::string filnavn) {
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

void TriangleSurface::writeFile(std::string filnavn)
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

void TriangleSurface::init(GLint shader)
{

    mMatrixUniform = shader;

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal[0]));

    glBindVertexArray(0);

}

void TriangleSurface::draw()
{
    glBindVertexArray(mVAO);
    //glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawArrays(GL_TRIANGLES,0, mVertices.size());
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBindVertexArray(0);
}
