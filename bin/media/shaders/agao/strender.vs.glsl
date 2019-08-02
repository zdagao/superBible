#version 330 core

out VS_OUT
{
    vec3 tc;
} vs_out;

void main(void)
{
	const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),
									vec4(-0.25, -0.25, 0.5, 1.0),
									vec4( 0.25,  0.25, 0.5, 1.0),
									vec4( 0.25,  0.25, 0.5, 1.0),
									vec4(-0.25, -0.25, 0.5, 1.0),
									vec4(-0.25, 0.25, 0.5, 1.0));
	gl_Position = vertices[gl_VertexID] - vec4(0.4, 0, 0, 0) + vec4(0.8*gl_InstanceID, 0.0, 0.0, 0.0);
	
    vs_out.tc = vec3(vertices[gl_VertexID].xy*2 + vec2(0.5), float(gl_InstanceID));
}
