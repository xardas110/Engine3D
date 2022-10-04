#include "SkeletalMeshSystem.h"
#include "RenderEngine.h"
#include "World.h"
#include "Components.h"
#include "Debug.h"
#include "RenderEngine.h"

void SkeletalMeshSystem::Init(World* world, entt::registry& registry)
{
	InitShaders(world);
	InitShaderBindings();
}

void SkeletalMeshSystem::InitShaders(World* world)
{
	auto* renderEngine = RenderEngine::Get();
	renderEngine->GetShader(ShaderManager::ShaderType::SkeletalMesh, mainPassShader);
	renderEngine->GetShader(ShaderManager::ShaderType::DirectionalLightDepthPassSM, shadowPassShader);
}

void SkeletalMeshSystem::InitShaderBindings()
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(mainPassShader);
	// vertex bindings
	msb.model = renderEngine->GetUniformLocation("model");
	msb.MVP = renderEngine->GetUniformLocation("MVP");
	msb.scale = renderEngine->GetUniformLocation("scale");

	{
		for (size_t i = 0; i < 100; i++)
		{
			const std::string location = "gBones[" + std::to_string(i) + "]";
			msb.gBones[i] = renderEngine->GetUniformLocation(location);
		}
	}

	//frag bindings
	msb.smCascades = renderEngine->GetUniformLocation("shadowMaps");

	msb.renderSettingsbHasTransparency = renderEngine->GetUniformLocation("renderSettings.bHasTransparency");

	msb.viewPos = renderEngine->GetUniformLocation("viewPos");
	msb.camViewMat = renderEngine->GetUniformLocation("camView");

	msb.dirlight.ambient = renderEngine->GetUniformLocation("dirLight.ambient");
	msb.dirlight.lightDir = renderEngine->GetUniformLocation("dirLight.direction");
	msb.dirlight.diffuse = renderEngine->GetUniformLocation("dirLight.diffuse");
	msb.dirlight.specular = renderEngine->GetUniformLocation("dirLight.specular");

	msb.material.ambientmap.texLoc = renderEngine->GetUniformLocation("material.AOmap");
	msb.material.ambientmap.numTex = renderEngine->GetUniformLocation("material.numAOmaps");

	msb.material.diffusemap.texLoc = renderEngine->GetUniformLocation("material.diffusemap");
	msb.material.diffusemap.numTex = renderEngine->GetUniformLocation("material.numDiffusemaps");

	msb.material.specularmap.texLoc = renderEngine->GetUniformLocation("material.specularmap");
	msb.material.specularmap.numTex = renderEngine->GetUniformLocation("material.numSpecularmaps");

	msb.material.emissivemap.texLoc = renderEngine->GetUniformLocation("material.emissivemap");
	msb.material.emissivemap.numTex = renderEngine->GetUniformLocation("material.numEmissivemaps");

	msb.material.opacitymap.texLoc = renderEngine->GetUniformLocation("material.opacitymap");
	msb.material.opacitymap.numTex = renderEngine->GetUniformLocation("material.numOpacitymaps");

	msb.material.heightmap.texLoc = renderEngine->GetUniformLocation("material.heightmap");
	msb.material.heightmap.numTex = renderEngine->GetUniformLocation("material.numHeightmaps");

	msb.material.color = renderEngine->GetUniformLocation("material.color");
	msb.material.shininess = renderEngine->GetUniformLocation("material.shininess");

	// shadow pass bindings
	renderEngine->BindShader(shadowPassShader);
	spb.model = renderEngine->GetUniformLocation("model");

	{
		for (size_t i = 0; i < 100; i++)
		{
			const std::string location = "gBones[" + std::to_string(i) + "]";
			spb.gBones[i] = renderEngine->GetUniformLocation(location);
		}
	}
}

void SkeletalMeshSystem::Update(World* world, entt::registry& registry, float deltaTime)
{
	registry.view<SkeletalMeshComponent>().each([=](auto entity, auto& sm)
		{
			UpdateAnimiations(sm.skeletalMesh, deltaTime);
		});
}

void SkeletalMeshSystem::UpdateAnimiations(SkeletalMesh& skeletalMesh, float deltaTime)
{
	skeletalMesh.UpdateAnimation(deltaTime);
}

