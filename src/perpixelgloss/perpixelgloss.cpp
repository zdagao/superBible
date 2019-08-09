/*
 * Copyright � 2012-2013 Graham Sellers
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

#include <object.h>
#include <shader.h>
#include <sb6ktx.h>

static void checkShaderCompile(GLuint shader, const GLchar * desc)
{
	if (shader == 0) {
		fprintf(stderr, "invalid shader.\n");
		return;
	}
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	glGetShaderInfoLog(shader, 1024, NULL, buffer);
	
	if (strlen(buffer) != 0)
		fprintf(stderr, "error happened in %s shader(%d): %s\n", desc, shader, buffer);
}

#define CheckGLErr() {checkGLErr(__LINE__);}
static void checkGLErr(GLint line)
{
	for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
		fprintf(stdout, "line %d: Error found 0x%04x\n", line, err);
}

class perpixelgloss_app : public sb6::application
{
public:
    perpixelgloss_app()
        : render_prog(0)
    {
    }

protected:
    void init()
    {
        static const char title[] = "OpenGL SuperBible - Per-Pixel Gloss";

        sb6::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    virtual void startup()
    {
        load_shaders();
		CheckGLErr();

        glActiveTexture(GL_TEXTURE0);
        tex_envmap = sb6::ktx::file::load("../bin/media/textures/envmaps/mountains3d.ktx");

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glActiveTexture(GL_TEXTURE1);
        tex_glossmap = sb6::ktx::file::load("../bin/media/textures/pattern1.ktx");

        object.load("../bin/media/objects/torus_nrms_tc.sbm");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

		CheckGLErr();
    }

    virtual void render(double currentTime)
    {
        static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, tex_envmap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_glossmap);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glUseProgram(render_prog);

		GLint location = glGetUniformLocation(render_prog, "tex_envmap");
		//fprintf(stdout, "location %d\n", location);
		glUniform1i(location, 0);
		//CheckGLErr();

		location = glGetUniformLocation(render_prog, "tex_glossmap");
		//fprintf(stdout, "tex_glossmap %d\n", tex_glossmap);
		glUniform1i(location, 1);
		//CheckGLErr();

        vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -3.0f) *
                                vmath::rotate((float)currentTime * 13.75f, 0.0f, 1.0f, 0.0f) *
                                vmath::rotate((float)currentTime * 7.75f, 0.0f, 0.0f, 1.0f) *
                                vmath::rotate((float)currentTime * 15.3f, 1.0f, 0.0f, 0.0f);

        glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);
		CheckGLErr();

        object.render();
		CheckGLErr();
    }

    virtual void shutdown()
    {
        glDeleteProgram(render_prog);
        glDeleteTextures(1, &tex_envmap);
    }

    void load_shaders()
    {
        if (render_prog)
            glDeleteProgram(render_prog);

        GLuint vs, fs;

        vs = sb6::shader::load("../bin/media/shaders/perpixelgloss/perpixelgloss.vs.glsl", GL_VERTEX_SHADER);
		checkShaderCompile(vs, "perpixelgloss vs");
        fs = sb6::shader::load("../bin/media/shaders/perpixelgloss/perpixelgloss.fs.glsl", GL_FRAGMENT_SHADER);
		checkShaderCompile(fs, "perpixelgloss fs");

        render_prog = glCreateProgram();
        glAttachShader(render_prog, vs);
        glAttachShader(render_prog, fs);
        glLinkProgram(render_prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        uniforms.mv_matrix = glGetUniformLocation(render_prog, "mv_matrix");
        uniforms.proj_matrix = glGetUniformLocation(render_prog, "proj_matrix");
    }

    virtual void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
                case 'R': load_shaders();
                    break;
            }
        }
    }

protected:
    GLuint          render_prog;

    GLuint          tex_envmap;
    GLuint          tex_glossmap;

    struct
    {
        GLint       mv_matrix;
        GLint       proj_matrix;
    } uniforms;

    sb6::object     object;
};

DECLARE_MAIN(perpixelgloss_app)
