#include "MaterialManager.h"
#include <iostream>

MaterialManager::MaterialManager()
{
    initializeOpenGLFunctions();
}

MaterialManager& MaterialManager::Get()
{
    static MaterialManager materialManager;
    return materialManager;
}

bool MaterialManager::LoadInstancedMaterials(std::initializer_list<std::pair<std::string, Texture::Types>> paths, MaterialInstanced& outMaterial)
{
    std::cout << "Loading shared material.." << std::endl;
    for (auto& [path, type] : paths)
    {
        std::cout << "Loading texture at path: " << path << std::endl;
        if (type == Texture::Types::Diffuse)
        {
            if (!textureManager.LoadTexture(path, outMaterial.diffusemap, type))
            {
                std::cout << "Failed to load diffuse texture" << std::endl;
            }
        }
        if (type == Texture::Types::Ambient)
        {
            if (!textureManager.LoadTexture(path, outMaterial.ambientmap, type))
            {
                std::cout << "Failed to load AO texture" << std::endl;
            }
        }
        if (type == Texture::Types::Normals)
        {
            if (!textureManager.LoadTexture(path, outMaterial.normalmap, type))
            {
                std::cout << "Failed to load normal texture" << std::endl;
            }
        }
        if (type == Texture::Types::Opacity)
        {
            if (!textureManager.LoadTexture(path, outMaterial.opacitymap, type))
            {
                std::cout << "Failed to load opacity texture" << std::endl;
            }
        }
        if (type == Texture::Types::Specular)
        {
            if (!textureManager.LoadTexture(path, outMaterial.specularmap, type))
            {
                std::cout << "Failed to load specular texture" << std::endl;
            }
        }
        if (type == Texture::Types::Roughness)
        {
            if (!textureManager.LoadTexture(path, outMaterial.roughnessmap, type))
            {
                std::cout << "Failed to load specular texture" << std::endl;
            }
        }
        if (type == Texture::Types::Metallic)
        {
            if (!textureManager.LoadTexture(path, outMaterial.metallicmap, type))
            {
                std::cout << "Failed to load specular texture" << std::endl;
            }
        }
    }
    std::cout << "Finished loading material" << std::endl;
    return true;
}
