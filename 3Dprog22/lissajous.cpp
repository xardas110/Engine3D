#include "lissajous.h"

Lissajous::Lissajous()
{
   construct();
}

Lissajous::Lissajous(std::string filnavn) : VisualObject()
{
   readFile(filnavn);
}

void Lissajous::construct()
{
   float a1 = 0.5f, a2 = 0.8f, step = 0.3f, d = (M_PI/2);

   for (float i = 0.f, max = 200.f; i<max; i+= step)
   {
       float x = cos(a1*i + d);
       float y = sin(a2*i);
       mVertices.push_back(Vertex{x, y, 0.f, x, y, x*y});
   }

   writeFile("Lissajou.txt");
}

Lissajous::~Lissajous()
{

}

//Siden inn- og utstrøm operatorene er overlastet i Vertex klassen, blir readFile() ganske enkel (husk å inkludere headerfiler for stream klasser):

void Lissajous::readFile(std::string filnavn) {
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

void Lissajous::writeFile(std::string filnavn)
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

void Lissajous::init(GLint shader)
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

void Lissajous::draw()
{
    glBindVertexArray(mVAO);
    //glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_LINES,0, mVertices.size());
    glBindVertexArray(0);
}
