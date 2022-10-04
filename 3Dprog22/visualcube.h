#ifndef VISUALCUBE_H
#define VISUALCUBE_H
#include "visualobject.h"
#include "iostream"

class VisualCube : public VisualObject
{
public:
   VisualCube();
   VisualCube(std::string filnavn);

   VisualCube(const VisualCube& other)
   {
	   this->mVAO = other.mVAO;
   }

   VisualCube& operator=(const VisualCube& other)
   {
	   this->mVAO = other.mVAO;
	   return *this;
   };

   void construct();
   ~VisualCube() override;
   void readFile(std::string filnavn);
   void writeFile(std::string filnavn);
   void init(GLint shader) override;
   void draw() override;
};


#endif // VISUALCUBE_H
