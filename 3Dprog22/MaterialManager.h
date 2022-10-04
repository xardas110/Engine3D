#pragma once
#include <qopenglfunctions_4_5_core.h>
#include "Include/Assimp/material.h"
#include "texturemanager.h"
#include "Material.h"
#include <map>
#include <string>
#include <initializer_list>
#include "Texture.h"

class MaterialManager : protected QOpenGLFunctions_4_5_Core
{
public:
	MaterialManager();
	static MaterialManager& Get();

	/*Albedo-AO-Normal-Opacity-Specular*/
	bool LoadInstancedMaterials(std::initializer_list<std::pair<std::string, Texture::Types>> paths, MaterialInstanced& outMaterial);
private:
	TextureManager textureManager; // cleanup happens when the texturemanager is destroyed
};

