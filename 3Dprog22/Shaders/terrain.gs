#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

uniform vec4 light_position;
uniform float voxel_size;

uniform mat4 gxl3d_ModelViewProjectionMatrix;
uniform mat4 gxl3d_ModelViewMatrix;
uniform mat4 gxl3d_ViewMatrix;

layout(r8, location = 4) uniform image3D texture3D;

in Vertex
{
	vec4 normal;
	vec4 color;
}vertex[];

out vec3 vertex_position;
out vec4 vertex_color;
out vec4 vertex_normal;
out vec4 vertex_eyevec;
out vec4 vertex_lightdir;

void EmitQuad(vec4 p0, vec4 p1, vec4 p2, vec4 p3, vec4 light_pos_view_space)
{
	vec4 V0 = p0 - p1;
	vec4 V1 = p2 - p1;

	vec3 N = cross(V1.xyz, V0.xyz);
	N = normalize(N);

	vec4 v_normal = gxl3d_ModelViewMatrix * vec4(N, 0.0);
	gl_Position = gxl3d_ModelViewProjectionMatrix * p0;
	vertex_color = vec4(1.0, 1.0, 0.0, 1.0);
	vertex_normal = v_normal;
	vec4 view_vertex = gxl3d_ModelViewMatrix * p0;
	vertex_lightdir = light_pos_view_space - view_vertex;
	vertex_eyevec = -view_vertex;
	EmitVertex();

	gl_Position = gxl3d_ModelViewProjectionMatrix * p1;
	vertex_color = vec4(1.0, 1.0, 0.0, 1.0);
	vertex_normal = v_normal;
	view_vertex = gxl3d_ModelViewMatrix * p1;
	vertex_lightdir = light_pos_view_space - view_vertex;
	vertex_eyevec = -view_vertex;
	EmitVertex();

	gl_Position = gxl3d_ModelViewProjectionMatrix * p2;
	vertex_color = vec4(1.0, 1.0, 0.0, 1.0);
	vertex_normal = v_normal;
	view_vertex = gxl3d_ModelViewMatrix * p2;
	vertex_lightdir = light_pos_view_space - view_vertex;
	vertex_eyevec = -view_vertex;
	EmitVertex();

	gl_Position = gxl3d_ModelViewProjectionMatrix * p3;
	vertex_color = vec4(1.0, 1.0, 0.0, 1.0);
	vertex_normal = v_normal;
	view_vertex = gxl3d_ModelViewMatrix * p3;
	vertex_lightdir = light_pos_view_space - view_vertex;
	vertex_eyevec = -view_vertex;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	int i;

	vec3 center = vec3((
		gl_in[0].gl_Position.xyz +
		gl_in[1].gl_Position.xyz +
		gl_in[2].gl_Position.xyz) / 3.0);

	vec4 P = vec4(center, 1.0);
	imageStore(texture3D, ivec3(5,6,7), vec4(1.0, 0.0, 0.0, 1.0));
	vec4 lightPosViewSpace = gxl3d_ViewMatrix * light_position;

	//============================================================
  // Front
  //============================================================
	vec4 P0;
	P0.x = P.x - voxel_size / 2.0;
	P0.y = P.y - voxel_size / 2.0;
	P0.z = P.z + voxel_size / 2.0;
	P0.w = 1.0;

	vec4 P1;
	P1.x = P.x - voxel_size / 2.0;
	P1.y = P.y + voxel_size / 2.0;
	P1.z = P.z + voxel_size / 2.0;
	P1.w = 1.0;

	vec4 P2;
	P2.x = P.x + voxel_size / 2.0;
	P2.y = P.y - voxel_size / 2.0;
	P2.z = P.z + voxel_size / 2.0;
	P2.w = 1.0;

	vec4 P3;
	P3.x = P.x + voxel_size / 2.0;
	P3.y = P.y + voxel_size / 2.0;
	P3.z = P.z + voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);


	//============================================================
	// Back
	//============================================================
	P0.x = P.x - voxel_size / 2.0;
	P0.y = P.y - voxel_size / 2.0;
	P0.z = P.z - voxel_size / 2.0;
	P0.w = 1.0;

	P1.x = P.x - voxel_size / 2.0;
	P1.y = P.y + voxel_size / 2.0;
	P1.z = P.z - voxel_size / 2.0;
	P1.w = 1.0;

	P2.x = P.x + voxel_size / 2.0;
	P2.y = P.y - voxel_size / 2.0;
	P2.z = P.z - voxel_size / 2.0;
	P2.w = 1.0;

	P3.x = P.x + voxel_size / 2.0;
	P3.y = P.y + voxel_size / 2.0;
	P3.z = P.z - voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);


	//============================================================
	// Right
	//============================================================
	P0.x = P.x + voxel_size / 2.0;
	P0.y = P.y - voxel_size / 2.0;
	P0.z = P.z + voxel_size / 2.0;
	P0.w = 1.0;

	P1.x = P.x + voxel_size / 2.0;
	P1.y = P.y + voxel_size / 2.0;
	P1.z = P.z + voxel_size / 2.0;
	P1.w = 1.0;

	P2.x = P.x + voxel_size / 2.0;
	P2.y = P.y - voxel_size / 2.0;
	P2.z = P.z - voxel_size / 2.0;
	P2.w = 1.0;

	P3.x = P.x + voxel_size / 2.0;
	P3.y = P.y + voxel_size / 2.0;
	P3.z = P.z - voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);


	//============================================================
	// Left
	//============================================================
	P0.x = P.x - voxel_size / 2.0;
	P0.y = P.y - voxel_size / 2.0;
	P0.z = P.z - voxel_size / 2.0;
	P0.w = 1.0;

	P1.x = P.x - voxel_size / 2.0;
	P1.y = P.y + voxel_size / 2.0;
	P1.z = P.z - voxel_size / 2.0;
	P1.w = 1.0;

	P2.x = P.x - voxel_size / 2.0;
	P2.y = P.y - voxel_size / 2.0;
	P2.z = P.z + voxel_size / 2.0;
	P2.w = 1.0;

	P3.x = P.x - voxel_size / 2.0;
	P3.y = P.y + voxel_size / 2.0;
	P3.z = P.z + voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);


	//============================================================
	// Top
	//============================================================
	P0.x = P.x - voxel_size / 2.0;
	P0.y = P.y + voxel_size / 2.0;
	P0.z = P.z + voxel_size / 2.0;
	P0.w = 1.0;

	P1.x = P.x - voxel_size / 2.0;
	P1.y = P.y + voxel_size / 2.0;
	P1.z = P.z - voxel_size / 2.0;
	P1.w = 1.0;

	P2.x = P.x + voxel_size / 2.0;
	P2.y = P.y + voxel_size / 2.0;
	P2.z = P.z + voxel_size / 2.0;
	P2.w = 1.0;

	P3.x = P.x + voxel_size / 2.0;
	P3.y = P.y + voxel_size / 2.0;
	P3.z = P.z - voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);


	//============================================================
	// Bottom
	//============================================================
	P0.x = P.x - voxel_size / 2.0;
	P0.y = P.y - voxel_size / 2.0;
	P0.z = P.z - voxel_size / 2.0;
	P0.w = 1.0;

	P1.x = P.x - voxel_size / 2.0;
	P1.y = P.y - voxel_size / 2.0;
	P1.z = P.z + voxel_size / 2.0;
	P1.w = 1.0;

	P2.x = P.x + voxel_size / 2.0;
	P2.y = P.y - voxel_size / 2.0;
	P2.z = P.z - voxel_size / 2.0;
	P2.w = 1.0;

	P3.x = P.x + voxel_size / 2.0;
	P3.y = P.y - voxel_size / 2.0;
	P3.z = P.z + voxel_size / 2.0;
	P3.w = 1.0;

	EmitQuad(P0, P1, P2, P3, lightPosViewSpace);
}
