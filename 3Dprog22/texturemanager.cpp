#include "texturemanager.h"
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

std::unique_ptr<TextureManager> TextureManager::Create()
{
    return std::unique_ptr<TextureManager>();
}

bool TextureManager::InsertIntoCubeTextureMap(const CubemapDescriptor& descriptor)
{
    const auto beg = cubeTextureMap.find(descriptor.tag);
	if (beg != cubeTextureMap.end())
	{
        std::cout << "Cubemap texture: " << descriptor.tag << " already exists" << std::endl;
        return false;
	}
    cubeTextureMap[descriptor.tag] = descriptor;
    return true;
}

bool TextureManager::InsertIntoTextureMap(const std::string& path, const Texture& texture)
{
    if (textureMap.find(path) != textureMap.end())
    {
        std::cout << "Texture already exists: " << path << std::endl;
        return false;
    } 
	
    textureMap[path] = texture;
    return true;
}

bool TextureManager::HasCubemap(const CubemapDescriptor& descriptor)
{
    bool bFound = false;
    for (const auto& desc : cubeTextureMap)
    {
        for (size_t i = 0; i < desc.second.texturePaths.size(); i++)
        {
            if (desc.second.texturePaths[i] != descriptor.texturePaths[i])
            {
                bFound = false;
                break;
            }
            bFound = true;
        }
        if (bFound)
        {
            return true;
        }
    }
    return false;
}

bool TextureManager::HasCubemap(const std::string& tag)
{
	if (cubeTextureMap.find(tag) != cubeTextureMap.end())
	{
        return true;
	}
    return false;
}

bool TextureManager::GetCubemap(const std::string& tag, Texture& outCubemapTexture)
{
	if (HasCubemap(tag))
	{
        outCubemapTexture = cubeTextureMap[tag].texture;
        return true;
	}
    return false;
}

bool TextureManager::GetCubemap(const CubemapDescriptor& inDescriptor, Texture& outTexture)
{
    bool bFound = false;
    for (const auto& desc : cubeTextureMap)
    {
        for (size_t i = 0; i < desc.second.texturePaths.size(); i++)
        {
            if (desc.second.texturePaths[i] != inDescriptor.texturePaths[i])
            {
                bFound = false;
                break;
            }
            bFound = true;
        }
    	if (bFound)
    	{
	        outTexture = desc.second.texture;
	        return true;
        }
    }
    return false;
}

bool TextureManager::HasTexture(const std::string& texturePath)
{
    return textureMap.find(texturePath) != textureMap.end();
}

TextureManager::~TextureManager()
{
	for (const auto& textureId : textureMap)
	{
        glDeleteTextures(1, &textureId.second.textureID);
	}
}

