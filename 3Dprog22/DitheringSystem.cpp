#include "DitheringSystem.h"
#include "RenderEngine.h"
#include "texturemanager.h"
#include "renderwindow.h"

void DitheringSystem::Init(TextureManager* textureManager)
{
    auto* re = RenderEngine::Get();

    re->GetShader(ShaderManager::DitheringFilter, dithering.shader);

    re->BindShader(dithering.shader);
    dithering.db.source = re->GetUniformLocation("source");
    dithering.db.noiseTexture = re->GetUniformLocation("noiseTexture");

    textureManager->LoadTexture("../3Dprog22/Assets/Textures/Noise/Noise.png", dithering.noise);
}

void DitheringSystem::Filter(const Texture& source, const Texture& target, const Mesh& quad)
{
    auto* re = RenderEngine::Get();
    auto* rw = RenderWindow::Get();

    re->SetViewport(0, 0, target.width, target.height);

    re->BindShader(dithering.shader);
    re->DisableDepthTest();
    re->DisableBlending();
    re->SetTexture(source, GL_TEXTURE_2D, dithering.db.source, 0);
    re->SetTexture(dithering.noise, GL_TEXTURE_2D, dithering.db.noiseTexture, 1);
    re->BindImageTexture(3, target, 0, false, 0, GL_WRITE_ONLY, GL_RGBA16F);
    re->DrawElements(quad, GL_TRIANGLES);

    re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
}

void DitheringSystem::Clean()
{
    //nothing to clean
}
