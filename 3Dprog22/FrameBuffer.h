#pragma once
#include "Texture.h"

struct FrameBuffer
{
	unsigned fboId;
	//Layered rendering if multiple textures are needed
	//Or depth texture for a renderbuffer
	Texture texture; 
	
	[[nodiscard]]
	bool IsFBOValid() const
	{
		return fboId != 0;
	}
	
	[[nodiscard]]
	bool IsTextureValid() const
	{
		return texture.IsValid();
	}
};
