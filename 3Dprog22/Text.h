#pragma once
#include "Font.h"
#include "Include/glm/glm.hpp"
#include <vector>

namespace CoordinateSpace
{
	enum CoordinateSpace
	{
		LocalSpace,
		WorldSpace,
		BillboardSpace
	};
}

struct Char
{
	glm::vec4 offset; // offset.xy = pos2d, offset.zw = scale2d  
	glm::vec4 texcoord; //pos.xy scale.zw
};

struct Text
{
	friend class BillboardSystem;

	using Characters = std::vector<Char>;

	Text() = default;
	Text(const std::string& text, const glm::vec3& color);

	Text(const std::string& text,
		const glm::vec3& color,
		CoordinateSpace::CoordinateSpace coordSpace);

	Text(const std::string& text, 
		const glm::vec3& color,
		CoordinateSpace::CoordinateSpace coordSpace,
		FontType::FontType);

	void SetText(const std::string& text);
	const std::string& GetText() const;

	void SetFontType(const FontType::FontType newType);
	FontType::FontType GetFontType() const;

	void SetCoordinateSpace(const CoordinateSpace::CoordinateSpace newSpace);
	CoordinateSpace::CoordinateSpace GetCoordinateSpace() const;

	void SetColor(const glm::vec3& newColor);
	const glm::vec3& GetColor() const;

	void SetTextOffset(const glm::vec2& offset);
	const glm::vec2& GetTextOffset() const;

	void SetHidden(bool bHidden);
	bool IsHidden() const;
private:

	bool bShow{ true };
	bool bCalculateCharDuty{ true };

	std::string text;
	Characters characters; // cache & gpu data

	glm::vec2 textOffset{ 0.1f, 0.8f };
	glm::vec3 color;

	FontType::FontType fontType{ FontType::Arial };
	CoordinateSpace::CoordinateSpace coordinateSpace{ CoordinateSpace::WorldSpace };
};

struct TextBindings
{
	int model;
	int color;
	int atlas;
	int vp;
};