#pragma once

#define UBO_INVALID 0xffffffff

/// <summary>
/// No ownership of UBOs, contains only data.
/// </summary>
struct UBO
{
    unsigned UBOId{UBO_INVALID};

	bool IsValid() const
	{
        return UBOId != UBO_INVALID;
	}
};
