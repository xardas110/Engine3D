#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

struct VoxelGPUData
{
	vec4 gridCenter;
	vec4 gridSize;
	ivec4 dims;
	mat4 projX, projY, projZ;
};

layout (std140, binding=6) uniform VoxelData
{
	VoxelGPUData voxelData;
};

in Vertex
{
	vec3 texCoords;
	vec3 normal;
}In[3];

out GeometryOut
{
	vec3 fragWS;
	vec3 lightWS;
	vec3 normal;
	vec3 pos;
	vec3 texCoords;
	mat4 projection;
	flat int axis;
	flat vec4 triangleAABB;
} Out;

vec4 AABB(vec4 pos[3], vec2 pixelDiagonal)
{
	vec4 aabb;

	aabb.xy = min(pos[2].xy, min(pos[1].xy, pos[0].xy));
	aabb.zw = max(pos[2].xy, max(pos[1].xy, pos[0].xy));

	// enlarge by half-pixel
	aabb.xy -= pixelDiagonal;
	aabb.zw += pixelDiagonal;

	return aabb;
}

const float voxelScale = 1.f / float(voxelData.gridSize.x);
vec3 minPoint = voxelData.gridCenter.xyz - vec3(voxelData.gridSize.xyz / 2.0);

void main()
{
/*
	vec3 e1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 e2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 norm = normalize(cross(e1, e2));

	float ndx = abs(norm.x); float ndy = abs(norm.y); float ndz= abs(norm.z);

	if (ndx > ndy && ndx > ndz){	
		Out.projection = voxelData.projX;
		Out.axis = 1;
	}
	else if (ndy > ndx && ndy > ndz)
	{
		Out.projection = voxelData.projY;
		Out.axis = 2;
	}
	else{
		Out.projection = voxelData.projZ;
		Out.axis = 3;
	}
	
	
	for(int i=0; i < gl_in.length(); i++)
	{
		Out.texCoords = In[i].texCoords;
		Out.fragWS = gl_in[i].gl_Position.xyz;
		Out.normal = In[i].normal;

		gl_Position = Out.projection * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
	*/

	//Above is the old approach

	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 faceNormal = cross(p1, p2);

	float nDX = abs(faceNormal.x);
	float nDY = abs(faceNormal.y);
	float nDZ = abs(faceNormal.z);

	mat4 viewProjection;
	mat4 viewProjectionI;

	if( nDX > nDY && nDX > nDZ )
    {
		viewProjection = voxelData.projX;
	}
	else if( nDY > nDX && nDY > nDZ  )
    {
	   viewProjection = voxelData.projY;
    }
	else
    {
	    viewProjection = voxelData.projZ;
	}
	viewProjectionI = inverse(viewProjection);

	vec3 texCoord[3];

    for (int i = 0; i < gl_in.length(); i++)
    {
        texCoord[i] = In[i].texCoords; 
    }

	//transform vertices to clip space
	vec4 pos[3] = vec4[3]
	(
		viewProjection * gl_in[0].gl_Position,
		viewProjection * gl_in[1].gl_Position,
		viewProjection * gl_in[2].gl_Position
	);

	vec4 trianglePlane;
	trianglePlane.xyz = cross(pos[1].xyz - pos[0].xyz, pos[2].xyz - pos[0].xyz);
	trianglePlane.xyz = normalize(trianglePlane.xyz);
	trianglePlane.w = -dot(pos[0].xyz, trianglePlane.xyz);

    // change winding, otherwise there are artifacts for the back faces.
    if (dot(trianglePlane.xyz, vec3(0.0, 0.0, 1.0)) < 0.0)
    {
        vec4 vertexTemp = pos[2];
        vec3 texCoordTemp = texCoord[2];
        
        pos[2] = pos[1];
        texCoord[2] = texCoord[1];
    
        pos[1] = vertexTemp;
        texCoord[1] = texCoordTemp;
    }

	vec2 halfPixel = vec2(0.);

	if(trianglePlane.z == 0.0f) return;
	// expanded aabb for triangle
	Out.triangleAABB = AABB(pos, halfPixel);
	// calculate the plane through each edge of the triangle
	// in normal form for dilatation of the triangle
	vec3 planes[3];
	planes[0] = cross(pos[0].xyw - pos[2].xyw, pos[2].xyw);
	planes[1] = cross(pos[1].xyw - pos[0].xyw, pos[0].xyw);
	planes[2] = cross(pos[2].xyw - pos[1].xyw, pos[1].xyw);
	planes[0].z -= dot(halfPixel, abs(planes[0].xy));
	planes[1].z -= dot(halfPixel, abs(planes[1].xy));
	planes[2].z -= dot(halfPixel, abs(planes[2].xy));
	// calculate intersection between translated planes
	vec3 intersection[3];
	intersection[0] = cross(planes[0], planes[1]);
	intersection[1] = cross(planes[1], planes[2]);
	intersection[2] = cross(planes[2], planes[0]);
	intersection[0] /= intersection[0].z;
	intersection[1] /= intersection[1].z;
	intersection[2] /= intersection[2].z;
	// calculate dilated triangle vertices
	float z[3];
	z[0] = -(intersection[0].x * trianglePlane.x + intersection[0].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[1] = -(intersection[1].x * trianglePlane.x + intersection[1].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[2] = -(intersection[2].x * trianglePlane.x + intersection[2].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	pos[0].xyz = vec3(intersection[0].xy, z[0]);
	pos[1].xyz = vec3(intersection[1].xy, z[1]);
	pos[2].xyz = vec3(intersection[2].xy, z[2]);

	for(int i = 0; i < 3; ++i)
	{
		vec4 voxelPos = viewProjectionI * pos[i];
		voxelPos.xyz /= voxelPos.w;
		voxelPos.xyz -= minPoint;
		voxelPos *= voxelScale;

		gl_Position = pos[i];
		Out.pos = pos[i].xyz;
		Out.normal = In[i].normal;
		Out.texCoords = texCoord[i];
		Out.fragWS = voxelPos.xyz * voxelData.gridSize.xyz;

		EmitVertex();
	}

	EndPrimitive();
}
