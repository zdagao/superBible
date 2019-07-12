/*
 * Copyright © 2012-2013 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sb6.h>
#include <vmath.h>
#include <string>

class tessellatedtri_app : public sb6::application
{
    void init()
    {
        static const char title[] = "OpenGL SuperBible - Tessellated Triangle";

        sb6::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    virtual void startup()
    {
        static const char * vs_source[] =
        {
            "#version 410 core                                                 \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4(-0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4( 0.25,  0.25, 0.5, 1.0)); \n"
            "                                                                  \n"
            "    gl_Position = vertices[gl_VertexID];                          \n"
            "}                                                                 \n"
        };

        static const char * tcs_source[] =
        {
            "#version 410 core                                                                 \n"
            "                                                                                  \n"
            "layout (vertices = 3) out;                                                        \n"
            "                                                                                  \n"
            "void main(void)                                                                   \n"
            "{                                                                                 \n"
            "    if (gl_InvocationID == 0)                                                     \n"
            "    {                                                                             \n"
            "        gl_TessLevelInner[0] = 5.0;                                               \n"
            "        gl_TessLevelOuter[0] = 5.0;                                               \n"
            "        gl_TessLevelOuter[1] = 5.0;                                               \n"
            "        gl_TessLevelOuter[2] = 5.0;                                               \n"
            "    }                                                                             \n"
            "    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;     \n"
            "}                                                                                 \n"
        };

        static const char * tes_source[] =
        {
            "#version 410 core                                                                 \n"
            "                                                                                  \n"
            "layout (triangles, equal_spacing, cw) in;                                         \n"

			"subroutine vec4 sub_mySubroutine(vec4 v0, vec4 v1, vec4 v2); \n"
			"subroutine (sub_mySubroutine)                                                      \n"
			"vec4 lerp3D(vec4 v0, vec4 v1, vec4 v2)                                          \n"
			"{                                                                                 \n"
			"   return vec3(0.5*gl_TessCoord.x)*v0 + vec3(gl_TessCoord.y)*v1 + vec3(gl_TessCoord.z)*v2;\n"
			"}                                                                                  \n"
			"subroutine (sub_mySubroutine)                                                      \n"
			"vec4 lerp3D1(vec4 v0, vec4 v1, vec4 v2)                                          \n"
			"{                                                                                 \n"
			"   return vec3(0.2*gl_TessCoord.x)*v0 + vec3(gl_TessCoord.z)*v1 + vec3(gl_TessCoord.y)*v2;\n"
			"}                                                                                  \n"
			"subroutine uniform sub_mySubroutine mySubroutineUniform; \n"
			"                                                                                  \n"
			"void main(void)                                                                   \n"
            "{                                                                                 \n"
			"    gl_Position = mySubroutineUniform(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);                       \n"
			//"    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +                       \n"
            //"                  (gl_TessCoord.y * gl_in[1].gl_Position) +                       \n"
            //"                  (gl_TessCoord.z * gl_in[2].gl_Position);                        \n"
            "}                                                                                 \n"
        };

        static const char * fs_source[] =
        {
            "#version 410 core                                                 \n"
            "                                                                  \n"
            "out vec4 color;                                                   \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    color = vec4(0.0, 0.8, 1.0, 1.0);                             \n"
            "}                                                                 \n"
        };

        program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);

        GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcs, 1, tcs_source, NULL);
        glCompileShader(tcs);

        GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tes, 1, tes_source, NULL);
        glCompileShader(tes);

		GLint log_length;
		glGetShaderiv(tes, GL_INFO_LOG_LENGTH, &log_length);

		char *str = new char[log_length];
		glGetShaderInfoLog(tes, log_length, NULL, str);
		fprintf(stdout, "log:%s \n", str);
		delete(str);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

        glAttachShader(program, vs);
        glAttachShader(program, tcs);
        glAttachShader(program, tes);
        glAttachShader(program, fs);

        glLinkProgram(program);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		float a = 2.0f;
		vmath::vec3 v = vmath::vec3(1.0f, 2.0f, 3.0f);
		vmath::vec3 v0;

		v0 = (vmath::vec3)(a);
		v0 = v0 * v;

		fprintf(stdout, "v = (%f, %f, %f)\n", v0[0], v0[1], v0[2]);
    }

    virtual void render(double currentTime)
    {
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, green);

		GLuint idx = glGetProgramResourceIndex(program, GL_TESS_EVALUATION_SUBROUTINE, "lerp3D1");
		if (idx != GL_INVALID_VALUE)
		{
			fprintf(stdout, "set subroutine lerp3D1 %d \n", idx);
			glUniformSubroutinesuiv(GL_TESS_EVALUATION_SHADER, 1, &idx);
		}

        glUseProgram(program);
        glDrawArrays(GL_PATCHES, 0, 3);
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

private:
    GLuint          program;
    GLuint          vao;
};

DECLARE_MAIN(tessellatedtri_app)