bool TextureManager::LoadCubemapTextures(CubemapDescriptor& descriptor, Texture& outTexture)
{
    if (GetCubemap(descriptor, outTexture))
    {
        return true;
    }

    if (HasCubemap(descriptor.tag))
    {
        std::cout << "Cubemap already exists, tag: " << descriptor.tag << std::endl;
        return false;
    }
	
    initializeOpenGLFunctions();
	
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels, dimensions{0};
    for (unsigned int i = 0; i < descriptor.texturePaths.size(); i++)
    {
        unsigned char* data = stbi_load(descriptor.texturePaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            ++dimensions;
            stbi_image_free(data);
        }
        else
        {        
            std::cout << "Cubemap texture failed to load at path: " << descriptor.texturePaths[i] << std::endl;
            stbi_image_free(data);
            break;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (dimensions != 6)
    {
        glDeleteTextures(dimensions, &textureID);
        return false;
    }

    Texture texture;
    texture.dimensions = dimensions;
    texture.textureID = textureID;
    texture.width = width;
    texture.height = height;

    descriptor.texture = texture;
    outTexture = texture;
	
    return InsertIntoCubeTextureMap(descriptor);
}

bool TextureManager::LoadTexture(const std::string& texturePath, Texture& outTexture, Texture::Types type, int minFilter, int magFilter, bool bFlip, bool* hasAlpha)
{
    if (GetTexture(texturePath, outTexture))
    {
        std::cout << "Successfully loaded a texture that already exists: " << texturePath << std::endl;
        return true;
    }

    initializeOpenGLFunctions();
	
    int x, y, channels;
    unsigned textureId;
	
    stbi_set_flip_vertically_on_load(bFlip);
    unsigned char* buffer = stbi_load(texturePath.c_str(), &x, &y, &channels, NULL);
    if (!buffer) {	
        stbi_image_free(buffer);
        printf("Failed to load texture data %s \n", texturePath.c_str()); return false;       
    }
    
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    GLuint format;
    switch (channels)
    {
	    case 1:
	        format = GL_R;		    
	        break;
        case 2:
            format = GL_RG;
            break;
	    case 3:
            format = GL_RGB;
            break;
	    case 4:
        {
	        format = GL_RGBA;
            if (hasAlpha)
            {
                *hasAlpha = true;
            }
        }
	        break;
	    default:
	        format = GL_RGB;
	        break;
    }
	
    glTexImage2D(GL_TEXTURE_2D, NULL, format, x, y, NULL, format, GL_UNSIGNED_BYTE, buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);  

    stbi_image_free(buffer);

    outTexture.width = x;
    outTexture.height = y;
    outTexture.dimensions = 1;
    outTexture.textureID = textureId;
    outTexture.type = type;
    outTexture.format = format;

    textureMap[texturePath] = outTexture;
	
    return true;
}

bool TextureManager::LoadTextureTerrain(const std::string& texturePath, Texture& outTexture, Texture::Types type, int minFilter, int magFilter, bool bFlip)
{
    if (GetTexture(texturePath, outTexture))
    {
        //std::cout << "Successfully loaded a texture that already exists: " << texturePath << std::endl;
        return true;
    }

    initializeOpenGLFunctions();

    int x, y, channels;
    unsigned textureId;

    stbi_set_flip_vertically_on_load(bFlip);
    float* buffer = stbi_loadf(texturePath.c_str(), &x, &y, &channels, NULL);
    if (!buffer) {
        stbi_image_free(buffer);
        printf("Failed to load texture data %s \n", texturePath.c_str()); return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, NULL, GL_R32F, x, y, NULL, GL_RGB, GL_FLOAT, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    stbi_image_free(buffer);

    outTexture.width = x;
    outTexture.height = y;
    outTexture.dimensions = 1;
    outTexture.textureID = textureId;
    outTexture.type = type;
    outTexture.format = GL_R32F;

    textureMap[texturePath] = outTexture;

    return true;
}

bool TextureManager::GenerateTexture(const std::string& name, Texture& outTexture)
{
    initializeOpenGLFunctions();
    if (HasTexture(name))
    {
        printf("Texture with name %s exists, creation failed \n", name.c_str());
        return false;
    }

    unsigned id;
    glGenTextures(1, &id);
    outTexture.textureID = id;

    return true;
}

bool TextureManager::LoadHeightmapBuffer(const Texture& texture, float* outData)
{
    if (!texture.IsValid()) 
    {
        std::cout << "texture is invalid: " << std::endl;
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture.textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, outData);

    return true;
}

bool TextureManager::LoadSlopeBuffer(const Texture& texture, float* outData)
{
    if (!texture.IsValid())
    {
        std::cout << "texture is invalid: " << std::endl;
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture.textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BLUE, GL_FLOAT, outData);

    return true;
}

bool TextureManager::LoadNormalBuffer(const Texture& texture, float* outData)
{
    if (!texture.IsValid())
    {
        std::cout << "texture is invalid: " << std::endl;
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture.textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, outData);

    return true;
}

bool TextureManager::CreateTextureStorage(Texture& outTexture, int x, int y, Texture::Types type, int minFilter, int magFilter, int format)
{
    initializeOpenGLFunctions();

    glTexStorage2D(GL_TEXTURE_2D, 1, format, x, y);

    outTexture.width = x;
    outTexture.height = y;
    outTexture.dimensions = 1;
    outTexture.type = type;
    outTexture.format = format;
    return true;
}

bool TextureManager::LoadEmbeddedTexture(aiTexture* texture, const std::string& texturePath, Texture& outTexture, Texture::Types type)
{
    if (GetTexture(texturePath + "emb", outTexture)) return true;
	
    initializeOpenGLFunctions();
	
    unsigned int textureID; 
    aiTexel* data = texture->pcData;
    const int height = texture->mHeight;
    const int width = texture->mWidth;

    unsigned char* newData = reinterpret_cast<unsigned char*>(data);
    int x, y, nrChannels;
	
    unsigned char* data2 = stbi_load_from_memory(newData, width, &x, &y, &nrChannels, 4);

    if (!data2) { printf("Failed to load texture: %s", texturePath.c_str()); return false; }

    glGenTextures(1, &textureID);
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else
        format = GL_RGB;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data2);

    outTexture.width = x;
    outTexture.height = y;
    outTexture.path = texturePath+"emb";
    outTexture.dimensions = 1;
    outTexture.textureID = textureID;
    outTexture.type = type;

    textureMap[outTexture.path] = outTexture;
	
    return true;
}

bool TextureManager::CreateLazTexture(const std::string& lazPath, Texture& outTexture)
{
    //last inn laz fila og lagre den inn i en tekstur

    /* her er et lite eksempel for å lagre data i tekstur  
    * 
    float* data = new float[width * height * 4];

    GLuint id;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, width, height, GL_RGBA, GL_FLOAT, data);

	outTexture.textureID = id;
	outTexture.width = width;
	outTexture.height = height;
	outTexture.format = GL_RGBA16F;
    outTexture.type = Texture::Height;
    textureMap[lazPath] = outTexture;
	delete[] data;
    */

    return false;
}

bool TextureManager::DeleteTexture(const std::string& name)
{
    if (!HasTexture(name))
    {
        printf("Cant erase texture with name: %s because it doesnt exist\n", name.c_str());
        return false;
    }

    Texture texture = textureMap[name];
    glDeleteTextures(1, &texture.textureID);

    textureMap.erase(name);
    return true;
}

bool TextureManager::DeleteTexture(const Texture& texture)
{
    initializeOpenGLFunctions();
    for (auto it = textureMap.begin(); it != textureMap.end(); ++it)
    {
        std::cout << "Looping for texture deletion" << it->first << std::endl;
        if (it->second.textureID == texture.textureID)
        {
            std::cout << "Texture found: " << texture.textureID << std::endl;
            glDeleteTextures(1, &texture.textureID);
            textureMap.erase(it);
            return true;
        }
    }

    return false;
}

bool TextureManager::GetTexture(const std::string& texturePath, Texture& outTexture)
{
	if (HasTexture(texturePath))
	{
        outTexture = textureMap[texturePath];
        return true;
	}
	
    return false;
}
