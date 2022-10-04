#include "VegetationSystem.h"
#include "World.h"
#include "RenderEngine.h"
#include "Components.h"
#include "StaticMeshInstanced.h"

void VegetationSystem::Init(World* world, entt::registry& registry)
{
	InitShaders();
	InitSSBO();
	InitShaderBindings();
}

void VegetationSystem::InitShaders()
{
	auto* renderEngine = RenderEngine::Get();

	if (!renderEngine->GetShader(ShaderManager::ShaderType::Vegetation, mainPass))
	{
		printf("Failed to load instanced shader for main pass\n");
	}

	if (!renderEngine->GetShader(ShaderManager::ShaderType::VegetationSM, shadowPass))
	{
		printf("Failed to load instanced SM shader for shadow pass\n");
	}

	if (!renderEngine->GetShader(ShaderManager::ShaderType::VegetationSG, sgShader))
	{
		printf("Failed to load SG Shader\n");
	}
}

void VegetationSystem::InitSSBO()
{
	auto* renderEngine = RenderEngine::Get();
	auto* ssboManager = renderEngine->GetSSBOManager();

	ssbo = ssboManager->CreateSSBO("InstancedMatrices", MAX_INSTANCES * 64, 1);

	renderEngine->BindSSBO(ssbo);
	ssboManager->BindShaderToBlock(mainPass.shaderId, ssbo.id, 1, "instancedMatrices");
	ssboManager->BindShaderToBlock(sgShader.shaderId, ssbo.id, 1, "instancedMatrices");
}

void VegetationSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();

	re->BindShader(mainPass);

	mpb.instanceOffset = re->GetUniformLocation("instanceOffset");
	mpb.color = re->GetUniformLocation("material.color");
	mpb.shininess = re->GetUniformLocation("material.shininess");

	mpb.sms = re->GetUniformLocation("shadowMaps");
	mpb.cloudShadowVL = re->GetUniformLocation("cloudShadowVL");
	mpb.depthmap =  re->GetUniformLocation("depthmap");

	mpb.mib.ambientmap.texLoc = re->GetUniformLocation("material.AOmap");
	mpb.mib.ambientmap.numTex = re->GetUniformLocation("material.numAOmaps");

	mpb.mib.diffusemap.texLoc = re->GetUniformLocation("material.diffusemap");
	mpb.mib.diffusemap.numTex = re->GetUniformLocation("material.numDiffusemaps");

	mpb.mib.specularmap.texLoc = re->GetUniformLocation("material.specularmap");
	mpb.mib.specularmap.numTex = re->GetUniformLocation("material.numSpecularmaps");

	mpb.mib.normalmap.texLoc = re->GetUniformLocation("material.normalmap");
	mpb.mib.normalmap.numTex = re->GetUniformLocation("material.numNormalmaps");

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

	re->BindShader(sgShader);
	sgb.instanceOffset = re->GetUniformLocation("instanceOffset");
	sgb.opacitymap = re->GetUniformLocation("opacitymap");
	sgb.numOpacitymaps = re->GetUniformLocation("numOpacitymaps");
}

void VegetationSystem::Update(World* world, entt::registry& registry)
{
	UpdateBuffer(world, registry);
}

void VegetationSystem::UpdateBuffer(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();

	auto cameraPos = camera->GetPosition();
	auto camDir = camera->GetDirection();

	std::atomic<int> instanceOffset{ 0 };
	auto view = registry.view<VegetationComponent>();
	for (auto entity : view)
	{
		auto& vegc = view.get<VegetationComponent>(entity);
		auto& veg = vegc.veg;

		for (int k=0; k < veg.variants.size(); k++)
		{
			auto& variant = veg.variants[k];
			auto& variantTrans = variant.transforms;

			std::vector<glm::mat4> LODTransforms[VEG_MAX_LODS];

			for (int i = 0; i < variantTrans.size(); i++)
			{
				glm::vec3 objectPos = variantTrans[i][3];
				auto toObject = objectPos - cameraPos;
				auto length = glm::length(toObject);

				if (length > VEG_LOD_RANGES[VEG_MAX_LODS - 1]) continue;

				auto angle = glm::dot(camDir, toObject / length);

				if (angle < 0.65f) continue; //culling eventually

				for (int j = 0; j < VEG_MAX_LODS; j++)
				{
					if (VEG_LOD_RANGES[j] > length)
					{
						LODTransforms[j].emplace_back(variantTrans[i]);
						break;
					}
				}
			}

			for (int i = 0; i < VEG_MAX_LODS; i++)
			{
				variant.size[i] = LODTransforms[i].size();

				renderEngine->BindSSBO(ssbo);
				renderEngine->BindSSBOData(instanceOffset * 64, LODTransforms[i].size() * 64, LODTransforms[i].data());
				instanceOffset += LODTransforms[i].size();
			}
		}
	}
}

