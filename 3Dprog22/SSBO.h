#pragma once
struct SSBO
{
	unsigned id;

	[[nodiscard]] bool IsValid() const
	{
		return id != 0U;
	}
};

