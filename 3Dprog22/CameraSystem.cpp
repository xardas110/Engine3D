#include "CameraSystem.h"
#include "World.h"
#include "RenderEngine.h"

void CameraSystem::Init(World* world, entt::registry& registry)
{
	InitBuffer(world);
	InitCameraShader();
	InitCameraMesh(world);	
}

void CameraSystem::InitBuffer(World* world)
{
	auto* re = RenderEngine::Get();
	auto* uboManager = re->GetUBOManager();
	ubo = uboManager->CreateUBO("CameraUBO", 208, 2);
}

void CameraSystem::InitCameraMesh(World* world)
{
	auto* re = RenderEngine::Get();
	auto* sm = world->GetStaticMeshManager();

	sm->LoadStaticMesh("../3Dprog22/Assets/Models/Camera/Camera.obj", cameraMesh);
}

void CameraSystem::InitCameraShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::StandardTextured, cameraShader);

	re->BindShader(cameraShader);
	// vertex bindings
	model = re->GetUniformLocation("model");

	smCascades = re->GetUniformLocation("smCascades");

	mtb.ambientmap.texLoc = re->GetUniformLocation("material.AOmap");
	mtb.ambientmap.numTex = re->GetUniformLocation("material.numAOmaps");

	mtb.diffusemap.texLoc = re->GetUniformLocation("material.diffusemap");
	mtb.diffusemap.numTex = re->GetUniformLocation("material.numDiffusemaps");

	mtb.normalmap.texLoc = re->GetUniformLocation("material.normalmap");
	mtb.normalmap.numTex = re->GetUniformLocation("material.numNormalmaps");

	mtb.specularmap.texLoc = re->GetUniformLocation("material.specularmap");
	mtb.specularmap.numTex = re->GetUniformLocation("material.numSpecularmaps");

	mtb.emissivemap.texLoc = re->GetUniformLocation("material.emissivemap");
	mtb.emissivemap.numTex = re->GetUniformLocation("material.numEmissivemaps");

	mtb.opacitymap.texLoc = re->GetUniformLocation("material.opacitymap");
	mtb.opacitymap.numTex = re->GetUniformLocation("material.numOpacitymaps");

	mtb.heightmap.texLoc = re->GetUniformLocation("material.heightmap");
	mtb.heightmap.numTex = re->GetUniformLocation("material.numHeightmaps");

	mtb.metallicmap.texLoc = re->GetUniformLocation("material.metallicmap");
	mtb.metallicmap.numTex = re->GetUniformLocation("material.numMetallicmaps");

	mtb.roughnessmap.texLoc = re->GetUniformLocation("material.roughnessmap");
	mtb.roughnessmap.numTex = re->GetUniformLocation("material.numRoughnessmaps");

	mtb.color = re->GetUniformLocation("material.color");
	mtb.shininess = re->GetUniformLocation("material.shininess");
}

void CameraSystem::Update(World* world, entt::registry& registry, float deltaTime)
{
	auto* renderEngine = RenderEngine::Get();

	auto* renderCamera = world->GetRenderCamera();
	renderCamera->UpdateCamera(deltaTime);

	auto camView = renderCamera->GetViewMatrix();
	auto camProj = renderCamera->GetProjectionMatrix();
	auto vp = camProj * camView;

	auto cameraPos = glm::vec4(renderCamera->GetPosition(), 0.f);

	std::vector<glm::mat4> matrices;
	matrices.emplace_back(camView);
	matrices.emplace_back(camProj);
	matrices.emplace_back(vp);

	const auto matricesTotalSize = matrices.size() * sizeof(matrices[0]);

	float zFar = renderCamera->GetFar();

	renderEngine->BindUBO(ubo);
	renderEngine->BindUBOData(0, matricesTotalSize, matrices.data());
	renderEngine->BindUBOData(matricesTotalSize, 16, &cameraPos);
	renderEngine->BindUBOData(204, 4, &zFar);
}

