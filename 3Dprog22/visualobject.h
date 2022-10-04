#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <QMatrix4x4>
#include <vector>
#include "vertex.h"
#include "Include/glm/glm.hpp"

class VisualObject : public QOpenGLFunctions_4_1_Core
{
public:
   VisualObject();
	
  VisualObject(const VisualObject& other)
  {
	  this->mVAO = other.mVAO;
  };
	
   ~VisualObject();
   //virtual void LoadTextures(std::vector<std::string> paths) = 0;
   virtual void init(GLint matrixUniform)=0;
   virtual void draw()=0;
   const glm::mat4 & GetTranslation() const;
   void SetTransform(const glm::mat4 &transform);
   void SetPosition(const glm::vec3 &newPos);
   void SetScale(const glm::vec3 &newScale);
   glm::vec3 GetPosition() const;
   //glm::vec3 GetPosition() const;
protected:
   std::vector<Vertex> mVertices;
   std::vector<uint> mIndices;
   GLuint mVAO{0};
   GLuint mVBO{0};
   GLuint mEBO{0};
   GLuint mTex{0};
   GLint mMatrixUniform{0};
   glm::mat4 mMatrix{1.f};
};

class VisualBoundingBox : public VisualObject
{
public:
    virtual void init(GLint matrixUniform) override;
    virtual void draw() override;
};


#endif // VISUALOBJECT_H
