#version 330 core

out vec4 vs_color;
void main(void)
{
	const vec4 vertices[] = vec4[](vec4( 1.0, -1.0, 1.0, 1.0),
									vec4(-1.0, -1.0, 1.0, 1.0),
									vec4( 1.0,  1.0, 1.0, 1.0),
									vec4( -1.0,  1.0, 1.0, 1.0));
	const vec4 colors[] = vec4[](vec4( 0.5, 0.0, 0.0, 1.0),
									vec4(0.0, 0.5, 0.0, 1.0),
									vec4( 0.0,  0.0, 0.5, 1.0),
									vec4( 0.5,  0.5, 0.5, 1.0));
	vs_color = colors[gl_VertexID];
	gl_Position = vertices[gl_VertexID];
}

