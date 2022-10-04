#include "Font.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include <fstream>

/// <summary>
/// Parsing a font file.
/// </summary>

bool GetValueString(const std::string& delimiter, const std::string& text, std::string& outValString, char endDelimiter = ' ')
{
	auto startPos = text.find(delimiter);
	auto equalPos = text.find_first_of('=', startPos);
	auto endPos = text.find_first_of(endDelimiter, equalPos);
	
	if (startPos == std::string::npos)
	{
		printf("Delimiter not found: %s \n", delimiter.c_str());
		return false;
	}

	if (equalPos == std::string::npos)
	{
		printf("Delimiter is not searching for a right format: %s \n", delimiter.c_str());
		return false;
	}

	if (endPos == std::string::npos)
	{
		printf("Delimiter is not searching for a right format, end space not found: %s \n", delimiter.c_str());
		return false;
	}

	outValString = std::string(text.begin() + equalPos + 1 , text.begin() + endPos);

	return true;
}

int ParseAsInt(const std::string& valString)
{
	return std::stoi(valString);
}

bool ParseCommonFontData(const std::string& text, FontData& outFontData)
{
	std::string valString;	
	if (!GetValueString("scaleW", text, valString))
	{
		printf("scaleW not found \n");
		return false;
	}
	outFontData.scaleW = ParseAsInt(valString);

	if (!GetValueString("scaleH", text, valString))
	{
		printf("scaleH not found \n");
		return false;
	}
	outFontData.scaleH = ParseAsInt(valString);

	return true;
}

bool ParseCharsData(const std::string& text, FontData& outFontData)
{
	std::string valString;
	if (!GetValueString("count", text, valString, '\n'))
	{
		printf("Failed to get chars count text string\n");
		return false;
	}
	outFontData.numChars = ParseAsInt(valString);

	return true;
}

bool ParseChar(const std::string& line, char& outCharId, FontCharData& outFontCharData)
{
	std::string valString;
	if (!GetValueString("id", line, valString))
	{
		printf("Failed to parse char id from line %s\n", line.c_str());
		return false;
	}
	outCharId = ParseAsInt(valString);
	
	if (!GetValueString("x", line, valString))
	{
		printf("Failed to parse char x from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.x = ParseAsInt(valString);

	if (!GetValueString("y", line, valString))
	{
		printf("Failed to parse char y from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.y = ParseAsInt(valString);

	if (!GetValueString("width", line, valString))
	{
		printf("Failed to parse char width from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.width = ParseAsInt(valString);

	if (!GetValueString("height", line, valString))
	{
		printf("Failed to parse char height from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.height = ParseAsInt(valString);

	if (!GetValueString("xoffset", line, valString))
	{
		printf("Failed to parse char xoffset from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.xOffset = ParseAsInt(valString);

	if (!GetValueString("yoffset", line, valString))
	{
		printf("Failed to parse char yoffset from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.yOffset = ParseAsInt(valString);

	if (!GetValueString("xadvance", line, valString))
	{
		printf("Failed to parse char xadvance from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.xadvance = ParseAsInt(valString);

	if (!GetValueString("page", line, valString))
	{
		printf("Failed to parse char page from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.page = ParseAsInt(valString);

	if (!GetValueString("chnl", line, valString))
	{
		printf("Failed to parse char chnl from line %s\n", line.c_str());
		return false;
	}
	outFontCharData.chnl = ParseAsInt(valString);

	return true;
}

bool ParsePage(const std::string& line, FontData& outFontData)
{
	std::string valString;
	if (!GetValueString("file", line, valString, '\n'))
	{
		printf("Failed to parse char file from line %s\n", line.c_str());
		return false;
	}

	auto aIt = std::remove(valString.begin(), valString.end(), '\"');
	valString.erase(aIt, valString.end());

	outFontData.file = valString;

	return true;
}

bool Font::Load(const std::string& path)
{
	std::ifstream is;
	std::string line{ "" };

	is.open(path);

	if (!is.is_open())
	{
		printf("Failed to open font file %s\n", path.c_str());
		return false;
	}

	while (std::getline(is, line))
	{
		line += '\n';
		auto sectionEnd = line.find_first_of(' ');
		std::string section(line.begin(), line.begin() + sectionEnd);

		if (section == "common")
		{
			if (!ParseCommonFontData(line, fontData))
			{
				printf("failed to parse common data \n");
				return false;
			}
		}
		else if (section == "page")
		{
			if (!ParsePage(line, fontData))
			{
				printf("failed to parse page fontdata \n");
				return false;
			}
		}
		else if (section == "chars")
		{
			if (!ParseCharsData(line, fontData))
			{
				printf("failed to parse chars data \n");
				return false;
			}
		}
		else if (section == "char")
		{
			char charId;
			FontCharData fcd;
			if (!ParseChar(line, charId, fcd))
			{
				printf("failed to parse a character \n");
				return false;
			}
			fontMap[charId] = fcd;
		}		
	};
	
	is.close();

	auto rpp = path.find_last_of('/');
	const std::string texturePath(path.begin(), path.begin() + rpp);

	if (!LoadTexture(texturePath + '/' + fontData.file))
	{
		printf("Failed to load font texture at path %s\n", texturePath.c_str());
		return false;
	}

	return true;
}

bool Font::GetCharPosition(char charId, glm::vec4& outPos, glm::vec4& outTex, glm::vec2& tempOffset)
{
	auto& cd = fontMap[charId];
	float w = fontData.scaleW;
	float h = fontData.scaleH;

	outPos.x = tempOffset.x;
	outPos.z = ((float)(cd.width)/ w);
	outPos.w = ((float)(cd.height) / h);
	outPos.y = tempOffset.y;

	tempOffset.x += ((float)cd.xadvance / w) * 0.4f;

	outTex.x = ((float)(cd.x) / w);
	outTex.y = (float)(cd.y + cd.height) / h;
	outTex.y = 1.f - outTex.y;
	outTex.z = (float)cd.width / w;
	outTex.w = (float)cd.height / h;

	return true;
}

bool Font::LoadTexture(const std::string& path)
{
	auto* re = RenderEngine::Get();
	auto* tm = re->GetTextureManager();
	return tm->LoadTexture(path, atlas, Texture::Diffuse, 9987, 9729, true);
}
