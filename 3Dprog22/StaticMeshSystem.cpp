#include "StaticMeshSystem.h"
#include "Json.h"
#include "World.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include "Debug.h"
#include "Components.h"

void StaticMeshSystem::Init(World* world, entt::registry& registry)
{
	InitShaders(world);
	InitShaderBindings();
}

void StaticMeshSystem::LoadPaths(World* world, const std::string& jsonPath)
{
	auto* smm = world->GetStaticMeshManager();

	Json json = Json::Load(jsonPath);

	auto& assets = json["assets"];

	for (int i = 0; i < assets.Size(); i++)
	{
		StaticMesh blob;
		std::string path = assets[i]["path"];

		if (!smm->LoadStaticMesh(path, blob))
		{
			std::cout << "\nFailed to load staic mesh: " << path << std::endl;

		}
		else
		{
			std::cout << "\nLoaded static mesh at path: " << path << std::endl;
		}
	}
}

void StaticMeshSystem::AddAlphaMesh(const Mesh& mesh, const glm::mat4& meshModel, const glm::vec3& camPos)
{
	auto* apSystem = AlphaPassSystem::Get();

	AlphaPassInformation ap;

	glm::mat4 meshTransform = meshModel * mesh.transform.GetTransform();

	glm::vec3 meshPos = meshTransform[3];

	auto dist = glm::length2(meshPos - camPos);

	ap.alphaMesh.parentTransform = meshModel;
	ap.alphaMesh.mesh = mesh;

	ap.distToCamera2 = dist;

	ap.callBackFunc = [this](World* world, entt::registry& registry, const AlphaMesh& alphaMesh) {
		this->AlphaPass(world, registry, alphaMesh);
	};

	apSystem->AddAlphaMesh(ap);
}

void StaticMeshSystem::BindDirlight(World* world)
{
	auto* renderEngine = RenderEngine::Get();

	DirLight dirlight; // Support for only one
	world->GetDirectionalLight(dirlight);
	renderEngine->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, msb.shadowMaps, 31);
}

void StaticMeshSystem::InitShaders(World* world)
{
	auto* renderEngine = RenderEngine::Get();
	auto* shaderManager = RenderEngine::Get()->GetShaderManager();
	shaderManager->GetShader(ShaderManager::StandardTextured, mainPassShader);
	shaderManager->GetShader(ShaderManager::ShaderType::DirectionalLightDepthPass, shadowPassShader);
}

void StaticMeshSystem::InitShaderBindings()
{
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(mainPassShader);
	// vertex bindings
	msb.model = renderEngine->GetUniformLocation("model");

	msb.shadowMaps = renderEngine->GetUniformLocation("shadowMaps");

	msb.settingsbTransparency = renderEngine->GetUniformLocation("meshSettings.bHasTransparency");

	msb.material.ambientmap.texLoc = renderEngine->GetUniformLocation("material.AOmap");
	msb.material.ambientmap.numTex = renderEngine->GetUniformLocation("material.numAOmaps");

	msb.material.diffusemap.texLoc = renderEngine->GetUniformLocation("material.diffusemap");
	msb.material.diffusemap.numTex = renderEngine->GetUniformLocation("material.numDiffusemaps");

	msb.material.normalmap.texLoc = renderEngine->GetUniformLocation("material.normalmap");
	msb.material.normalmap.numTex = renderEngine->GetUniformLocation("material.numNormalmaps");

	msb.material.specularmap.texLoc = renderEngine->GetUniformLocation("material.specularmap");
	msb.material.specularmap.numTex = renderEngine->GetUniformLocation("material.numSpecularmaps");

	msb.material.emissivemap.texLoc = renderEngine->GetUniformLocation("material.emissivemap");
	msb.material.emissivemap.numTex = renderEngine->GetUniformLocation("material.numEmissivemaps");

	msb.material.opacitymap.texLoc = renderEngine->GetUniformLocation("material.opacitymap");
	msb.material.opacitymap.numTex = renderEngine->GetUniformLocation("material.numOpacitymaps");

	msb.material.heightmap.texLoc = renderEngine->GetUniformLocation("material.heightmap");
	msb.material.heightmap.numTex = renderEngine->GetUniformLocation("material.numHeightmaps");

	msb.material.metallicmap.texLoc = renderEngine->GetUniformLocation("material.metallicmap");
	msb.material.metallicmap.numTex = renderEngine->GetUniformLocation("material.numMetallicmaps");

	msb.material.roughnessmap.texLoc = renderEngine->GetUniformLocation("material.roughnessmap");
	msb.material.roughnessmap.numTex = renderEngine->GetUniformLocation("material.numRoughnessmaps");

	msb.material.color = renderEngine->GetUniformLocation("material.color");
	msb.material.shininess = renderEngine->GetUniformLocation("material.shininess");

	// shadow pass bindings
	renderEngine->BindShader(shadowPassShader);
	spb.model = renderEngine->GetUniformLocation("model");
}

