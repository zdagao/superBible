#version 330 core

out vec4 color;

in GS_OUT
{
	vec4 color;
} gs_in;

void main(void)
{
	 color = gs_in.color;
}

