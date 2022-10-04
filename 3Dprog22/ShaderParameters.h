#pragma once
#include "GlobalTypeDefs.h"
#include <fstream>
#include <iostream>
#include <sstream>
///Bind shader defines at compile time

#define BEGIN_SHADER(path) \
	{\
	std::string shaderCode;\
	std::ifstream ifShaderStream;\
	std::ofstream ofShaderStream;\
	std::stringstream ssShader;\
	\
	const std::string shaderPath = path;\
	\
	ifShaderStream.open(shaderPath);\
	if (!ifShaderStream)\
	{\
	    std::cout << "Shader failed to open: " << shaderPath << std::endl;\
	}\
	else\
	{\
	    ssShader << ifShaderStream.rdbuf();\
	    shaderCode = ssShader.str();

#define END_SHADER(path)\
	}\
	ifShaderStream.close();\
	std::cout << "Ended writing to shader: " << (path) << std::endl;\
	}


#define SHADER_DEFINE(NAME)\
	{\
		std::string type = "#define";\
		\
		std::string variableName = #NAME;\
		\
		std::string findString = type + " " + variableName;\
		\
		std::string replacementString = type + " " + variableName + " " + std::to_string(NAME);\
		\
		\
		size_t findPos = shaderCode.find(findString);\
		size_t lastPos = shaderCode.find_first_of("\n", findPos);\
		\
		bool bFound = findPos != std::numeric_limits<size_t>().max();\
		\
		if (bFound)\
		{\
		    auto firstIt = shaderCode.begin() + findPos;\
		    auto endIt = shaderCode.begin() + lastPos;\
		\
		    shaderCode.replace(firstIt, endIt, replacementString);\
		\
		    ofShaderStream.open(shaderPath);\
		    if (ofShaderStream)\
		    {\
		        ofShaderStream.write(shaderCode.c_str(), shaderCode.size());\
		        ofShaderStream.close();\
		        std::cout << "Successfully wrote to the shader: " << #NAME << " " << (NAME) << " at path: " << shaderPath << std::endl;\
		    }\
		    else\
		    {\
		        std::cout << "Failed to open shader for write: " << shaderPath << std::endl;\
		    }\
		}\
		else\
		{\
		    std::cout << "Variable not found! " << findString << std::endl;\
		}\
	}\


//INSERT SHADER DEFINES

#define BEGIN_SHADER_WRITES()\
{\
	BEGIN_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/ShadowMap.geom")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/ShadowMap.geom")\
	BEGIN_SHADER("../3Dprog22/Shaders/StandardForward.frag")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/StandardForward.frag")\
	BEGIN_SHADER("../3Dprog22/Shaders/Terrain/Terrain.frag")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/Terrain/Terrain.frag")\
	BEGIN_SHADER("../3Dprog22/Shaders/Vegetation/Vegetation.frag")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/Vegetation/Vegetation.frag")\
	BEGIN_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/Vegetation.geom")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/Vegetation.geom")\
	BEGIN_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.geom")\
	SHADER_DEFINE(MAX_CASCADES)\
	END_SHADER("../3Dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.geom")\
	BEGIN_SHADER("../3Dprog22/Shaders/PostProcess/SSAO/SSAO.frag")\
	SHADER_DEFINE(SSAO_KERNEL_SIZE)\
	END_SHADER("../3Dprog22/Shaders/PostProcess/SSAO/SSAO.frag")\

#define END_SHADER_WRITES()\
}

/* for easier debugging, cause macros suck to read
    std::string shaderCode;
    std::ifstream ifShaderStream;
    std::ofstream ofShaderStream;
    std::stringstream ssShader;

    const std::string shaderPath = "../3Dprog22/Shaders/ShadowPass/DirLight/ShadowMap.geom";

    ifShaderStream.open(shaderPath);
    if (!ifShaderStream)
    {
        std::cout << "Shader failed to open: " << shaderPath << std::endl;
    }
    else
    {
        ssShader << ifShaderStream.rdbuf();
        shaderCode = ssShader.str();

        std::string type = "#define";

        std::string variableName = "MAX_CASCADES";

        std::string findString = type + " " + variableName;

        std::string replacementString = "#define MAX_CASCADES 8";


        size_t findPos = shaderCode.find(findString);
        size_t lastPos = shaderCode.find_first_of("\n", findPos);

        bool bFound = findPos != std::numeric_limits<size_t>().max();

        if (bFound)
        {
            auto firstIt = shaderCode.begin() + findPos;
            auto endIt = shaderCode.begin() + lastPos;

            shaderCode.replace(firstIt, endIt, replacementString);

            ofShaderStream.open(shaderPath);
            if (ofShaderStream)
            {
                ofShaderStream.write(shaderCode.c_str(), shaderCode.size());
                ofShaderStream.close();
                std::cout << "Successfully wrote to the shader" << std::endl;
            }
            else
            {
                std::cout << "Failed to open shader for write: " << shaderPath << std::endl;
            }
        }
        else
        {
            std::cout << "Variable not found! " << findString << std::endl;
        }
    }

    ifShaderStream.close();
    */