#pragma once

#define SSBO_INVALID 0xffffffff

struct SSBO
{
    unsigned id = SSBO_INVALID;

    bool IsValid() const
	{
        return id != SSBO_INVALID;
	}
};

