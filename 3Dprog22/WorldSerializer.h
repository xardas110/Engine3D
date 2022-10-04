#pragma once
#include <string>


class WorldSerializer
{
	friend class World;
	
	WorldSerializer(class World* world);
	class World* world{nullptr};

	void Serialize(const std::string& path);
	void Deserialize(const std::string& path);
};