void SkeletalMeshSystem::ShadowPass(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(shadowPassShader);

	auto view = registry.view<TransformComponent, SkeletalMeshComponent>();
	for (auto entity : view)
	{
		auto& transform = view.get<TransformComponent>(entity);
		auto& skeletalMesh = view.get<SkeletalMeshComponent>(entity).skeletalMesh;

		renderEngine->SetMat4(spb.model, transform.GetTransform() * skeletalMesh.transform.GetTransform());
		BindSkeletalBonesShadowPass(world, renderEngine, skeletalMesh);

		for (auto& mesh : skeletalMesh.GetMeshes())
		{
			renderEngine->DrawElements(mesh, GL_TRIANGLES);
		}
	}
}

void SkeletalMeshSystem::MainPass(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();

	auto vp = world->GetRenderCamera()->GetProjectionMatrix() * world->GetRenderCamera()->GetViewMatrix();
	auto camPos = world->GetRenderCamera()->GetPosition();

	DirLight dirlight; // Support for only one
	world->GetDirectionalLight(dirlight);

	renderEngine->BindShader(mainPassShader);
	
	auto view = registry.view<TransformComponent, SkeletalMeshComponent>();
	for (auto entity : view)
	{
		auto [transform, skeletalMeshComponent] = view.get<TransformComponent, SkeletalMeshComponent>(entity);

		auto& skeletalMesh = skeletalMeshComponent.skeletalMesh;

		auto meshModel = transform.GetTransform() * skeletalMesh.transform.GetTransform();
		auto meshMVP = vp * meshModel;

		renderEngine->SetMat4(msb.model, meshModel);
		renderEngine->SetMat4(msb.MVP, meshMVP);
		renderEngine->SetVec3(msb.viewPos, camPos);

		renderEngine->SetVec3(msb.dirlight.ambient, dirlight.ambient);
		renderEngine->SetVec3(msb.dirlight.lightDir, dirlight.lightDir);
		renderEngine->SetVec3(msb.dirlight.diffuse, dirlight.diffuse);
		renderEngine->SetVec3(msb.dirlight.specular, dirlight.specular);

		renderEngine->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, msb.smCascades, 31);

		BindSkeletalBones(world, renderEngine, skeletalMesh);

		for (auto& mesh : skeletalMesh.GetMeshes())
		{
			if (mesh.bIsTransparent)
			{
				AddAlphaMesh(mesh, meshModel, camPos, entity);
				continue;
			};
			renderEngine->SetInt(msb.renderSettingsbHasTransparency, 0);
			DrawMesh(renderEngine, mesh);
		}
	}
	renderEngine->EnableCulling();
}

void SkeletalMeshSystem::AlphaPass(World* world, entt::registry& registry, const AlphaMesh& alphaMesh)
{
	auto* renderEngine = RenderEngine::Get();

	auto vp = world->GetRenderCamera()->GetProjectionMatrix() * world->GetRenderCamera()->GetViewMatrix();
	auto camPos = world->GetRenderCamera()->GetPosition();
	auto& sm = registry.get<SkeletalMeshComponent>(alphaMesh.entity).skeletalMesh;

	DirLight dirlight; // Support for only one
	world->GetDirectionalLight(dirlight);

	renderEngine->BindShader(mainPassShader);

	auto meshModel = alphaMesh.parentTransform;
	auto meshMVP = vp * meshModel;

	renderEngine->SetMat4(msb.model, meshModel);
	renderEngine->SetMat4(msb.MVP, meshMVP);
	renderEngine->SetVec3(msb.viewPos, camPos);

	renderEngine->SetVec3(msb.dirlight.ambient, dirlight.ambient);
	renderEngine->SetVec3(msb.dirlight.lightDir, dirlight.lightDir);
	renderEngine->SetVec3(msb.dirlight.diffuse, dirlight.diffuse);
	renderEngine->SetVec3(msb.dirlight.specular, dirlight.specular);

	renderEngine->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, msb.smCascades, 31);

	BindSkeletalBones(world, renderEngine, sm);

	DrawMesh(renderEngine, alphaMesh.mesh);
}

void SkeletalMeshSystem::BindSkeletalBones(World* world, RenderEngine* renderEngine, SkeletalMesh& skeletalMesh)
{
	std::vector<glm::mat4> transforms;
	skeletalMesh.GetBoneTransforms(transforms);

	for (int i = 0; i < transforms.size(); i++)
	{
		renderEngine->SetMat4(msb.gBones[i], transforms[i]);
	}
}

