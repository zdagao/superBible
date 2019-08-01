#version 330 core

out vec4 color;

uniform sampler2DArray stereo_buffer;

in VS_OUT
{
    vec3 tc;
} fs_in;

void main(void)
{
    color = texture(stereo_buffer, fs_in.tc);
    //color = vec4(1.0, 0.0, 0.0, 1);
}
