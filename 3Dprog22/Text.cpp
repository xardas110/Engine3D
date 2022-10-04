#include "Text.h"

Text::Text(const std::string& text, const glm::vec3& color)
	:text(text), color(color)
{
}

Text::Text(const std::string& text, const glm::vec3& color, CoordinateSpace::CoordinateSpace coordSpace)
	:text(text), color(color), coordinateSpace(coordSpace)
{
}

Text::Text(const std::string& text, const glm::vec3& color, CoordinateSpace::CoordinateSpace coordSpace, FontType::FontType font)
	:text(text), color(color), coordinateSpace(coordSpace), fontType(font)
{
}

void Text::SetText(const std::string& text)
{
	bCalculateCharDuty = true;
	this->text = text;
}

const std::string& Text::GetText() const
{
	return text;
}

void Text::SetFontType(const FontType::FontType newType)
{
	fontType = newType;
}

FontType::FontType Text::GetFontType() const
{
	return fontType;
}

void Text::SetCoordinateSpace(const CoordinateSpace::CoordinateSpace newSpace)
{
	coordinateSpace = newSpace;
}

CoordinateSpace::CoordinateSpace Text::GetCoordinateSpace() const
{
	return coordinateSpace;
}

void Text::SetColor(const glm::vec3& newColor)
{
	color = newColor;
}

const glm::vec3& Text::GetColor() const
{
	return color;
}

void Text::SetTextOffset(const glm::vec2& offset)
{
	textOffset = offset;
}

const glm::vec2& Text::GetTextOffset() const
{
	return textOffset;
}

void Text::SetHidden(bool bHidden)
{
	bShow = !bHidden;
}

bool Text::IsHidden() const
{
	return !bShow;
}
