#pragma once

#define PUBO_INVALID 0xffffffff

/// <summary>
/// Pixel unpack buffer object
/// </summary>
struct PUBO
{
    unsigned id{PUBO_INVALID};

	bool IsValid() const
	{
        return id != PUBO_INVALID;
	}
};

