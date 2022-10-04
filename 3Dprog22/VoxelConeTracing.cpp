#include "VoxelConeTracing.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include "Include/glm/gtc/matrix_transform.hpp"

void VoxelConeTracing::Init(World* world)
{
	initializeOpenGLFunctions();
	InitVoxel();
	UpdateVoxelUBO();
	InitVoxelDisplay();
	InitVCT();
}

void VoxelConeTracing::UpdateVoxelUBO()
{
	auto* re = RenderEngine::Get();

	re->BindUBO(voxelStructure.ubo);
	re->BindUBOData(0, sizeof(VoxelGPUData), &voxelStructure.gpuData);
}

void VoxelConeTracing::InitVoxel()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::Voxel, voxelStructure.shader);
	re->GetShader(ShaderManager::VoxelMip, voxelStructure.mipShader);
	re->GetShader(ShaderManager::VoxelRadiance, voxelStructure.radianceShader);
	re->GetShader(ShaderManager::VoxelFirstBounce, voxelStructure.firstBounceShader);
	re->GetShader(ShaderManager::VoxelAnisoMip, voxelStructure.anisoMipShader);
	re->GetShader(ShaderManager::VoxelAnisoMipMips, voxelStructure.anisoMipMipsShader);
	re->GetShader(ShaderManager::VoxelClear, voxelStructure.clearShader);

	re->BindShader(voxelStructure.shader);
	voxelStructure.vb.model = re->GetUniformLocation("model");
	voxelStructure.vb.albedo = re->GetUniformLocation("albedo");
	voxelStructure.vb.numAlbedo = re->GetUniformLocation("numAlbedo");
	voxelStructure.vb.shadowmaps = re->GetUniformLocation("shadowmaps");

	re->BindShader(voxelStructure.radianceShader);
	voxelStructure.vrb.shadowMaps = re->GetUniformLocation("shadowMaps");

	re->BindShader(voxelStructure.firstBounceShader);
	voxelStructure.vpib.voxelAlbedo = re->GetUniformLocation("voxelAlbedo");
	voxelStructure.vpib.voxelNormal = re->GetUniformLocation("voxelNormal");

	for (size_t i = 0; i < 6; i++)
	{
		std::string location = "voxelMips[" + std::to_string(i) + "]";
		voxelStructure.vpib.voxelMips[i] = re->GetUniformLocation(location);
	}
	
	re->BindShader(voxelStructure.anisoMipShader);
	voxelStructure.amb.source = re->GetUniformLocation("source");

	re->BindShader(voxelStructure.anisoMipMipsShader);
	voxelStructure.mmb.mipLevel = re->GetUniformLocation("mipLevel");

	auto& dims = voxelStructure.gpuData.dims;

	for (size_t i = 0; i < voxelStructure.mips.size(); i++)
	{
		std::string location = "source[" + std::to_string(i) + "]";
		voxelStructure.mmb.source[i] = re->GetUniformLocation(location);
	}

	{ //main texture
		unsigned id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexStorage3D(GL_TEXTURE_3D, voxelStructure.numBaseLods + 1, GL_RGBA8, dims.x, dims.y, dims.z);

		voxelStructure.texture.textureID = id;
		voxelStructure.texture.width = dims.x;
		voxelStructure.texture.height = dims.y;
		voxelStructure.texture.dimensions = dims.z;
		voxelStructure.texture.format = GL_RGBA8;
	}
	{ //albedo
		unsigned id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, dims.x, dims.y, dims.z);

		voxelStructure.albedo.textureID = id;
		voxelStructure.albedo.width = dims.x;
		voxelStructure.albedo.height = dims.y;
		voxelStructure.albedo.dimensions = dims.z;
		voxelStructure.albedo.format = GL_RGBA8;
	}
	{ // normal
		unsigned id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, dims.x, dims.y, dims.z);

		voxelStructure.normal.textureID = id;
		voxelStructure.normal.width = dims.x;
		voxelStructure.normal.height = dims.y;
		voxelStructure.normal.dimensions = dims.z;
		voxelStructure.normal.format = GL_RGBA8;
	}

	for (size_t i = 0; i < voxelStructure.mips.size(); i++)
	{
		unsigned id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexStorage3D(GL_TEXTURE_3D, voxelStructure.numMipLods + 1, GL_RGBA8, dims.x / 2, dims.y / 2, dims.z / 2);

		voxelStructure.mips[i].textureID = id;
		voxelStructure.mips[i].width = dims.x / 2;
		voxelStructure.mips[i].height = dims.y / 2;
		voxelStructure.mips[i].dimensions = dims.z / 2;
		voxelStructure.mips[i].format = GL_RGBA8;
	}

	ClearVoxel();

	glm::vec3 gridSize = voxelStructure.gpuData.gridSize;
	glm::vec3 halfSize = glm::vec3(gridSize) / 2.f;

	//Create voxel projections
	glm::mat4 projection = glm::ortho(-halfSize.x, halfSize.x,
		-halfSize.y, halfSize.y,
		0.f, gridSize.z);

	voxelStructure.gpuData.projX = projection * glm::lookAt(
		glm::vec3(halfSize.x, 0.f, 0.f),
		glm::vec3(0.f),
		glm::vec3(0.f, 1.f, 0.f));

	voxelStructure.gpuData.projY = projection * glm::lookAt(
		glm::vec3(0.f, halfSize.y, 0.f),
		glm::vec3(0.f),
		glm::vec3(0.f, 0.f, -1.f));

	voxelStructure.gpuData.projZ = projection * glm::lookAt(
		glm::vec3(0.f, 0.f, halfSize.z),
		glm::vec3(0.f),
		glm::vec3(0.f, 1.f, 0.f));

	auto* uboManager = re->GetUBOManager();
	voxelStructure.ubo = uboManager->CreateUBO("VoxelData", sizeof(VoxelGPUData), 6);

	UpdateVoxelUBO();
}

