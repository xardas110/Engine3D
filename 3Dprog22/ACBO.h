#pragma once

/// <summary>
/// Atomic Counter Buffer Object
/// </summary>
struct ACBO
{
	unsigned id{0xffffffff};

	bool IsValid() const
	{
		return id != 0xffffffff;
	}
};

