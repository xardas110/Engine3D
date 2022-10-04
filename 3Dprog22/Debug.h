#pragma once
#include <iostream>
#include <string>

///prints vectors with variable name
///

#define VEC2(vec2) \
	std::cout << #vec2 << ": " << "(" << vec2.x << "," << vec2.y << ")" << std::endl;

#define VEC3(vec3) \
	std::cout << #vec3 << ": " << "(" << vec3.x << "," << vec3.y << "," << vec3.z << ")" << std::endl;

#define VEC4(vec4) \
	std::cout << #vec4 << ": " << "(" << vec4.x << "," <<  vec4.y << "," << vec4.z << "," << vec4.w << ")" << std::endl;

#define MAT4(MAT)\
	std::cout << #MAT << ": " << std::endl;\
	std::cout << "(" <<  MAT[0].x << "," <<   MAT[0].y << "," <<  MAT[0].z << "," <<  MAT[0].w << ")" << std::endl;\
	std::cout << "(" <<  MAT[1].x << "," <<  MAT[1].y << "," <<  MAT[1].z << "," <<  MAT[1].w << ")" << std::endl;\
	std::cout << "(" <<  MAT[2].x << "," <<  MAT[2].y << "," <<  MAT[2].z << "," <<  MAT[2].w << ")" << std::endl;\
	std::cout << "(" <<  MAT[3].x << "," <<  MAT[3].y << "," <<  MAT[3].z << "," <<  MAT[3].w << ")" << std::endl;

/// <summary>
/// TODO:Make a more robust float checker, too lazy right now
/// </summary>
inline bool IsFloat(const std::string& s)
{
	return s.find_first_not_of("-0123456789.") == std::string::npos;
}

inline bool GetFloatIfValid(const std::string& s, float & outFloat)
{
	if (!IsFloat(s))
		return false;
	
	outFloat = std::stof(s);
	return true;
}