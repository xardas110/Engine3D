#ifndef LISSAJOUS_H
#define LISSAJOUS_H

#include "visualobject.h"
#include <iostream>
#include <fstream>

class Lissajous : public VisualObject
{
public:
   Lissajous();
   Lissajous(std::string filnavn);
   void construct();
   ~Lissajous() override;
   void readFile(std::string filnavn);
   void writeFile(std::string filnavn);
   void init(GLint shader) override;
   void draw() override;
};

#endif // LISSAJOUS_H
