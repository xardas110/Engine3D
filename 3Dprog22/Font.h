#pragma once
#include "Texture.h"
#include "Include/glm/glm.hpp"
#include "unordered_map"

namespace FontType
{
	enum FontType
	{
		Arial,
		Segoe,
		Tahoma,
		Size
	};
}

struct FontData
{
	int size;
	int bold;
	int lineHeight;
	int scaleW;
	int scaleH;
	int pages;
	int numChars;
	std::string file;
};

struct FontCharData
{
	int x, y;
	int width, height;
	int xOffset, yOffset;
	int xadvance;
	int page;
	int chnl;	
};

class Font
{
	friend class BillboardSystem;

	using FontMap = std::unordered_map<char, FontCharData>;

	bool Load(const std::string& path);

private:

	bool GetCharPosition(char charId, glm::vec4& outPos, glm::vec4& outTex, glm::vec2& tempOffset);

	bool LoadTexture(const std::string& path);

	FontData fontData;
	Texture atlas;
	FontMap fontMap;
};