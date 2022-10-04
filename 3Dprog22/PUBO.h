#pragma once
/// <summary>
/// Pixel unpack buffer object
/// </summary>
struct PUBO
{
	unsigned id{0xffffffff};

	bool IsValid() const
	{
		return id != 0xffffffff;
	}
};