void VoxelConeTracing::InitVoxelDisplay()
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::VoxelDisplay2, voxelDisplay2.shader);
	re->BindShader(voxelDisplay2.shader);

	voxelDisplay2.vdb.model = re->GetUniformLocation("model");
	voxelDisplay2.vdb.voxelStructure = re->GetUniformLocation("voxelStructure");
}

void VoxelConeTracing::InitVCT()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto width = (float)rw->GetWidth() * vct.downScale;
	auto height = (float)rw->height() * vct.downScale;

	re->GetShader(ShaderManager::VoxelConeTrace, vct.shader);
	re->BindShader(vct.shader);

	vct.bind.gBufferB0 = re->GetUniformLocation("gBuffer.b0");
	vct.bind.depth = re->GetUniformLocation("gBuffer.depth");

	vct.bind.voxelStructure = re->GetUniformLocation("voxelStructure");

	for (size_t i = 0; i < voxelStructure.mips.size(); i++)
	{
		std::string location = "voxelMips[" + std::to_string(i) + "]";
		vct.bind.mips[i] = re->GetUniformLocation(location);
	}

	re->GenerateFBO("VCT", vct.fbo);
	re->BindFrameBuffer(vct.fbo);

	re->GenerateTexture("VCTDiffuseAO", vct.il.diffuseAO);
	re->GenerateTexture("VCTDiffuseAOFiltered", vct.il.diffuseAOFiltered);
	re->GenerateTexture("VCTSpecularShadow", vct.il.specularShadow);

	re->Bind2DTexture(vct.il.diffuseAO);
	re->CreateTexture2D(vct.il.diffuseAO, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DNoFilter();

	re->Bind2DTexture(vct.il.diffuseAOFiltered);
	re->CreateTexture2D(vct.il.diffuseAOFiltered, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DNoFilter();

	re->Bind2DTexture(vct.il.specularShadow);
	re->CreateTexture2D(vct.il.specularShadow, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DNoFilter();

	re->BindFrameBuffer(vct.fbo);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vct.il.diffuseAO, 0);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, vct.il.specularShadow, 0);

	unsigned buffers[] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, buffers);
}


void VoxelConeTracing::Update(World* world, float deltatime)
{
	UpdateVoxelUBO();
}

void VoxelConeTracing::RenderVoxelStructure(const Mesh& screenQuad)
{
	auto* re = RenderEngine::Get();
	
	re->BindFrameBuffer(0);
	re->GLClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	re->EnableCulling();
	re->EnableDepthTest();
	re->BindShader(voxelDisplay2.shader);
	re->SetTexture(voxelStructure.texture,
		GL_TEXTURE_3D,
		voxelDisplay2.vdb.voxelStructure,
		0);
	re->DrawElements(screenQuad, GL_TRIANGLES);
	re->EnableCulling();
}

