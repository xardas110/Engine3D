#include "BillboardSystem.h"
#include "RenderEngine.h"
#include "World.h"
#include "Components.h"

void BillboardSystem::Init(World* world, entt::registry& registry)
{
	InitShader();
	InitShaderBindings();
	InitSSBO();
	InitFonts();
}

void BillboardSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::ShaderType::Text, textShader);
}

void BillboardSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();

	re->BindShader(textShader);
	tb.model = re->GetUniformLocation("model");
	tb.color = re->GetUniformLocation("textColor");
	tb.atlas = re->GetUniformLocation("atlas");
	tb.vp = re->GetUniformLocation("VP");
}

void BillboardSystem::InitSSBO()
{
	auto* re = RenderEngine::Get();
	auto* sm = re->GetSSBOManager();

	ssbo = sm->CreateSSBO("Text",
		sizeof(Char) * MAX_CHARACTERS,
		7);

	re->BindSSBO(ssbo);
	sm->BindShaderToBlock(textShader.shaderId, ssbo.id, 7, "instancedCharactersSSBO");
}

void BillboardSystem::InitFonts()
{
	fonts[FontType::Arial].Load("../3dprog22/Assets/Fonts/arial.fnt");
	fonts[FontType::Segoe].Load("../3dprog22/Assets/Fonts/segoe.fnt");
	fonts[FontType::Tahoma].Load("../3dprog22/Assets/Fonts/tahoma.fnt");
}

void BillboardSystem::Update(World* world, entt::registry& registry, float deltatime)
{
	auto view = registry.view<TextComponent>();

	for (auto entity : view)
	{
		auto& text = view.get<TextComponent>(entity).text;
		
		if (text.bCalculateCharDuty)
		{
			text.characters.resize(text.text.size());

			glm::vec2 tempOffset = text.GetTextOffset();

			for (auto i = 0; i < text.GetText().size(); i++)
			{ 
				auto& ch = text.GetText()[i];
				fonts[text.GetFontType()].GetCharPosition(ch, 
					text.characters[i].offset, 
					text.characters[i].texcoord, 
					tempOffset);
			}
		}
	}
}

void BillboardSystem::Render(class World* world, entt::registry& registry, const ABuffer* aBuffer, const Mesh& quad)
{
	auto* re = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();

	re->BindShader(textShader);

	re->DisableCulling();
	re->EnableBlending();
	re->EnableDepthTest();
	re->SetDepthMask(GL_FALSE);

	if (aBuffer)
	{
		re->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		re->BindACBO(aBuffer->acbo, 0);
	}

	auto view = registry.view<TransformComponent, TextComponent>();

	for (auto entity : view)
	{
		auto [trans, textComp] = view.get<TransformComponent, TextComponent>(entity);
		auto& text = textComp.text;

		if (text.IsHidden()) continue;

		re->SetVec3(tb.color, text.color);
		re->SetTexture(fonts[text.GetFontType()].atlas, GL_TEXTURE_2D, tb.atlas, 0);

		re->BindSSBO(ssbo);
		re->BindSSBOData(0, sizeof(Char) * text.characters.size(), text.characters.data());

		switch (text.GetCoordinateSpace())
		{
			case CoordinateSpace::WorldSpace:
			{
				re->SetMat4(tb.model, trans.GetTransform());
				re->SetMat4(tb.vp,
					camera->GetProjectionMatrix() * camera->GetViewMatrix());
			}
			break;
			case CoordinateSpace::LocalSpace:
			{
				glm::mat4 identity{ 1.f };
				re->SetMat4(tb.model, identity);
				re->SetMat4(tb.vp, identity);
			}
			break;
			case CoordinateSpace::BillboardSpace:
			{
				auto model = trans.GetTransform();
				auto& view = camera->GetViewMatrix();
				

				model[0][0] = view[0][0];
				model[0][1] = view[1][0];
				model[0][2] = view[2][0];

				model[1][0] = view[0][1];
				model[1][1] = view[1][1];
				model[1][2] = view[2][1];

				model[2][0] = view[0][2];
				model[2][1] = view[1][2];
				model[2][2] = view[2][2];
				
				glm::mat4 scale(1.f);

				scale = glm::scale(scale,
					{ trans.GetTransform()[0][0],
					trans.GetTransform()[1][1],
					trans.GetTransform()[2][2] });
					
				model = model * scale;

				re->SetMat4(tb.model, model);

				re->SetMat4(tb.vp, camera->GetProjectionMatrix() * view);				
			}
			break;
			default: break;
		}

		re->DrawElementsInstanced(quad, GL_TRIANGLES, text.characters.size());
	}

	re->EnableCulling();
	re->EnableDepthTest();
	re->SetDepthMask(GL_TRUE);
	re->EnableBlending();
}
