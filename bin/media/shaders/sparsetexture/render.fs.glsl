#version 330 core

uniform sampler2D sparseTex;

layout (std140) uniform TEXURE_BLOCK
{
    uint foo;
};

in vec2 uv;

layout (location = 0) out vec4 o_color;

void main(void)
{
    // o_color = vec4(0.3, 0.6, 0.0, 1.0);
    o_color = textureLod(sparseTex, uv, 0.0);
}