void VoxelConeTracing::BindVoxelStructure(int lodLevel)
{
	auto* re = RenderEngine::Get();
	auto* world = World::Get();

	DirLight light;
	world->GetDirectionalLight(light);

	re->DisableCulling();
	re->DisableDepthTest();

	re->SetViewport(0, 0, voxelStructure.texture.width, voxelStructure.texture.height);

	re->BindShader(voxelStructure.shader);

	re->BindImageTexture(0, voxelStructure.albedo, 0, true, 0, GL_READ_WRITE, GL_R32UI);
	re->BindImageTexture(1, voxelStructure.normal, 0, true, 0, GL_READ_WRITE, GL_R32UI);

	re->SetTexture(light.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, voxelStructure.vb.shadowmaps, 6);
}

void VoxelConeTracing::UnbindVoxelStructure()
{
	auto* rw = RenderWindow::Get();
	auto* re = RenderEngine::Get();

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->EnableCulling();
	re->EnableDepthTest();
}

void VoxelConeTracing::GenerateMipMap()
{
	//GenerateLinearMips();
	GenerateAllAnisoMips();
}

void VoxelConeTracing::GenerateLinearMips()
{
	auto* re = RenderEngine::Get();
	re->BindShader(voxelStructure.mipShader);

	auto& dims = voxelStructure.gpuData.dims;
	for (int i = 0; i <= voxelStructure.numBaseLods; i++)
	{
		int mipScale = pow(2, i + 1);
		auto mipRes = dims / mipScale;

		re->BindImageTexture(0,
			voxelStructure.texture,
			i, true, 0, GL_READ_ONLY,
			voxelStructure.texture.format);

		re->BindImageTexture(1,
			voxelStructure.texture,
			i + 1, true, 0, GL_WRITE_ONLY,
			voxelStructure.texture.format);

		re->DispatchCompute(mipRes.x / 8, mipRes.y / 8, mipRes.z / 8);
	}
}

void VoxelConeTracing::GenerateAnisoMips()
{
	auto* re = RenderEngine::Get();
	re->BindShader(voxelStructure.anisoMipShader);

	auto& dims = voxelStructure.gpuData.dims;
	auto mipRes = dims / 2;

	re->SetTexture(voxelStructure.texture, GL_TEXTURE_3D, voxelStructure.amb.source, 0);

	for (int i = 0; i < voxelStructure.mips.size(); i++)
	{
		re->BindImageTexture(i+1,
			voxelStructure.mips[i],
			0, true, 0, GL_WRITE_ONLY,
			voxelStructure.mips[i].format);
	}
	re->DispatchCompute(mipRes.x / 8, mipRes.y / 8, mipRes.z / 8);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void VoxelConeTracing::GenerateAnisoMipMips()
{
	auto* re = RenderEngine::Get();
	re->BindShader(voxelStructure.anisoMipMipsShader);

	auto& dims = voxelStructure.gpuData.dims;
	auto mipRes = dims / 2; // base=512->256

	for (size_t mip = 1; mip <= voxelStructure.numMipLods; mip++)
	{
		mipRes /= 2; // first mip level is 256->128

		re->SetInt(voxelStructure.mmb.mipLevel, mip - 1);

		for (int i = 0; i < voxelStructure.mips.size(); i++)
		{		
			re->SetTexture(voxelStructure.mips[i], GL_TEXTURE_3D, voxelStructure.mmb.source[i], 5 + i);

			re->BindImageTexture(i,
				voxelStructure.mips[i],
				mip, true, 0, GL_WRITE_ONLY,
				voxelStructure.mips[i].format);
		}
		re->DispatchCompute(mipRes.x / 8, mipRes.y / 8, mipRes.z / 8);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);	
	}
}

void VoxelConeTracing::GenerateAllAnisoMips()
{
	GenerateAnisoMips();
	GenerateAnisoMipMips();
}

