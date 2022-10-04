#include "StaticMeshInstancedSystem.h"
#include "World.h"
#include "RenderEngine.h"
#include "Components.h"
#include "Components.h"
#include "StaticMeshInstanced.h"
#include "StaticMeshSystem.h"

void StaticMeshInstancedSystem::Init(World* world, entt::registry& registry)
{
	InitShaders();
	InitSSBO();
	InitShaderBindings();
}

void StaticMeshInstancedSystem::InitShaders()
{
	auto* renderEngine = RenderEngine::Get();

	if (!renderEngine->GetShader(ShaderManager::ShaderType::ForwardInstanced, mainPass))
	{
		printf("Failed to load instanced shader for main pass\n");
	}

	if (!renderEngine->GetShader(ShaderManager::ShaderType::ForwardInstancedSM, shadowPass))
	{
		printf("Failed to load instanced SM shader for shadow pass\n");
	}
}

void StaticMeshInstancedSystem::InitSSBO()
{
	auto* renderEngine = RenderEngine::Get();
	auto* ssboManager = renderEngine->GetSSBOManager();

	ssbo = ssboManager->CreateSSBO("InstancedMatrices2", MAX_INSTANCES * 64, 4);

	renderEngine->BindSSBO(ssbo);
	ssboManager->BindShaderToBlock(mainPass.shaderId, ssbo.id, 4, "instancedMatrices2");
	ssboManager->BindShaderToBlock(shadowPass.shaderId, ssbo.id, 4, "instancedMatrices2");
}

void StaticMeshInstancedSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();

	re->BindShader(mainPass);

	mpb.instanceOffset = re->GetUniformLocation("instanceOffset");
	mpb.color = re->GetUniformLocation("material.color");
	mpb.shininess = re->GetUniformLocation("material.shininess");

	mpb.meshSettingsbHasTransparency = re->GetUniformLocation("meshSettings.bHasTransparency");

	mpb.sms = re->GetUniformLocation("shadowMaps");
	mpb.cloudShadowVL = re->GetUniformLocation("cloudShadowVL");

	mpb.mib.ambientmap.texLoc = re->GetUniformLocation("material.AOmap");
	mpb.mib.ambientmap.numTex = re->GetUniformLocation("material.numAOmaps");

	mpb.mib.diffusemap.texLoc = re->GetUniformLocation("material.diffusemap");
	mpb.mib.diffusemap.numTex = re->GetUniformLocation("material.numDiffusemaps");

	mpb.mib.specularmap.texLoc = re->GetUniformLocation("material.specularmap");
	mpb.mib.specularmap.numTex = re->GetUniformLocation("material.numSpecularmaps");

	mpb.mib.normalmap.texLoc = re->GetUniformLocation("material.normalmap");
	mpb.mib.normalmap.numTex = re->GetUniformLocation("material.numNormalmaps");

	mpb.mib.heightmap.texLoc = re->GetUniformLocation("material.heightmap");
	mpb.mib.heightmap.numTex = re->GetUniformLocation("material.numHeigthmaps");

	mpb.mib.opacitymap.texLoc = re->GetUniformLocation("material.opacitymap");
	mpb.mib.opacitymap.numTex = re->GetUniformLocation("material.numOpacitymaps");

	mpb.mib.metallicmap.texLoc = re->GetUniformLocation("material.metallicmap");
	mpb.mib.metallicmap.numTex = re->GetUniformLocation("material.numMetallicmaps");

	mpb.mib.roughnessmap.texLoc = re->GetUniformLocation("material.roughnessmap");
	mpb.mib.roughnessmap.numTex = re->GetUniformLocation("material.numRoughnessmaps");

	re->BindShader(shadowPass);
	spb.instanceOffset = re->GetUniformLocation("instanceOffset");
	spb.opacitymap = re->GetUniformLocation("opacitymap");
	spb.numOpacitymaps = re->GetUniformLocation("numOpacitymaps");
}

void StaticMeshInstancedSystem::Update(World* world, entt::registry& registry)
{
	UpdateBuffer(world, registry);
}