void CameraSystem::Render(World* world, entt::registry& registry)
{
	//Oppgave 5b
	//Alle kamera meshene som skal vises i editor er interne systemer, derfor har de en egen seksjon i camerasystemet.

	if (world->IsState(WorldState::Play)) return;

	auto* re = RenderEngine::Get();

	auto view = registry.view<CameraComponent>();

	for (auto entity : view)
	{
		auto& cam = view.get<CameraComponent>(entity).camera;
		auto camPos = cam.GetPosition();
		re->BindShader(cameraShader);
		re->DisableCulling();

		Transform trans;
		trans.SetPosition(camPos);
		trans.SetScale({ 0.5f, 0.5f, 0.5f });
		trans.SetRotation(glm::inverse(cam.GetViewMatrix())); //inverse av view er samme som en lookat

		re->SetMat4(model, trans.GetTransform());

		//Det er meninga at kamera ikke skal ha en tekstur, skal skille seg ut litt :)
		for (auto& mesh : cameraMesh.GetMeshes())
		{
			DrawMesh(re, mesh);
		}
		
		re->EnableCulling();
	}
}

void CameraSystem::BindDirlight(World* world)
{
	auto* re = RenderEngine::Get();

	DirLight dirlight; // Support for only one
	world->GetDirectionalLight(dirlight);
	re->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, smCascades, 31);
}

void CameraSystem::DrawMesh(RenderEngine* renderEngine, const Mesh& mesh)
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
			mtb.ambientmap.texLoc,
			index
		);
	}

	if (mesh.material.HasDiffusemap())
	{
		index++; numDiffuse++;

		renderEngine->SetTexture(
			mesh.material.diffusemap,
			GL_TEXTURE_2D,
			mtb.diffusemap.texLoc,
			index
		);
	}

	if (mesh.material.HasSpecularmap())
	{
		index++; numSpecular++;

		renderEngine->SetTexture(
			mesh.material.specularmap,
			GL_TEXTURE_2D,
			mtb.specularmap.texLoc,
			index
		);
	}

	if (mesh.material.HasNormalmap())
	{
		index++; numNormal++;

		renderEngine->SetTexture(
			mesh.material.normalmap,
			GL_TEXTURE_2D,
			mtb.normalmap.texLoc,
			index
		);
	}

	if (mesh.material.HasEmissivemap())
	{
		index++; numEmissive++;

		renderEngine->SetTexture(
			mesh.material.emissivemap,
			GL_TEXTURE_2D,
			mtb.emissivemap.texLoc,
			index
		);
	}

	if (mesh.material.HasOpacitymap())
	{
		index++; numOpacity++;

		renderEngine->SetTexture(
			mesh.material.opacitymap,
			GL_TEXTURE_2D,
			mtb.opacitymap.texLoc,
			index
		);
	}

	if (mesh.material.HasHeightmap())
	{
		index++; numHeight++;
		numNormal++;

		renderEngine->SetTexture(
			mesh.material.heightmap,
			GL_TEXTURE_2D,
			mtb.heightmap.texLoc,
			index
		);

		index++;

		renderEngine->SetTexture(
			mesh.material.heightmap,
			GL_TEXTURE_2D,
			mtb.normalmap.texLoc,
			index
		);

	}
	if (mesh.material.HasMetallicmap())
	{
		index++; numMetallic++;

		renderEngine->SetTexture(
			mesh.material.metallicmap,
			GL_TEXTURE_2D,
			mtb.metallicmap.texLoc,
			index
		);
	}

	if (mesh.material.HasRoughnessmap())
	{
		index++; numRoughness++;

		renderEngine->SetTexture(
			mesh.material.roughnessmap,
			GL_TEXTURE_2D,
			mtb.roughnessmap.texLoc,
			index
		);
	}

	if (mesh.material.HasShininessmap())
	{
		index++; numRoughness++;

		renderEngine->SetTexture(
			mesh.material.shininessmap,
			GL_TEXTURE_2D,
			mtb.roughnessmap.texLoc,
			index
		);
	}

	renderEngine->SetInt(mtb.ambientmap.numTex, numAO);
	renderEngine->SetInt(mtb.diffusemap.numTex, numDiffuse);
	renderEngine->SetInt(mtb.specularmap.numTex, numSpecular);
	renderEngine->SetInt(mtb.normalmap.numTex, numNormal);
	renderEngine->SetInt(mtb.emissivemap.numTex, numEmissive);
	renderEngine->SetInt(mtb.opacitymap.numTex, numOpacity);
	renderEngine->SetInt(mtb.heightmap.numTex, numHeight);
	renderEngine->SetInt(mtb.metallicmap.numTex, numMetallic);
	renderEngine->SetInt(mtb.roughnessmap.numTex, numRoughness);

	renderEngine->SetVec3(mtb.color, mesh.material.color);
	renderEngine->SetFloat(mtb.shininess, mesh.material.shininess);

	renderEngine->DrawElements(mesh, GL_TRIANGLES);
}
