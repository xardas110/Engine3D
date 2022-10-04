#include "OITSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include <vector>

void OITSystem::Init()
{
	initializeOpenGLFunctions();
	InitIndexBuffer();
	InitAtomicBuffer();
	InitSSBO();
	InitClearTexture();
}

void OITSystem::InitIndexBuffer()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();
	auto* tm = re->GetTextureManager();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	re->GenerateTexture("OITIndexBuffer", aBuffer.indexBuffer);
	re->Bind2DTexture(aBuffer.indexBuffer);
	re->CreateImage2D(aBuffer.indexBuffer, GL_R32UI, GL_RED_INTEGER, 1920, 1080, GL_UNSIGNED_INT);
	re->Bind2DNoFilter();
	re->BindImageTexture(0, aBuffer.indexBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
}

void OITSystem::InitAtomicBuffer()
{
	auto* re = RenderEngine::Get();
	
	unsigned cb;
	glGenBuffers(1, &cb);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, cb);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
	aBuffer.acbo.id = cb;
}

void OITSystem::InitSSBO()
{
	auto* re = RenderEngine::Get();
	auto* ssbom = re->GetSSBOManager();

	aBuffer.ssbo = ssbom->CreateSSBO(
		"OITFragmentBuffer",
		sizeof(OITNode) * OIT_MAX_NODES,
		5);
}

void OITSystem::InitClearTexture()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	std::vector<unsigned> clear(width * height, OIT_CLEAR_VALUE);

	unsigned id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
	glBufferData(
		GL_PIXEL_UNPACK_BUFFER,
		clear.size() * sizeof(unsigned),
		&clear[0], GL_STATIC_COPY);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	aBuffer.pubo.id = id;
}

void OITSystem::BindShaderToBlock(const Shader& shader)
{
	auto* re = RenderEngine::Get();
	auto* ssbom = re->GetSSBOManager();
	re->BindSSBO(aBuffer.ssbo);
	ssbom->BindShaderToBlock(shader.shaderId, aBuffer.ssbo.id, 5, "OITSSBO");
}

void OITSystem::Clear()
{
	auto* rw = RenderWindow::Get();
	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	glBindTexture(GL_TEXTURE_2D, aBuffer.indexBuffer.textureID);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, aBuffer.pubo.id);
	glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	
	unsigned zero = 0;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, aBuffer.acbo.id);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned), &zero);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
	
}

void OITSystem::OnResize(unsigned width, unsigned height)
{
	auto* re = RenderEngine::Get();
	auto* tm = re->GetTextureManager();

	//index buffer
	re->Bind2DTexture(aBuffer.indexBuffer);
	re->CreateImage2D(aBuffer.indexBuffer, GL_R32UI, GL_RED_INTEGER, width, height, GL_UNSIGNED_INT);

	//clear texture
	std::vector<unsigned> clear(width * height, OIT_CLEAR_VALUE);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, aBuffer.pubo.id);
	glBufferData(
		GL_PIXEL_UNPACK_BUFFER,
		clear.size() * sizeof(unsigned),
		clear.data(), GL_STATIC_COPY);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void OITSystem::Clean()
{
	auto* re = RenderEngine::Get();
	auto* ssbom = re->GetSSBOManager();

	re->DeleteTexture("OITIndexBuffer");
	ssbom->DeleteSSBO("OITSSBO");

	glDeleteBuffers(1, &aBuffer.acbo.id);
	glDeleteBuffers(1, &aBuffer.pubo.id);
}