void StaticMeshInstancedSystem::UpdateBuffer(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();

	auto cameraPos = camera->GetPosition();
	auto camDir = camera->GetDirection();

	auto& staticMeshSystem = World::Get()->staticMeshSystem;

	int instanceOffset{ 0 };
	auto view = registry.view<StaticMeshInstancedComponent>();
	for (auto entity : view)
	{
		auto& smic = view.get<StaticMeshInstancedComponent>(entity);

		auto& smi = smic.staticMeshInstanced;
		auto& smiTrans = smi.transforms;

		std::vector<glm::mat4> LODTransforms[4];

		for (int i = 0; i < smiTrans.size(); i++)
		{
			glm::vec3 objectPos = smiTrans[i][3];
			auto toObject = objectPos - cameraPos;
			auto length = glm::length(toObject);
			auto angle = glm::dot(camDir, toObject / length);

			//if (angle < 0.65f) continue; //culling eventually

			for (int j = 0; j < 4; j++)
			{
				if (SMI_LOD_RANGE[j] > length)
				{
					LODTransforms[j].emplace_back(smiTrans[i]);
					break;
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			smi.size[i] = LODTransforms[i].size();

			auto& meshes = smic.staticMeshInstanced.LOD[i].GetMeshes();

			for (int j = 0; j < meshes.size(); j++)
			{
				auto& mesh = meshes[j];

				
				if (mesh.bIsTransparent)
				{
					SMIAlphaMesh smiAlphaMesh;
					smiAlphaMesh.mesh = mesh;
					smiAlphaMesh.transforms = LODTransforms[i];
					smiAlphaMeshes.emplace_back(smiAlphaMesh);
				}
				
				/*
				if (mesh.bIsTransparent)
				{
					for (size_t k = 0; k < LODTransforms[i].size(); k++)
					{
						staticMeshSystem.AddAlphaMesh(mesh, LODTransforms[i][k], cameraPos);
					}
				}
				*/
			}

			renderEngine->BindSSBO(ssbo);
			renderEngine->BindSSBOData(instanceOffset * 64, LODTransforms[i].size() * 64, LODTransforms[i].data());
			instanceOffset += LODTransforms[i].size();
		}
	}
}

void StaticMeshInstancedSystem::ShadowPass(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(shadowPass);

	int instanceOffset{ 0 };
	auto view = registry.view<StaticMeshInstancedComponent>();
	for (auto entity : view)
	{
		auto& smic = view.get<StaticMeshInstancedComponent>(entity);
		auto& smi = smic.staticMeshInstanced;

		for (int i = 0; i < 4; i++)
		{
			auto size = smi.size[i];
		
			/*
			if (i == 3)
			{
				instanceOffset += size;
				continue;
			}*/

			auto& staticMeshInstanced = smi.LOD[i];
			renderEngine->SetInt(spb.instanceOffset, instanceOffset);

			for (auto& mesh : staticMeshInstanced.GetMeshes())
			{	
				if (mesh.material.HasOpacitymap())
				{
					renderEngine->SetTexture(mesh.material.opacitymap, GL_TEXTURE_2D, spb.opacitymap, 0);
					renderEngine->SetInt(spb.numOpacitymaps, 1);
				}
				else
				{
					renderEngine->SetTexture(mesh.material.diffusemap, GL_TEXTURE_2D, spb.opacitymap, 0);
					renderEngine->SetInt(spb.numOpacitymaps, 1);
				}

				renderEngine->DrawElementsInstanced(mesh, GL_TRIANGLES, size);
			}

			instanceOffset += size;
		}
	}
}

void StaticMeshInstancedSystem::MainPass(World* world, entt::registry& registry, const ABuffer* aBuffer, const Texture& shadowMaps, const Texture& cloudShadowVL)
{
	auto* renderEngine = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();
	auto cameraPos = camera->GetPosition();

	auto VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();

	renderEngine->BindShader(mainPass);
	renderEngine->BindSSBO(ssbo);

	if (aBuffer)
	{
		renderEngine->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		renderEngine->BindACBO(aBuffer->acbo, 0);
	}
	
	renderEngine->EnableCulling();
	renderEngine->DisableBlending();
	renderEngine->EnableDepthTest();
	renderEngine->SetDepthMask(GL_TRUE);

	renderEngine->SetTexture(cloudShadowVL, GL_TEXTURE_2D, mpb.cloudShadowVL, 16);
	renderEngine->SetTexture(shadowMaps, GL_TEXTURE_2D_ARRAY, mpb.sms, 17);

	renderEngine->SetInt(mpb.meshSettingsbHasTransparency, 0);

	int instanceOffset{ 0 };
	auto view = registry.view<StaticMeshInstancedComponent>();
	for (auto entity : view)
	{
		auto& smic = view.get<StaticMeshInstancedComponent>(entity);

		auto& smi = smic.staticMeshInstanced;

		for (int i = 0; i < 4; i++)
		{
			auto& staticMeshInstanced = smi.LOD[i];
			auto size = smi.size[i];

			renderEngine->SetInt(mpb.instanceOffset, instanceOffset);

			for (auto& mesh : staticMeshInstanced.GetMeshes())
			{
				if (mesh.bIsTransparent) continue;

				BindMaterial(mesh.material);				
				renderEngine->DrawElementsInstanced(mesh, GL_TRIANGLES, size);
			}

			instanceOffset += size;
		}
	}

	renderEngine->EnableCulling();
	renderEngine->SetDepthMask(GL_TRUE);
}

void StaticMeshInstancedSystem::AlphaPass(World* world, const Texture& shadowMaps, const ABuffer* aBuffer)
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(mainPass);
	renderEngine->BindSSBO(ssbo);

	if (aBuffer)
	{
		renderEngine->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		renderEngine->BindACBO(aBuffer->acbo, 0);
	}

	renderEngine->EnableCulling();
	renderEngine->DisableBlending();
	renderEngine->EnableDepthTest();
	renderEngine->SetDepthMask(GL_FALSE);

	renderEngine->SetInt(mpb.instanceOffset, 0);
	renderEngine->SetInt(mpb.meshSettingsbHasTransparency, 1);

	renderEngine->SetTexture(shadowMaps, GL_TEXTURE_2D_ARRAY, mpb.sms, 17);

	for (size_t i = 0; i < smiAlphaMeshes.size(); i++)
	{
		auto& smiAlphaMesh = smiAlphaMeshes[i];

		renderEngine->BindSSBO(ssbo);
		renderEngine->BindSSBOData(0, smiAlphaMesh.transforms.size() * 64, smiAlphaMesh.transforms.data());

		BindMaterial(smiAlphaMesh.mesh.material);
		renderEngine->DrawElementsInstanced(smiAlphaMesh.mesh, GL_TRIANGLES, smiAlphaMesh.transforms.size());
	}
	renderEngine->EnableCulling();
	renderEngine->SetDepthMask(GL_TRUE);
	smiAlphaMeshes.clear();
}

void StaticMeshInstancedSystem::BindMaterial(const MaterialTextured& material)
{
	auto* re = RenderEngine::Get();

	if (material.HasAOmap())
	{
		re->SetTexture(material.ambientmap, GL_TEXTURE_2D, mpb.mib.ambientmap.texLoc, 0);
		re->SetInt(mpb.mib.ambientmap.numTex, 1);
	}
	if (material.HasEmissivemap()) // emissive will also count as AO
	{
		re->SetTexture(material.emissivemap, GL_TEXTURE_2D, mpb.mib.ambientmap.texLoc, 0);
		re->SetInt(mpb.mib.ambientmap.numTex, 1);
	}
	if (material.HasDiffusemap())
	{
		re->SetTexture(material.diffusemap, GL_TEXTURE_2D, mpb.mib.diffusemap.texLoc, 1);
		re->SetInt(mpb.mib.diffusemap.numTex, 1);
	}
	if (material.HasSpecularmap())
	{
		re->SetTexture(material.specularmap, GL_TEXTURE_2D, mpb.mib.specularmap.texLoc, 2);
		re->SetInt(mpb.mib.specularmap.numTex, 1);
	}
	if (material.HasNormalmap())
	{
		re->SetTexture(material.normalmap, GL_TEXTURE_2D, mpb.mib.normalmap.texLoc, 3);
		re->SetInt(mpb.mib.normalmap.numTex, 1);
	}
	if (material.HasHeightmap())
	{
		re->SetTexture(material.heightmap, GL_TEXTURE_2D, mpb.mib.normalmap.texLoc, 3);
		re->SetInt(mpb.mib.normalmap.numTex, 1);
	}
	if (material.HasOpacitymap())
	{
		re->SetTexture(material.opacitymap, GL_TEXTURE_2D, mpb.mib.opacitymap.texLoc, 4);
		re->SetInt(mpb.mib.opacitymap.numTex, 1);
	}
	if (material.HasMetallicmap())
	{
		re->SetTexture(material.metallicmap, GL_TEXTURE_2D, mpb.mib.metallicmap.texLoc, 5);
		re->SetInt(mpb.mib.metallicmap.numTex, 1);
	}
	if (material.HasRoughnessmap())
	{
		re->SetTexture(material.roughnessmap, GL_TEXTURE_2D, mpb.mib.roughnessmap.texLoc, 6);
		re->SetInt(mpb.mib.roughnessmap.numTex, 1);
	}
	if (material.HasHeightmap())
	{
		re->SetTexture(material.heightmap, GL_TEXTURE_2D, mpb.mib.heightmap.texLoc, 7);
		re->SetInt(mpb.mib.heightmap.numTex, 1);
	}

	re->SetVec3(mpb.color, material.color);
	re->SetFloat(mpb.shininess, material.shininess);
}
