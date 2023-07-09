#include "ShaderManager.h"
#include "logger.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "Include/glm/gtc/type_ptr.hpp"

bool HasPath(const std::string& path, std::vector<std::string>& pragmaOnce)
{
    for (const auto& p : pragmaOnce)
    {
        if (p == path) return true;
    }
    return false;
}


ShaderManager::ShaderManager()
{
    //Loading all shaders here, might change it later, right now its convinient.

    LoadShader(ShaderType::Standard,
        "../3dprog22/plainshader.vert",
        "../3dprog22/plainshader.frag");

    LoadShader(ShaderType::Skybox, 
        "../3dprog22/Shaders/Sky/Skybox/SkyBox.vert",
        "../3dprog22/Shaders/Sky/Skybox/SkyBox.frag");

    LoadShader(ShaderType::DefaultDebug,
        "../3dprog22/Shaders/Debug/DefaultDebug.vert",
        "../3dprog22/Shaders/Debug/DefaultDebug.frag");

    LoadShader(ShaderType::Sun, 
        "../3dprog22/Shaders/Sky/Sun/Sun.vert",
        "../3dprog22/Shaders/Sky/Sun/Sun.frag");

    LoadShader(ShaderType::LensFlare, 
        "../3dprog22/Shaders/Sky/Lens Flare/LensFlare.vert",
        "../3dprog22/Shaders/Sky/Lens Flare/LensFlare.frag");

    Shader volumetric = LoadShader(ShaderType::VolumetricLight, 
        "../3dprog22/Shaders/PostProcess/VolumetricLight/VolumetricLight.vert",
        "../3dprog22/Shaders/PostProcess/VolumetricLight/VolumetricLight.frag");

    Shader dithering = LoadShader(ShaderType::DitheringFilter,
        "../3dprog22/Shaders/Filters/Dithering/Dithering.vert",
        "../3dprog22/Shaders/Filters/Dithering/Dithering.frag");

    Shader skyDome = LoadShader(
        ShaderType::SkyDome, 
        "../3dprog22/Shaders/Sky/SkyDome/SkyDome.vert",
        "../3dprog22/Shaders/Sky/SkyDome/SkyDome.frag");

    Shader standardForward =  LoadShader(ShaderType::StandardTextured, 
        "../3dprog22/Shaders/StandardForward.vert",
        "../3dprog22/Shaders/StandardForward.frag");

    LoadShader(ShaderType::Debug, 
        "../3dprog22/Shaders/Debug.vert",
        "../3dprog22/Shaders/Debug.frag");

    LoadShader(ShaderType::SkeletalMesh, 
        "../3dprog22/Shaders/SkeletalMesh.vert",
        "../3dprog22/Shaders/StandardForward.frag");

    Shader dldp = LoadShader(ShaderType::DirectionalLightDepthPass,
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMap.vert",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMap.frag",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMap.geom");

    Shader dldpSM = LoadShader(ShaderType::DirectionalLightDepthPassSM,
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMapSM.vert",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMap.frag",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMap.geom");

    Shader forwardInstancedSM = LoadShader(ShaderType::ForwardInstancedSM,
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMapInstanced.vert",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMapInstanced.frag",
        "../3dprog22/Shaders/ShadowPass/DirLight/ShadowMapInstanced.geom");

    Shader vegetationSM = LoadShader(ShaderType::VegetationSM,
        "../3dprog22/Shaders/ShadowPass/DirLight/Vegetation.vert",
        "../3dprog22/Shaders/ShadowPass/DirLight/Vegetation.frag",
        "../3dprog22/Shaders/ShadowPass/DirLight/Vegetation.geom");

   Shader terrain = LoadShader(ShaderType::Terrain,
        "../3dprog22/Shaders/Terrain/Terrain.vert",
        "../3dprog22/Shaders/Terrain/Terrain.frag",
        "../3dprog22/Shaders/Terrain/Terrain.geom",
        "../3dprog22/Shaders/Terrain/Terrain.tese",
        "../3dprog22/Shaders/Terrain/Terrain.tesc");

   Shader terrainSG = LoadShader(ShaderType::TerrainShadowGrid,
       "../3dprog22/Shaders/ShadowGrid/Terrain/TerrainSG.vert",
       "../3dprog22/Shaders/ShadowGrid/Terrain/TerrainSG.frag",
       "../3dprog22/Shaders/ShadowGrid/Terrain/TerrainSG.geom",
       "../3dprog22/Shaders/ShadowGrid/Terrain/TerrainSG.tese",
       "../3dprog22/Shaders/ShadowGrid/Terrain/TerrainSG.tesc");

   Shader terrainSM = LoadShader(ShaderType::TerrainShadowMap,
       "../3dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.vert",
       "../3dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.frag",
       "../3dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.geom",
       "../3dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.tese",
       "../3dprog22/Shaders/ShadowPass/DirLight/Terrain/TerrainSP.tesc");

    LoadShader(ShaderType::TerrainNormalCompute,
        "../3dprog22/Shaders/Terrain/TerrainNormal.comp");

    LoadShader(ShaderType::TerrainSplatCompute,
        "../3dprog22/Shaders/Terrain/TerrainSplat.comp");

    Shader forwardInstanced = LoadShader(ShaderType::ForwardInstanced, 
        "../3dprog22/Shaders/ForwardInstanced/ForwardInstanced.vert",
        "../3dprog22/Shaders/ForwardInstanced/ForwardInstanced.frag");

    Shader vegetation = LoadShader(ShaderType::Vegetation, 
        "../3dprog22/Shaders/Vegetation/Vegetation.vert",
        "../3dprog22/Shaders/vegetation/Vegetation.frag");

    Shader vegetationSG = LoadShader(ShaderType::VegetationSG,
        "../3dprog22/Shaders/ShadowGrid/Vegetation/VegetationSG.vert",
        "../3dprog22/Shaders/ShadowGrid/Vegetation/VegetationSG.frag",
        "../3dprog22/Shaders/ShadowGrid/Vegetation/VegetationSG.geom");

    Shader particle = LoadShader(ShaderType::Particle, 
        "../3dprog22/Shaders/Particle/Particle.vert",
        "../3dprog22/Shaders/Particle/Particle.frag");

    Shader deferredStandard = LoadShader(ShaderType::DeferredStandard,
        "../3dprog22/Shaders/DeferredShaders/StandardDeferred/StandardDeferred.vert",
        "../3dprog22/Shaders/DeferredShaders/StandardDeferred/StandardDeferred.frag");

    Shader lightPass = LoadShader(ShaderType::DeferredLightPass,
        "../3dprog22/Shaders/DeferredShaders/LightPass/LightPass.vert",
        "../3dprog22/Shaders/DeferredShaders/LightPass/LightPass.frag");

    Shader HDR = LoadShader(ShaderType::HDR,
        "../3dprog22/Shaders/HDR/HDR.vert",
        "../3dprog22/Shaders/HDR/HDR.frag");

    Shader wbClear = LoadShader(ShaderType::WBClearCompute,
        "../3dprog22/Shaders/WeightedBlend/WeightedBlendClear.comp");

    Shader bilateral = LoadShader(ShaderType::Bilateral,
        "../3dprog22/Shaders/Filters/Bilateral/Bilateral.vert",
        "../3dprog22/Shaders/Filters/Bilateral/Bilateral.frag");

    Shader bilateralCompute = LoadShader(ShaderType::BilateralCompute,
        "../3dprog22/Shaders/Filters/Bilateral/Bilateral.comp");

    Shader SMAA = LoadShader(ShaderType::SMAA,
        "../3dprog22/Shaders/Filters/SMAA/SMAA.vert",
        "../3dprog22/Shaders/Filters/SMAA/SMAA.frag");

    Shader blur = LoadShader(ShaderType::Blur,
        "../3dprog22/Shaders/Filters/Blur/Blur.vert",
        "../3dprog22/Shaders/Filters/Blur/Blur.frag");

    Shader fog = LoadShader(ShaderType::Fog,
        "../3dprog22/Shaders/PostProcess/Fog/Fog.vert",
        "../3dprog22/Shaders/PostProcess/Fog/Fog.frag");

    Shader ssao = LoadShader(ShaderType::SSAO,
        "../3dprog22/Shaders/PostProcess/SSAO/SSAO.vert",
        "../3dprog22/Shaders/PostProcess/SSAO/SSAO.frag");

    Shader gaussianBlur = LoadShader(ShaderType::GaussianBlur,
        "../3dprog22/Shaders/Filters/GaussianBlur/GaussianBlur.vert",
        "../3dprog22/Shaders/Filters/GaussianBlur/GaussianBlur.frag");
        
    Shader dof = LoadShader(ShaderType::DoF,
        "../3dprog22/Shaders/DoF/DoF.vert",
        "../3dprog22/Shaders/DoF/DoF.frag");

    Shader text = LoadShader(ShaderType::Text,
        "../3dprog22/Shaders/Text/Text.vert",
        "../3dprog22/Shaders/Text/Text.frag");

    Shader perlinWorley3D = LoadShader(ShaderType::PerlinWorley3D, 
        "../3dprog22/Shaders/Noise/perlinWorley3D.comp");

    Shader worley3D = LoadShader(ShaderType::Worley3D, 
        "../3dprog22/Shaders/Noise/worley3D.comp");

    Shader weather = LoadShader(ShaderType::Weather, 
        "../3dprog22/Shaders/Volumetric Clouds/weather.comp");

    Shader vc = LoadShader(ShaderType::VolumetricClouds, 
        "../3dprog22/Shaders/PostProcess/VolumetricClouds/VolumetricClouds.vert",
        "../3dprog22/Shaders/PostProcess/VolumetricClouds/VolumetricClouds.frag");

    Shader voxel = LoadShader(ShaderType::Voxel,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/Voxelize.vert",
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/Voxelize.frag",
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/Voxelize.geom");

    Shader voxelClear = LoadShader(ShaderType::VoxelClear,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelClear.comp");

    Shader voxelDisplay2 = LoadShader(ShaderType::VoxelDisplay2,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelDisplay2.vert",
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelDisplay2.frag");

    Shader voxelConeTrace = LoadShader(ShaderType::VoxelConeTrace,
        "../3dprog22/Shaders/VoxelConeTracing/VoxelConeTrace/VoxelConeTrace.vert",
        "../3dprog22/Shaders/VoxelConeTracing/VoxelConeTrace/VoxelConeTrace.frag");

    Shader voxelMip = LoadShader(ShaderType::VoxelMip,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelMip.comp");
    Shader voxelAnisoMip = LoadShader(ShaderType::VoxelAnisoMip,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelAnisoMip.comp");
    Shader voxelAnisoMipMips = LoadShader(ShaderType::VoxelAnisoMipMips,
        "../3dprog22/Shaders/VoxelConeTracing/Voxelize/VoxelAnisoMipMips.comp");

    Shader voxelRadiance = LoadShader(ShaderType::VoxelRadiance,
        "../3dprog22/Shaders/VoxelConeTracing/VoxelRadiance/VoxelRadiance.comp");

    Shader voxelFirstBounce = LoadShader(ShaderType::VoxelFirstBounce,
        "../3dprog22/Shaders/VoxelConeTracing/VoxelRadiance/VoxelFirstBounce.comp");

    LoadShader(ShaderType::Water,
        "../3dprog22/Shaders/Water/Water.vert",
        "../3dprog22/Shaders/Water/Water.frag");

    //shadow bindings
    uboManager.BindShaderToBlock(dldpSM.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(forwardInstancedSM.shaderId, 0, "CSMMatrices");    
    uboManager.BindShaderToBlock(vegetationSM.shaderId, 0, "CSMMatrices");

    //main bindings
    uboManager.BindShaderToBlock(terrain.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(forwardInstanced.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(vegetation.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(dldp.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(standardForward.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(volumetric.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(terrainSM.shaderId, 0, "CSMMatrices");
    uboManager.BindShaderToBlock(lightPass.shaderId, 0, "CSMMatrices");

    uboManager.BindShaderToBlock(
        standardForward.shaderId, 
        1, 
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        vc.shaderId,
        1,
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        lightPass.shaderId,
        1,
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        terrain.shaderId,
        1,
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        vegetation.shaderId,
        1,
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        standardForward.shaderId, 
        2, 
        "CameraUBO");

    uboManager.BindShaderToBlock(
        vc.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        lightPass.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        deferredStandard.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        forwardInstanced.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        terrain.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        terrainSM.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        skyDome.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        volumetric.shaderId,
        1,
        "DirlightUBO");

    uboManager.BindShaderToBlock(
        volumetric.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        text.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        fog.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        HDR.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        ssao.shaderId,
        2,
        "CameraUBO");

    uboManager.BindShaderToBlock(
        terrainSG.shaderId,
        3,
        "ShadowGridUBO");

    uboManager.BindShaderToBlock(
        HDR.shaderId,
        3,
        "ShadowGridUBO");

    uboManager.BindShaderToBlock(
        vegetationSG.shaderId,
        3,
        "ShadowGridUBO");

    std::cout << "Finished loading all shaders" << std::endl;
}

bool ShaderManager::StripIncludeLine(std::string& inOutline)
{
    std::cout << "running strip include " << std::endl;

    const std::string type = "#Include";

    auto wsIt = std::remove(inOutline.begin(), inOutline.end(), ' ');
    inOutline.erase(wsIt, inOutline.end());

    auto typeIt = inOutline.find_first_of('#', 0);
    auto typeBeg = inOutline.begin() + typeIt;
    auto typeEnd = typeBeg + type.size();
    inOutline.erase(typeBeg, typeEnd);

    auto aIt = std::remove(inOutline.begin(), inOutline.end(), '\"');
    inOutline.erase(aIt, inOutline.end());

    auto commentIt = inOutline.find("//");
    if (commentIt != std::numeric_limits<size_t>().max())
    {
        inOutline = std::string(inOutline.begin(), inOutline.begin() + commentIt);
        std::cout << "line stripped " << inOutline << std::endl;
    } 

    return true;
}

bool ShaderManager::LoadIncludeString(const std::string& path, std::string& outString)
{
    std::ifstream iStream(path);
    if (!iStream.is_open())
    {
        std::cout << "Failed to open include path: " << path << std::endl;
        return false;
    }

    outString = std::string((std::istreambuf_iterator<char>(iStream)), std::istreambuf_iterator<char>());

    iStream.close();

    return true;
}

const bool ShaderManager::LoadInclude(const std::string& relativePath, std::string& code, std::vector<std::string>& pragmaOnce)
{
    auto* logger = Logger::getInstance();

    std::string type = "#include";

    size_t findPos = code.find(type);
    size_t endPos = code.find("\n", findPos);

    bool bFound = findPos != std::numeric_limits<size_t>().max();
    if (!bFound)
    {
        return false;
    }

    std::string includeLine(code.begin() + findPos, code.begin() + endPos);
    if (!StripIncludeLine(includeLine))
    {
        std::cout << "include line is invalid" << std::endl;
        return false;
    }

    auto rpend = relativePath.find_last_of('/');
    if (rpend == std::numeric_limits<size_t>().max())
    { 
        std::cout << "seems like the relative path is invalid " << relativePath << std::endl;
        return false;
    }

    std::string fixedRelativePath(relativePath.begin(), relativePath.begin() + rpend + 1);
    std::string includeStringPath = fixedRelativePath + includeLine;

    auto firstIt = code.begin() + findPos;
    auto endIt = code.begin() + endPos;

    if (HasPath(includeStringPath, pragmaOnce))
    {
        std::cout << "Header already loaded at path: " << includeStringPath << std::endl;
        code.replace(firstIt, endIt, "");
        return false;
    }

    std::string includeCode;
    if (!LoadIncludeString(includeStringPath, includeCode))
    {
        std::cout << "Failed to open include: " << type + ' ' + includeLine << std::endl;
        return false;
    }

    pragmaOnce.emplace_back(includeStringPath);
  
    auto fil = includeLine.find_last_of('/');
    if (fil == std::numeric_limits<size_t>().max())
    {
        fixedRelativePath = fixedRelativePath + includeLine;
    }
    else
    { 
        auto fixedInc = std::string(includeLine.begin(), includeLine.begin() + fil + 1);
        fixedRelativePath = fixedRelativePath + fixedInc;
    }

    std::cout << "loading includes for a included file" << std::endl;
    LoadIncludes(fixedRelativePath, includeCode, pragmaOnce);

    code.replace(firstIt, endIt, includeCode);

    std::string success = "GLSL included: " + type + ' ' + includeLine;
    logger->logText(success, LogType::HIGHLIGHT);

    return true;
}

void ShaderManager::LoadIncludes(const std::string& relativePath, std::string& code, std::vector<std::string>& pragmaOnce)
{
    while (LoadInclude(relativePath, code, pragmaOnce));
}

GLuint ShaderManager::LoadShaderType(unsigned type, const std::string& path)
{
    auto* logger = Logger::getInstance();

    std::string shaderCode;
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    shaderFile.open(path);
    if (!shaderFile)
        logger->logText("ERROR SHADER FILE " + std::string(path) + " NOT SUCCESFULLY READ", LogType::REALERROR);

    // Read file's buffer contents into streams
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    shaderCode = shaderStream.str();

    std::vector<std::string> pragmaOnce;
    LoadIncludes(path, shaderCode, pragmaOnce);

    const GLchar* code = shaderCode.c_str();

    GLuint shader;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    // Print compile errors if any
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        Logger::getInstance()->logText("ERROR SHADER " + std::string(path) +
            " COMPILATION_FAILED\n" + infoLog, LogType::REALERROR);
        return 0xffffffff;
    }
    return shader;
}

Shader ShaderManager::LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    GLint success; 
    unsigned shaderId;
    GLchar infoLog[512];

    auto mLogger = Logger::getInstance();

    unsigned vertex = LoadShaderType(GL_VERTEX_SHADER, vertexPath);
    // Fragment Shader
    unsigned fragment = LoadShaderType(GL_FRAGMENT_SHADER, fragmentPath);
  
    // Shader Program linking
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glLinkProgram(shaderId);
    // Print linking errors if any
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

    //making nice output name for printing:
    std::string shadername{ vertexPath };
    shadername.resize(shadername.size() - 5); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
        mLogger->logText("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" +
            shadername + "\n   " + infoLog, LogType::REALERROR);
    }
    else
    {
        mLogger->logText("GLSL shader " + shadername + " was successfully compiled");
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    Shader shader;
    shader.shaderId = shaderId;
	
    shaderMap[shaderType] = shader;

    return shader;
}

Shader ShaderManager::LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    auto mLogger = Logger::getInstance();

    GLint success;
    unsigned shaderId;
    GLchar infoLog[512];

    unsigned vertex = LoadShaderType(GL_VERTEX_SHADER, vertexPath);
    unsigned fragment = LoadShaderType(GL_FRAGMENT_SHADER, fragmentPath);
    unsigned geo = LoadShaderType(GL_GEOMETRY_SHADER, geoPath);

    // Shader Program linking
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glAttachShader(shaderId, geo);

    glLinkProgram(shaderId);
    // Print linking errors if any
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

    //making nice output name for printing:
    std::string shadername{ vertexPath };
    shadername.resize(shadername.size() - 5); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
        mLogger->logText("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" +
            shadername + "\n   " + infoLog, LogType::REALERROR);
    }
    else
    {
        mLogger->logText("GLSL shader " + shadername + " was successfully compiled");
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geo);
	
    Shader shader;
    shader.shaderId = shaderId;

    shaderMap[shaderType] = shader;

    return shader;
}

Shader ShaderManager::LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath, const std::string& tesePath, const std::string& tescPath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    //Get the instance of the Output logger
    //Have to do this, else program will crash (or you have to put in nullptr tests...)
    auto mLogger = Logger::getInstance();

    GLint success;
    unsigned shaderId;
    GLchar infoLog[512];

    unsigned vertex = LoadShaderType(GL_VERTEX_SHADER, vertexPath);
    unsigned fragment = LoadShaderType(GL_FRAGMENT_SHADER, fragmentPath);
    unsigned geo = LoadShaderType(GL_GEOMETRY_SHADER, geoPath);
    unsigned tese = LoadShaderType(GL_TESS_EVALUATION_SHADER, tesePath);
    unsigned tesc = LoadShaderType(GL_TESS_CONTROL_SHADER, tescPath);

    // Shader Program linking
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glAttachShader(shaderId, geo);
    glAttachShader(shaderId, tese);
    glAttachShader(shaderId, tesc);

    glLinkProgram(shaderId);
    // Print linking errors if any
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

    //making nice output name for printing:
    std::string shadername{ vertexPath };
    shadername.resize(shadername.size() - 5); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
        mLogger->logText("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" +
            shadername + "\n   " + infoLog, LogType::REALERROR);
    }
    else
    {
        mLogger->logText("GLSL shader " + shadername + " was successfully compiled");
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geo);
    glDeleteShader(tese);
    glDeleteShader(tesc);

    Shader shader;
    shader.shaderId = shaderId;

    shaderMap[shaderType] = shader;

    return shader;
}

Shader ShaderManager::LoadShader(ShaderType shaderType, const std::string& computePath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    unsigned shaderId;
    auto mLogger = Logger::getInstance();
    // 2. Compile shaders
    GLuint compute;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    compute = LoadShaderType(GL_COMPUTE_SHADER, computePath);

    // Shader Program linking
    shaderId = glCreateProgram();
    glAttachShader(shaderId, compute);
    glLinkProgram(shaderId);
    // Print linking errors if any
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

    //making nice output name for printing:
    std::string shadername{ computePath };
    shadername.resize(shadername.size() - 5); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
        mLogger->logText("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" +
            shadername + "\n   " + infoLog, LogType::REALERROR);
    }
    else
    {
        mLogger->logText("GLSL shader " + shadername + " was successfully compiled");
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader(compute);

    Shader shader;
    shader.shaderId = shaderId;

    shaderMap[shaderType] = shader;

    return shader;
}

unsigned ShaderManager::CreateGeomShader(const char* path, unsigned shaderID)
{
    if (!path)    
        return -1;
    
    try {
        std::string geoCode;
        std::ifstream gShaderFile;
        gShaderFile.open(path);
    	
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
    	
        geoCode = gShaderStream.str();
    	
        const char* gShaderCode = geoCode.c_str();
    	
        unsigned short int geo;
        char infolog[512];
        geo = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geo, 1, &gShaderCode, NULL);
        glCompileShader(geo);
    	
        int success;
        glGetShaderiv(geo, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geo, 512, NULL, infolog);
            std::cerr << "ERROR::SHADER::GEOMETRY::COMPILE_ERROR\n" << infolog << std::endl;
        }
        glAttachShader(shaderID, geo);
        return geo;
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::GEOMETRY::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
        return -1;
    }
}

bool ShaderManager::HasShader(ShaderType type)
{
	if (shaderMap.find(type) != shaderMap.end())
		return true;
	
	return false;
}

bool ShaderManager::GetShader(ShaderType type, Shader& outShader)
{
	const auto beg = shaderMap.find(type);
	if (beg != shaderMap.end())
	{
		outShader = beg->second;
		return true;
	}
	
    return false;
}

void ShaderManager::Use(unsigned shaderId)
{
    glUseProgram(shaderId);
}

void ShaderManager::Use(const Shader& shader)
{
    Use(shader.shaderId);
}

void ShaderManager::BindMat4(const Shader& shader, const std::string& name, glm::mat4 mat)
{
    BindMat4(shader.shaderId, name, mat);
}

void ShaderManager::BindVec4(const Shader& shader, const std::string& name, glm::vec4 vec)
{
    BindVec4(shader.shaderId, name, vec);
}

void ShaderManager::BindVec3(const Shader& shader, const std::string& name, glm::vec3 vec)
{
    BindVec3(shader.shaderId, name, vec);
}

void ShaderManager::BindVec2(const Shader& shader, const std::string& name, glm::vec3 vec)
{
    BindVec2(shader.shaderId, name, vec);
}

void ShaderManager::BindVec2f(const Shader& shader, const std::string& name, glm::vec2 vec)
{
    glUniform2fv(glGetUniformLocation(shader.shaderId, name.c_str()), 1, glm::value_ptr(vec));
}

void ShaderManager::BindVec2i(const Shader& shader, const std::string& name, glm::vec2 vec)
{
    glm::vec<2, int> vec2 = { (int)vec.x, (int)vec.y };
    glUniform2iv(glGetUniformLocation(shader.shaderId, name.c_str()), 1, glm::value_ptr(vec2));
}

void ShaderManager::BindInt(const Shader& shader, const std::string& name, int id)
{
    BindInt(shader.shaderId, name, id);
}

void ShaderManager::BindFloat(const Shader& shader, const std::string& name, float id)
{
    BindFloat(shader.shaderId, name, id);
}

int ShaderManager::GetUniformLocation(const Shader& shader, const std::string& uniformName)
{
    return glGetUniformLocation(shader.shaderId, uniformName.c_str());
}

void ShaderManager::SetMat4(int bindingId, const glm::mat4& mat)
{
    glUniformMatrix4fv(bindingId, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderManager::SetVec4(int bindingId, const glm::vec4& vec)
{
    glUniform4fv(bindingId, 1, glm::value_ptr(vec));
}

void ShaderManager::SetVec3(int bindingId, const glm::vec3& vec)
{
    glUniform3fv(bindingId, 1, glm::value_ptr(vec));
}

void ShaderManager::SetVec2(int bindingId, const glm::vec2& vec)
{
    glUniform2fv(bindingId, 1, glm::value_ptr(vec));
}

void ShaderManager::SetInt(int bindingId, const int value)
{
    glUniform1i(bindingId, value);
}

void ShaderManager::SetFloat(int bindingId, const float value)
{
    glUniform1f(bindingId, value);
}

void ShaderManager::BindMat4(unsigned shaderId, const std::string& name, glm::mat4 mat)
{
    unsigned int bindID = glGetUniformLocation(shaderId, name.c_str());
    glUniformMatrix4fv(bindID, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderManager::BindVec4(unsigned shaderId, const std::string& name, glm::vec4 vec)
{
    glUniform4fv(glGetUniformLocation(shaderId, name.c_str()), 1, glm::value_ptr(vec));
}

void ShaderManager::BindVec3(unsigned shaderId, const std::string& name, glm::vec3 vec)
{
    glUniform3fv(glGetUniformLocation(shaderId, name.c_str()), 1, glm::value_ptr(vec));
}

void ShaderManager::BindVec2(unsigned shaderId, const std::string& name, glm::vec3 vec)
{
    glUniform2fv(glGetUniformLocation(shaderId, name.c_str()), 1, glm::value_ptr(vec));
}

void ShaderManager::BindInt(unsigned shaderId, const std::string& name, int id)
{
    glUniform1i(glGetUniformLocation(shaderId, name.c_str()), id);
}

void ShaderManager::BindFloat(unsigned shaderId, const std::string& name, float id)
{
    glUniform1f(glGetUniformLocation(shaderId, name.c_str()), id);
}

bool ShaderManager::FindShaderById(unsigned shaderId, Shader& outShader)
{
    for (const auto& shader : shaderMap)
    {
        if (shader.second.shaderId == shaderId)
        {
            outShader = shader.second;
            return true;
        }
    }
	
    return false;
}

ShaderManager::~ShaderManager()
{
	for (const auto& shader : shaderMap)
	{
        glDeleteShader(shader.second.shaderId);
	}
}
