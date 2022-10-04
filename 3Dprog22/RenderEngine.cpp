#include "RenderEngine.h"

RenderEngine* renderEngine = nullptr;

RenderEngine* RenderEngine::Get()
{
    return renderEngine;
}

RenderEngine::RenderEngine()
{
    renderEngine = this;
    initializeOpenGLFunctions();

    glDisable(GL_MULTISAMPLE);//currently not implemented TODO:Fix sampling

    glEnable(GL_DEPTH_TEST);
}

RenderEngine::~RenderEngine() 
{
    renderEngine = nullptr;
}

void RenderEngine::BindShader(const Shader& shader)
{
    shaderManager.Use(shader);
    boundShader = shader;
}

void RenderEngine::BindTexture(const Texture& texture, int textureType, const std::string& uniformName, int textureSlot)
{
    auto textureUnit = GL_TEXTURE0 + textureSlot;

    if (textureUnit > GL_TEXTURE31)
    {
        printf("Textureslots exeeded");
    }

    glActiveTexture(textureUnit);
    shaderManager.BindInt(boundShader, uniformName, textureSlot);
    glBindTexture(textureType, texture.textureID);
}

void RenderEngine::BindInt(const std::string& uniformName, int value)
{
    shaderManager.BindInt(boundShader, uniformName, value);
}

void RenderEngine::BindFloat(const std::string& uniformName, float value)
{
    shaderManager.BindFloat(boundShader, uniformName, value);
}

void RenderEngine::BindVec2(const std::string& uniformName, glm::vec2 value)
{
    shaderManager.BindVec2f(boundShader, uniformName, value);
}

void RenderEngine::BindVec3(const std::string& uniformName, glm::vec3 value)
{
    shaderManager.BindVec3(boundShader, uniformName, value);
}

void RenderEngine::BindVec4(const std::string& uniformName, glm::vec4 value)
{
    shaderManager.BindVec4(boundShader, uniformName, value);
}

void RenderEngine::BindMat4(const std::string& uniformName, glm::mat4 value)
{
    shaderManager.BindMat4(boundShader, uniformName, value);
}

int RenderEngine::GetUniformLocation(const std::string& uniformName)
{
    return shaderManager.GetUniformLocation(boundShader, uniformName);
}

void RenderEngine::SetInt(int bindingId, int value)
{
    shaderManager.SetInt(bindingId, value);
}

void RenderEngine::SetFloat(int bindingId, float value)
{
    shaderManager.SetFloat(bindingId, value);
}

void RenderEngine::SetVec2(int bindingId, const glm::vec2& value)
{
    shaderManager.SetVec2(bindingId, value);
}

void RenderEngine::SetVec3(int bindingId, const glm::vec3& value)
{
    shaderManager.SetVec3(bindingId, value);
}

void RenderEngine::SetVec4(int bindingId, const glm::vec4& value)
{
    shaderManager.SetVec4(bindingId, value);
}

void RenderEngine::SetMat4(int bindingId, const glm::mat4& value)
{
    shaderManager.SetMat4(bindingId, value);
}

void RenderEngine::SetTexture(const Texture& texture, int textureType, const int textureBindingId, int textureSlot)
{
    auto textureUnit = GL_TEXTURE0 + textureSlot;

    if (textureUnit > GL_TEXTURE31)
    {
        printf("Textureslots exeeded");
    }

    glActiveTexture(textureUnit);
    shaderManager.SetInt(textureBindingId, textureSlot);
    glBindTexture(textureType, texture.textureID);
}

void RenderEngine::DrawArrays(int drawType, int vao, int first, int count)
{
    glBindVertexArray(vao);
    glDrawArrays(drawType, first, count);
}

void RenderEngine::EnableWireframe()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RenderEngine::DisableWireframe()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderEngine::BindImageTexture(int bindingId, const Texture& texture, int lodLevel, bool layered, int layer, int access, int format)
{
    glBindImageTexture(bindingId, texture.textureID, lodLevel, layered, layer, access, format);
}

void RenderEngine::SetCulling(int mode)
{
    glEnable(GL_CULL_FACE);
    glCullFace(mode);
    glFrontFace(GL_CW);
}

void RenderEngine::DispatchCompute(int numX, int numY, int numZ)
{
    glDispatchCompute(numX, numY, numZ);
}

bool RenderEngine::GetShader(ShaderManager::ShaderType type, Shader& outShader)
{
    return shaderManager.GetShader(type, outShader);
}

void RenderEngine::BindUBO(UBO ubo)
{
    boundUBO = ubo;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.UBOId);
}

void RenderEngine::BindUBOData(unsigned offsetInBytes, unsigned sizeInBytes, void* data)
{
    shaderManager.uboManager.BindData(boundUBO, offsetInBytes, sizeInBytes, data);
}

void RenderEngine::BindSSBO(SSBO ssbo)
{
    boundSSBO = ssbo;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.id);
}

void RenderEngine::BindSSBOData(unsigned offsetInBytes, unsigned sizeInBytes, void* data)
{
    ssboManager.BindData(boundSSBO, offsetInBytes, sizeInBytes, data);
}

void RenderEngine::BindACBO(ACBO acbo, unsigned index)
{
    boundACBO = acbo;
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, acbo.id);
}

void RenderEngine::EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}

void RenderEngine::DisableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}

void RenderEngine::EnableCulling()
{
    glEnable(GL_CULL_FACE);
}

void RenderEngine::DisableCulling()
{
    glDisable(GL_CULL_FACE);
}

void RenderEngine::EnableBlending()
{
    glEnable(GL_BLEND);
}

void RenderEngine::DisableBlending()
{
    glDisable(GL_BLEND);
}

