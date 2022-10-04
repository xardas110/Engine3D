#pragma once

/// <summary>
/// No ownership of UBOs, contains only data.
/// </summary>
struct UBO
{
	unsigned UBOId{0u};

	[[nodiscard]]
	bool IsValid() const
	{
		return UBOId != 0;
	}
};