#pragma once

#include "Include/glm/glm.hpp"
#include "SSBO.h"
#include "PUBO.h"
#include "ACBO.h"
#include "Texture.h"

#define OIT_CLEAR_VALUE 0xffffffff
#define OIT_MAX_FRAGMENTS 50
#define OIT_MAX_PIXELS 1920*1080
#define OIT_MAX_NODES OIT_MAX_PIXELS * OIT_MAX_FRAGMENTS

/// Support for PBR
/// Each fragment has a linked list for translusent objects
/// Using SSBO and atomic write operations for GL4.3+

struct OITFragment
{
	float color;
	float depth;
	int next;
};

struct OITNode
{
	OITFragment fragment;
};

struct ABuffer
{
	Texture indexBuffer;
	ACBO acbo;
	PUBO pubo;
	SSBO ssbo;
};