void RenderEngine::EnableStencil()
{
    glEnable(GL_STENCIL_TEST);
}

void RenderEngine::DisableStencil()
{
    glDisable(GL_STENCIL_TEST);
}

void RenderEngine::EnableSRGB()
{
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void RenderEngine::DisableSRGB()
{
    glDisable(GL_FRAMEBUFFER_SRGB);
}

void RenderEngine::SetStencilFunc(unsigned func, unsigned ref, unsigned mask)
{
    glStencilFunc(func, ref, mask);
}

void RenderEngine::SetStencilOp(unsigned fail, unsigned zFail, unsigned zPass)
{
    glStencilOp(fail, zFail, zPass);
}

void RenderEngine::SetBlendFunc(int sfactor, int dfactor)
{
    glBlendFunc(sfactor, dfactor);
}

void RenderEngine::SetBlendFunc(int drawBuffer, int mode, int mode1)
{
    glBlendFunci(drawBuffer, mode, mode1);
}

void RenderEngine::SetBlendEquation(int mode)
{
    glBlendEquation(mode);
}

void RenderEngine::SetStencilMask(unsigned mask)
{
    glStencilMask(mask);
}

void RenderEngine::SetDepthMask(int mode)
{
    glDepthMask(mode);
}

void RenderEngine::SetCullFace(int mode)
{
    glCullFace(mode);
}

void RenderEngine::SetDepthFunc(int mode)
{
    glDepthFunc(mode);
}

void RenderEngine::BindFrameBuffer(int id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void RenderEngine::BindFrameBuffer(FrameBuffer& fbo)
{
    boundFBO = fbo;
    BindFrameBuffer(fbo.fboId);
}

bool RenderEngine::GenerateFBO(const std::string& name, FrameBuffer& outFBO)
{
    return GetFBOManager()->GenerateFBO(name, outFBO);
}

bool RenderEngine::CreateImage2D(Texture& outTexture, unsigned internalFormat, unsigned format, unsigned width, unsigned height, unsigned type, unsigned border, unsigned level, const void* pixels)
{
    if (!outTexture.IsValid())
    {
        printf("Texture creation failed, texture is invalid \n");
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);

    outTexture.format = internalFormat;
    outTexture.formatType = format;
    outTexture.dataType = type;
    outTexture.width = width;
    outTexture.height = height;

    return true;
}

void RenderEngine::BindFrameBufferTexture2D(int attachment, int textureType, const Texture& texture, int level)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureType, texture.textureID, level);
}

bool RenderEngine::GenerateTexture(const std::string& name, Texture& outTexture)
{
    return textureManager.GenerateTexture(name, outTexture);
}

void RenderEngine::Bind2DTexture(const Texture& texture)
{
    boundTexture = texture;
    glBindTexture(GL_TEXTURE_2D, texture.textureID);
}

void RenderEngine::Bind2DBilinearFilter()
{
    boundTexture.filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderEngine::Bind2DNoFilter()
{
    boundTexture.filter = GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void RenderEngine::Bind2DClampToEdge()
{
    boundTexture.wrap = GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void RenderEngine::Bind2DWrapRepeat()
{
    boundTexture.wrap = GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderEngine::SetMemoryBarrier(unsigned mode)
{
    glMemoryBarrier(mode);
}

bool RenderEngine::DeleteFBO(const std::string& name)
{
    return GetFBOManager()->DeleteFBO(name);
}

bool RenderEngine::DeleteTexture(const std::string& name)
{
    return textureManager.DeleteTexture(name);
}

bool RenderEngine::CreateTexture2D(Texture& outTexture, unsigned internalFormat, unsigned format, unsigned width, unsigned height, unsigned type, unsigned border, unsigned level, const void* pixels)
{
    if (!outTexture.IsValid())
    {
        printf("Texture creation failed, texture is invalid \n");
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);

    outTexture.format = internalFormat;
    outTexture.formatType = format;
    outTexture.dataType = type;
    outTexture.width = width; 
    outTexture.height = height;
 
    return true;
}

void RenderEngine::SetViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void RenderEngine::ClearDepthBuffer()
{
    GLClear(GL_DEPTH_BUFFER_BIT);
}

void RenderEngine::ClearBuffer(int bufferType, int renderTarget, const float* val)
{
    glClearBufferfv(bufferType, renderTarget, val);
}

void RenderEngine::GLClear(int bit)
{
    glClear(bit);
}

void RenderEngine::DrawElements(const Mesh& mesh, int mode)
{
    glBindVertexArray(mesh.vao);
    glDrawElements(mode, mesh.indices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

}

void RenderEngine::DrawElementsBaseInstanced(const Mesh& mesh, unsigned mode, unsigned amount, unsigned baseInstance)
{
    glBindVertexArray(mesh.vao);
    glDrawElementsInstancedBaseInstance(mode, mesh.indices, GL_UNSIGNED_INT, 0, amount, baseInstance);
    glBindVertexArray(0);
}

void RenderEngine::DrawElementsInstanced(const Mesh& mesh, int mode, int amount)
{
    glBindVertexArray(mesh.vao);
    glDrawElementsInstanced(mode, mesh.indices, GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}

ShaderManager* RenderEngine::GetShaderManager()
{
    return &shaderManager;
}

UBOManager* RenderEngine::GetUBOManager()
{
    return &shaderManager.uboManager;
}

SSBOManager* RenderEngine::GetSSBOManager()
{
    return &ssboManager;
}

FrameBufferManager* RenderEngine::GetFBOManager()
{
    return &shaderManager.FBOManager;
}

TextureManager* RenderEngine::GetTextureManager()
{
    return &textureManager;
}