void StaticMeshSystem::ShadowPass(World* world, entt::registry& registry)
{
	auto* renderWindow = RenderWindow::Get();
	auto* renderEngine = RenderEngine::Get();

	renderEngine->BindShader(shadowPassShader);
	auto view = registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : view)
	{
		auto& transform = view.get<TransformComponent>(entity);
		auto& staticMeshInstanced = view.get<StaticMeshComponent>(entity).staticMeshInstanced;

		if (!staticMeshInstanced.bCastShadow) continue;

		renderEngine->SetMat4(spb.model, transform.GetTransform() * staticMeshInstanced.transform.GetTransform());

		for (auto& mesh : staticMeshInstanced.GetMeshes())
		{
			renderEngine->DrawElements(mesh, GL_TRIANGLES);
		}
	}	
}

void StaticMeshSystem::Voxelize(World* world, entt::registry& registry, const VoxelStructure* voxelStructure)
{
	if (!voxelStructure) return;

	auto* re = RenderEngine::Get();
	re->DisableCulling();
	auto view = registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : view)
	{
		auto& staticMeshInstanced = view.get<StaticMeshComponent>(entity).staticMeshInstanced;
		auto& transform = view.get<TransformComponent>(entity);

		auto meshModel = transform.GetTransform();

		re->SetMat4(voxelStructure->vb.model, meshModel);
		for (auto& mesh : staticMeshInstanced.GetMeshes())
		{
			if (mesh.bIsTransparent)
			{
				continue;
			};

			if (mesh.material.HasDiffusemap())
			{ 
				re->SetTexture(mesh.material.diffusemap, GL_TEXTURE_2D, voxelStructure->vb.albedo, 4);
				re->SetInt(voxelStructure->vb.numAlbedo, 4);
			}

			re->DrawElements(mesh, GL_TRIANGLES);
		}
	}
}

void StaticMeshSystem::MainPass(World* world, entt::registry& registry)
{
	auto* renderEngine = RenderEngine::Get();

	auto camPos = world->GetRenderCamera()->GetPosition();

	renderEngine->BindShader(mainPassShader);

	renderEngine->DisableCulling();
	renderEngine->EnableDepthTest();
	renderEngine->SetDepthMask(GL_TRUE);

	renderEngine->SetInt(msb.settingsbTransparency, 0);

	BindDirlight(world);

	auto view = registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : view)
	{
		auto& staticMeshInstanced = view.get<StaticMeshComponent>(entity).staticMeshInstanced;
		auto& transform = view.get<TransformComponent>(entity);

		auto meshModel = transform.GetTransform();

		renderEngine->SetMat4(msb.model, meshModel);
		for (auto& mesh : staticMeshInstanced.GetMeshes())
		{			
			if (mesh.bIsTransparent)
			{
				AddAlphaMesh(mesh, meshModel, camPos);
				continue;
			};
			DrawMesh(renderEngine, mesh);
		}
	}
	renderEngine->EnableCulling();
}

void StaticMeshSystem::AlphaPass(World* world, entt::registry& registry, const AlphaMesh& alphaMesh)
{
	auto* renderEngine = RenderEngine::Get();

	auto vp = world->GetRenderCamera()->GetProjectionMatrix() * world->GetRenderCamera()->GetViewMatrix();
	auto camPos = world->GetRenderCamera()->GetPosition();

	DirLight dirlight; // Support for only one
	world->GetDirectionalLight(dirlight);

	renderEngine->BindShader(mainPassShader);

	renderEngine->EnableDepthTest();
	renderEngine->SetDepthMask(GL_FALSE);

	auto meshModel = alphaMesh.parentTransform;
	auto meshMVP = vp * meshModel;

	renderEngine->SetInt(msb.settingsbTransparency, 1);

	renderEngine->SetMat4(msb.model, meshModel);

	DrawMesh(renderEngine, alphaMesh.mesh);

	renderEngine->SetDepthMask(GL_TRUE);
}

void StaticMeshSystem::DrawMesh(class RenderEngine* renderEngine, const Mesh& mesh)
{
	int index = -1;
	auto numAO = 0;
	auto numDiffuse = 0;
	auto numSpecular = 0;
	auto numNormal = 0;
	auto numHeight = 0;
	auto numOpacity = 0;
	auto numEmissive = 0;
	auto numMetallic{ 0 };
	auto numRoughness{ 0 };

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


	if (mesh.material.HasMetallicmap())
	{
		index++; numMetallic++;

		renderEngine->SetTexture(
			mesh.material.metallicmap,
			GL_TEXTURE_2D,
			msb.material.metallicmap.texLoc,
			index
		);
	}
	
	if (mesh.material.HasRoughnessmap())
	{
		index++; numRoughness++;

		renderEngine->SetTexture(
			mesh.material.roughnessmap,
			GL_TEXTURE_2D,
			msb.material.roughnessmap.texLoc,
			index
		);
	}

	if (mesh.material.HasShininessmap())
	{
		index++; numRoughness++;

		renderEngine->SetTexture(
			mesh.material.shininessmap,
			GL_TEXTURE_2D,
			msb.material.roughnessmap.texLoc,
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
	renderEngine->SetInt(msb.material.metallicmap.numTex, numMetallic);
	renderEngine->SetInt(msb.material.roughnessmap.numTex, numRoughness);

	renderEngine->SetInt(msb.settingsbTransparency, mesh.bIsTransparent);

	renderEngine->SetVec3(msb.material.color, mesh.material.color);
	renderEngine->SetFloat(msb.material.shininess, mesh.material.shininess);

	renderEngine->DrawElements(mesh, GL_TRIANGLES);
}
