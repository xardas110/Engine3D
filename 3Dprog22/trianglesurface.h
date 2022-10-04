#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"
//#include "vector3d.h"

class TriangleSurface : public VisualObject
{
public:
   TriangleSurface();
   TriangleSurface(std::string filnavn);

   float GetHeightAt(glm::vec3 pos);

   void construct();
   ~TriangleSurface() override;
   void readFile(std::string filnavn);
   void writeFile(std::string filnavn);
   void init(GLint shader) override;
   void draw() override;
};

#endif // TRIANGLESURFACE_H
