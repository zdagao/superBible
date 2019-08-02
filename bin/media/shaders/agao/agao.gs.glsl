#version 330 core
/*
#extension GL_ARB_gpu_shader5 : require

layout (triangles, invocations = 2) in;
layout (triangle_strip, max_vertices = 3) out;

uniform matrices
{
	mat4 model_matrix;
	mat4 view_matrix[2];
	mat4 projection_matrix;
};

in VS_OUT
{
	vec4 color;
	vec3 normal;
	vec2 texture_coord;
} gs_in[];

out GS_OUT
{
	vec4 color;
	vec3 normal;
	vec2 texture_coord;
} gs_out;

void main(void)
{
	// Calculate a model-view matrix for the current eye
	mat4 model_view_matrix = view_matrix[gl_InvocationID] *
												model_matrix;
	for (int i = 0; i < gl_in.length(); i++){
		// Output layer is invocation ID
		gl_Layer = gl_InvocationID;
		// Multiply by the model matrix, the view matrix for the
		// appropriate eye, and then the projection matrix
		gl_Position = projection_matrix *
						model_view_matrix *
						gl_in[i].gl_Position;
		gs_out.color = gs_in[i].color;
		// Don't forget to transform the normals...
		gs_out.normal = mat3(model_view_matrix) * gs_in[i].normal;
		gs_out.texture_coord = gs_in[i].texture_coord;
		EmitVertex();
	}
	EndPrimitive();
}
*/

#extension GL_ARB_gpu_shader5 : require

layout (triangles, invocations = 2) in;
layout (triangle_strip, max_vertices = 3) out;

out GS_OUT
{
	vec4 color;
} gs_out;

in vec4 vs_color[];

void main(void)
{
	int i;
	const vec3 colors[] = vec3[](vec3( 0.5, 0.0, 0.0),
									vec3(0.0, 0.5, 0.0),
									vec3( 0.0,  0.0, 0.5),
									vec3( 0.5,  0.5, 0.5));
	for (i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = gl_in[i].gl_Position;
		gs_out.color = vs_color[i] + vec4(colors[i].xyz*gl_InvocationID, 0.0);
		EmitVertex();
	}
	EndPrimitive();
}