void VoxelConeTracing::InjectDirectLight()
{
	auto* re = RenderEngine::Get();
	auto* world = World::Get();

	DirLight light;
	world->GetDirectionalLight(light);

	auto& dims = voxelStructure.gpuData.dims;
	re->BindShader(voxelStructure.radianceShader);	
	re->BindImageTexture(0, voxelStructure.albedo, 0, true, 0, GL_READ_ONLY, GL_RGBA8);
	re->BindImageTexture(1, voxelStructure.normal, 0, true, 0, GL_READ_WRITE, GL_RGBA8);
	re->BindImageTexture(2, voxelStructure.texture, 0, true, 0, GL_WRITE_ONLY, GL_RGBA8);
	re->SetTexture(light.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, voxelStructure.vrb.shadowMaps, 3);
	re->DispatchCompute(std::ceil(dims.x / 8), std::ceil(dims.y / 8), std::ceil(dims.z / 8));
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void VoxelConeTracing::InjectFirstBounce()
{
	auto* re = RenderEngine::Get();
	auto* world = World::Get();

	DirLight light;
	world->GetDirectionalLight(light);

	auto& dims = voxelStructure.gpuData.dims;
	re->BindShader(voxelStructure.firstBounceShader);
	re->BindImageTexture(0, voxelStructure.texture, 0, true, 0, GL_WRITE_ONLY, GL_RGBA8);
	re->SetTexture(voxelStructure.albedo, GL_TEXTURE_3D, voxelStructure.vpib.voxelAlbedo, 1);
	re->SetTexture(voxelStructure.normal, GL_TEXTURE_3D, voxelStructure.vpib.voxelNormal, 2);

	for (size_t i = 0; i < 6; i++)
	{
		re->SetTexture(voxelStructure.mips[i], GL_TEXTURE_3D, voxelStructure.vpib.voxelMips[i], 3+i);
	}

	re->DispatchCompute(std::ceil(dims.x / 8), std::ceil(dims.y / 8), std::ceil(dims.z / 8));
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void VoxelConeTracing::TraceCones(const GBuffer& gbuffer, const Mesh& screenQuad)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->BindShader(vct.shader);
	re->SetTexture(gbuffer.b0, GL_TEXTURE_2D, vct.bind.gBufferB0, 0);
	re->SetTexture(gbuffer.fbo.texture, GL_TEXTURE_2D, vct.bind.depth, 1);

	/*Needs bindings points for all its mips?*/
	re->SetTexture(voxelStructure.texture, GL_TEXTURE_3D, vct.bind.voxelStructure, 2);
	
	for (size_t i = 0; i < voxelStructure.mips.size(); i++) // 6
	{
		re->SetTexture(voxelStructure.mips[i], GL_TEXTURE_3D, vct.bind.mips[i], 3+i);
	}
	//next tex slot = 9

	re->BindFrameBuffer(vct.fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT);
	re->SetViewport(0,0, vct.il.diffuseAO.width, vct.il.diffuseAO.height);

	re->DrawElements(screenQuad, GL_TRIANGLES);

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->BindFrameBuffer(0);
}

void VoxelConeTracing::ClearVoxel()
{
	auto* re = RenderEngine::Get();

	re->BindShader(voxelStructure.clearShader);

	re->BindImageTexture(0, voxelStructure.texture, 0, true, 0, GL_WRITE_ONLY, voxelStructure.texture.format);
	re->BindImageTexture(1, voxelStructure.albedo, 0, true, 0, GL_WRITE_ONLY, voxelStructure.texture.format);
	re->BindImageTexture(2, voxelStructure.normal, 0, true, 0, GL_WRITE_ONLY, voxelStructure.texture.format);

	glm::vec4 dims = voxelStructure.gpuData.dims;
	re->DispatchCompute(ceil(dims.x / 8.f), ceil(dims.y / 8.f), ceil(dims.z / 8.f));
}

void VoxelConeTracing::Clean()
{
	auto* re = RenderEngine::Get();

	glDeleteTextures(1, &voxelStructure.texture.textureID);

	for (size_t i = 0; i < voxelStructure.mips.size(); i++)
	{
		glDeleteTextures(1, &voxelStructure.mips[i].textureID);
	}

	glDeleteBuffers(1, &voxelStructure.ubo.UBOId);

	re->DeleteFBO("VCT");
	re->DeleteTexture("VCTDiffuseAO");
	re->DeleteTexture("VCTSpecularShadow");
	re->DeleteTexture("VCTDiffuseAOFiltered");
}

void VoxelConeTracing::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	float dsw = width * vct.downScale;
	float dsh = height * vct.downScale;

	re->Bind2DTexture(vct.il.diffuseAO);
	re->CreateTexture2D(vct.il.diffuseAO, GL_RGBA16F, GL_RGBA, dsw, dsh, GL_FLOAT);

	re->Bind2DTexture(vct.il.diffuseAOFiltered);
	re->CreateTexture2D(vct.il.diffuseAOFiltered, GL_RGBA16F, GL_RGBA, dsw, dsh, GL_FLOAT);

	re->Bind2DTexture(vct.il.specularShadow);
	re->CreateTexture2D(vct.il.specularShadow, GL_RGBA16F, GL_RGBA, dsw, dsh, GL_FLOAT);
}
