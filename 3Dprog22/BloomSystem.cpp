#include "BloomSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"

void BloomSystem::Init()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();
	auto widthDownsampled = width / bloom.downsample;
	auto heightDownsampled = height / bloom.downsample;

	re->GenerateTexture("BloomBlur", bloom.blur);
	re->GenerateTexture("BloomBlurUpscaled", bloom.blurUpscaled);

	re->Bind2DTexture(bloom.blur);
	re->CreateTexture2D(bloom.blur, GL_RGBA16F, GL_RGBA, widthDownsampled, heightDownsampled, GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->Bind2DClampToEdge();

	re->Bind2DTexture(bloom.blurUpscaled);
	re->CreateTexture2D(bloom.blurUpscaled, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->Bind2DClampToEdge();
}

void BloomSystem::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	auto widthDownsampled = width / bloom.downsample;
	auto heightDownsampled = height / bloom.downsample;

	re->Bind2DTexture(bloom.blur);
	re->CreateTexture2D(bloom.blur, GL_RGBA16F, GL_RGBA, widthDownsampled, heightDownsampled, GL_FLOAT);

	re->Bind2DTexture(bloom.blurUpscaled);
	re->CreateTexture2D(bloom.blurUpscaled, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
}

void BloomSystem::Clean()
{
	auto* re = RenderEngine::Get();
	re->DeleteTexture("BloomBlur");
	re->DeleteTexture("BloomBlurUpscaled");
}
