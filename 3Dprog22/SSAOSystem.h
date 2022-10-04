#pragma once
#include <vector>
#include "SSAO.h"
#include "Include/glm/glm.hpp"
#include <random>
#include "GBuffer.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"

class SSAOSystem
{
	friend class DeferredRenderer;

	void Init();
	void InitShader();
	void InitShaderBinings();
	void InitKernel();
	void InitKernelBindings();
	void InitNoise();
	void InitBlur();
	void InitFBO();
	void InitUpscaled();

	void UpdateNoise();

	void Process(const GBuffer& gBuffer, const Mesh& quad, const DeferredRendererConfig& config);

	void OnResize(int width, int height);

	void Clean();
private:
	int downSample = 1; //2-4-8-16
	SSAO ssao;
	SSAOBindings ssaob;
};