void SkeletalMeshSystem::BindSkeletalBonesShadowPass(World* world, RenderEngine* renderEngine, SkeletalMesh& skeletalMesh)
{
	std::vector<glm::mat4> transforms;
	skeletalMesh.GetBoneTransforms(transforms);

	for (int i = 0; i < transforms.size(); i++)
	{
		renderEngine->SetMat4(spb.gBones[i], transforms[i]);
	}
}

void SkeletalMeshSystem::AddAlphaMesh(const Mesh& mesh, const glm::mat4& meshModel, const glm::vec3& camPos, entt::entity entity)
{
	auto* apSystem = AlphaPassSystem::Get();

	AlphaPassInformation ap;

	glm::mat4 meshTransform = meshModel * mesh.transform.GetTransform();

	glm::vec3 meshPos = meshTransform[3];

	auto dist = glm::length2(meshPos - camPos);

	ap.alphaMesh.parentTransform = meshModel;
	ap.alphaMesh.mesh = mesh;
	ap.alphaMesh.entity = entity;

	ap.distToCamera2 = dist;

	ap.callBackFunc = [this](World* world, entt::registry& registry, const AlphaMesh& alphaMesh) {
		this->AlphaPass(world, registry, alphaMesh);
	};

	apSystem->AddAlphaMesh(ap);
}

void SkeletalMeshSystem::DrawMesh(RenderEngine* renderEngine, const Mesh& mesh)
{
	int index = -1;
	auto numAO = 0;
	auto numDiffuse = 0;
	auto numSpecular = 0;
	auto numNormal = 0;
	auto numHeight = 0;
	auto numOpacity = 0;
	auto numEmissive = 0;

	if (mesh.material.HasAOmap())
	{
		index++; numAO++;

		renderEngine->SetTexture(
			mesh.material.ambientmap,
			GL_TEXTURE_2D,
			msb.material.ambientmap.texLoc,
			index
		);
	}

	if (mesh.material.HasDiffusemap())
	{
		index++; numDiffuse++;

		renderEngine->SetTexture(
			mesh.material.diffusemap,
			GL_TEXTURE_2D,
			msb.material.diffusemap.texLoc,
			index
		);
	}

	if (mesh.material.HasSpecularmap())
	{
		index++; numSpecular++;

		renderEngine->SetTexture(
			mesh.material.specularmap,
			GL_TEXTURE_2D,
			msb.material.specularmap.texLoc,
			index
		);
	}

	if (mesh.material.HasNormalmap())
	{
		index++; numNormal++;

		renderEngine->SetTexture(
			mesh.material.normalmap,
			GL_TEXTURE_2D,
			msb.material.normalmap.texLoc,
			index
		);
	}

	if (mesh.material.HasEmissivemap())
	{
		index++; numEmissive++;

		renderEngine->SetTexture(
			mesh.material.emissivemap,
			GL_TEXTURE_2D,
			msb.material.emissivemap.texLoc,
			index
		);
	}

	if (mesh.material.HasOpacitymap())
	{
		index++; numOpacity++;

		renderEngine->SetTexture(
			mesh.material.opacitymap,
			GL_TEXTURE_2D,
			msb.material.opacitymap.texLoc,
			index
		);
	}

	if (mesh.material.HasHeightmap())
	{
		index++; numHeight++;

		renderEngine->SetTexture(
			mesh.material.heightmap,
			GL_TEXTURE_2D,
			msb.material.heightmap.texLoc,
			index
		);
	}

	renderEngine->SetInt(msb.material.ambientmap.numTex, numAO);
	renderEngine->SetInt(msb.material.diffusemap.numTex, numDiffuse);
	renderEngine->SetInt(msb.material.specularmap.numTex, numSpecular);
	renderEngine->SetInt(msb.material.normalmap.numTex, numNormal);
	renderEngine->SetInt(msb.material.emissivemap.numTex, numEmissive);
	renderEngine->SetInt(msb.material.opacitymap.numTex, numOpacity);
	renderEngine->SetInt(msb.material.heightmap.numTex, numHeight);

	renderEngine->SetVec3(msb.material.color, mesh.material.color);
	renderEngine->SetFloat(msb.material.shininess, mesh.material.shininess);

	renderEngine->DrawElements(mesh, GL_TRIANGLES);
}