void VegetationSystem::MainPass(World* world, entt::registry& registry, const ABuffer* aBuffer, const Texture& shadowMaps, const Texture& cloudShadowVL, const Texture& depthmap)
{
	auto* re = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();
	auto cameraPos = camera->GetPosition();

	auto VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();

	re->BindShader(mainPass);
	re->BindSSBO(ssbo);

	re->EnableCulling();
	re->EnableDepthTest();
	re->SetDepthMask(GL_FALSE);
	re->DisableBlending();

	if (aBuffer)
	{ 
		re->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		re->BindACBO(aBuffer->acbo, 0);
	}

	re->SetTexture(shadowMaps, GL_TEXTURE_2D_ARRAY, mpb.sms, 7);
	re->SetTexture(cloudShadowVL, GL_TEXTURE_2D, mpb.cloudShadowVL, 8);
	re->SetTexture(depthmap, GL_TEXTURE_2D, mpb.depthmap, 9);

	int instanceOffset{ 0 };
	auto view = registry.view<VegetationComponent>();
	for (auto entity : view)
	{
		auto& vegc = view.get<VegetationComponent>(entity);
		auto& veg = vegc.veg;

		for (int k = 0; k< veg.variants.size(); k++)
		{
			BindMaterials(veg.atlas);
			auto& variant = veg.variants[k];
			for (int i = 0; i < VEG_MAX_LODS; i++)
			{				
				if (i == VEG_MAX_LODS -1)
				{
					BindMaterials(veg.billboard);
				}
				
				auto& staticMeshInstanced = variant.LOD[i];
				auto size = variant.size[i];

				re->SetInt(mpb.instanceOffset, instanceOffset);

				for (auto& mesh : staticMeshInstanced.GetMeshes())
				{
					re->DrawElementsInstanced(mesh, GL_TRIANGLES, size);
				}

				instanceOffset += size;
			}
		}
	}
	re->DisableBlending();
	re->SetDepthMask(GL_TRUE);
	//re->SetMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void VegetationSystem::ShadowPass(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(shadowPass);

	int instanceOffset{ 0 };
	auto view = registry.view<VegetationComponent>();
	for (auto entity : view)
	{
		auto& vegc = view.get<VegetationComponent>(entity);
		auto& veg = vegc.veg;

		if (veg.atlas.HasOpacitymap())
		{
			renderEngine->SetTexture(veg.atlas.opacitymap, GL_TEXTURE_2D, spb.opacitymap, 0);
			renderEngine->SetInt(spb.numOpacitymaps, 1);
		}

		for (int k = 0; k < veg.variants.size(); k++)
		{			
			auto& variant = veg.variants[k];
			for (int i = 0; i < VEG_MAX_LODS; i++)
			{
				auto size = variant.size[i];

				if (i >= VEG_MAX_LOD_SHADOW)
				{
					instanceOffset += size;
					continue;
				}

				auto& staticMeshInstanced = variant.LOD[i];
				renderEngine->SetInt(spb.instanceOffset, instanceOffset);

				for (auto& mesh : staticMeshInstanced.GetMeshes())
				{
					renderEngine->DrawElementsInstanced(mesh, GL_TRIANGLES, size);
				}

				instanceOffset += size;
			}
		}
	}
}

void VegetationSystem::BindMaterials(MaterialInstanced& materialInstanced)
{
	auto* renderEngine = RenderEngine::Get();
	if (materialInstanced.HasAOmap())
	{
		renderEngine->SetTexture(materialInstanced.ambientmap, GL_TEXTURE_2D, mpb.mib.ambientmap.texLoc, 0);
		renderEngine->SetInt(mpb.mib.ambientmap.numTex, 1);
	}
	if (materialInstanced.HasDiffusemap())
	{
		renderEngine->SetTexture(materialInstanced.diffusemap, GL_TEXTURE_2D, mpb.mib.diffusemap.texLoc, 1);
		renderEngine->SetInt(mpb.mib.diffusemap.numTex, 1);
	}
	if (materialInstanced.HasSpecularmap())
	{
		renderEngine->SetTexture(materialInstanced.specularmap, GL_TEXTURE_2D, mpb.mib.specularmap.texLoc, 2);
		renderEngine->SetInt(mpb.mib.specularmap.numTex, 1);
	}
	if (materialInstanced.HasNormalmap())
	{
		renderEngine->SetTexture(materialInstanced.normalmap, GL_TEXTURE_2D, mpb.mib.normalmap.texLoc, 3);
		renderEngine->SetInt(mpb.mib.normalmap.numTex, 1);
	}
	if (materialInstanced.HasOpacitymap())
	{
		renderEngine->SetTexture(materialInstanced.opacitymap, GL_TEXTURE_2D, mpb.mib.opacitymap.texLoc, 4);
		renderEngine->SetInt(mpb.mib.opacitymap.numTex, 1);
	}

	if (materialInstanced.HasMetallicmap())
	{
		renderEngine->SetTexture(materialInstanced.metallicmap, GL_TEXTURE_2D, mpb.mib.metallicmap.texLoc, 5);
		renderEngine->SetInt(mpb.mib.metallicmap.numTex, 1);
	}

	if (materialInstanced.HasRoughnessmap())
	{
		renderEngine->SetTexture(materialInstanced.roughnessmap, GL_TEXTURE_2D, mpb.mib.roughnessmap.texLoc, 6);
		renderEngine->SetInt(mpb.mib.roughnessmap.numTex, 1);
	}

	renderEngine->SetVec3(mpb.color, materialInstanced.color);
	renderEngine->SetFloat(mpb.shininess, materialInstanced.shininess);
}

