#pragma once

#define ACBO_INVALID 0xffffffff

/// <summary>
/// Atomic Counter Buffer Object
/// </summary>
struct ACBO
{
    unsigned id{ACBO_INVALID};

	bool IsValid() const
	{
        return id != ACBO_INVALID;
	}
};

