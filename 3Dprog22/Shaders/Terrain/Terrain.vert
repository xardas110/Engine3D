#version 460

layout (location = 0) in vec2 position0;

//https://www.youtube.com/watch?v=z03vg2QTA8k&t=2053s&ab_channel=OREONENGINE

#include "../common/Camera.frag"

out vec2 mapCoord_TC;

uniform mat4 localMatrix;
uniform mat4 worldMatrix;

uniform float scaleY;
uniform int lod;
uniform vec2 index;
uniform float gap;
uniform vec2 location;
uniform int lod_morph_area[8];

uniform sampler2D heightmap;

float MorphX(vec2 position);
float MorphY(vec2 position);
vec2 Morph(vec2 localPos, int morphArea);

void main(){

	vec2 localPosition = (localMatrix * vec4(position0.x,0,position0.y,1)).xz;
	
	if (lod > 0) localPosition += Morph(localPosition, lod_morph_area[lod-1]);
		
	float height = texture(heightmap, localPosition).r;
	
	mapCoord_TC = localPosition;
	gl_Position = worldMatrix * vec4(localPosition.x, height,localPosition.y,1);
}

vec2 Morph(vec2 localPos, int morphArea){
	
	vec2 morphing = vec2(0,0);
	
	vec2 fixPointX = vec2(0,0);
	vec2 fixPointY = vec2(0,0);
	float distX;
	float distY;
	
	//Quad indexes inside the quadtree
	if (index == vec2(0,0)) 
	{
		fixPointX = location + vec2(gap,0);
		fixPointY = location + vec2(0,gap);
	}
	else if (index == vec2(1,0)) 
	{
		fixPointX = location;
		fixPointY = location + vec2(gap,gap);
	}
	else if (index == vec2(0,1)) 
	{
		fixPointX = location + vec2(gap,gap);
		fixPointY = location;
	}
	else if (index == vec2(1,1)) 
	{
		fixPointX = location + vec2(0,gap);
		fixPointY = location + vec2(gap,0);
	}
	
	float heightFactor = 0;
	if (viewPos.y > abs(scaleY))
		heightFactor = 1;
	else
		heightFactor = viewPos.y / abs(scaleY);
		
	distX = length(viewPos - (worldMatrix * vec4(fixPointX.x,heightFactor,fixPointX.y,1)).xyz);				
	distY = length(viewPos - (worldMatrix * vec4(fixPointY.x,heightFactor,fixPointY.y,1)).xyz);
									
	if (distX > morphArea) morphing.x = MorphX(localPos.xy);		
	if (distY > morphArea) morphing.y = MorphY(localPos.xy);
			
	return morphing;
}

float MorphX(vec2 position) {
	
	vec2 frac = position - location;
	
	if (index == vec2(0,0))
	{
		float morph = frac.x - frac.y;
		if (morph > 0)return morph;		
	}
	if (index == vec2(1,0))
	{
		float morph = gap - frac.x - frac.y;
		if (morph > 0)return morph;		
	}
	if (index == vec2(0,1))
	{
		float morph = frac.x + frac.y - gap;
		if (morph > 0)return -morph;		
	}
	if (index == vec2(1,1))
	{
		float morph = frac.y - frac.x;
		if (morph > 0) return -morph;			
	}
	return 0;
}

float MorphY(vec2 position) {
	
	vec2 frac = position - location;
	
	if (index == vec2(0,0))
	{
		float morph = frac.y - frac.x;
		if (morph > 0) return -morph;	
	}
	if (index == vec2(1,0))
	{
		float morph = frac.y - (gap - frac.x);
		if (morph > 0) return morph;		
	}
	if (index == vec2(0,1)){
		float morph = gap - frac.y - frac.x;
		if (morph > 0) return -morph;	
	}
	if (index == vec2(1,1)){
		float morph = frac.x - frac.y;
		if (morph > 0) return morph;	
	}
	return 0;
}