#include <sb6.h>
#include <sb6ktx.h>
#include <vmath.h>

#include <cmath>
#include <shader.h>


static unsigned int seed = 0x13733713;

static inline float random_float()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

class agao_app : public sb6::application
{
    void init()
    {
        static const char title[] = "agao";

        sb6::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void startup()
    {
	    GLuint  vs, fs;
		
		vs = sb6::shader::load("../bin/media/shaders/agao/agao.vs.glsl", GL_VERTEX_SHADER);

        char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
        glGetShaderInfoLog(vs, 1024, NULL, buffer);
		if (strlen(buffer) != 0)
			fprintf(stderr, "vs log: %s\n", buffer);

		fs = sb6::shader::load("../bin/media/shaders/agao/agao.fs.glsl", GL_FRAGMENT_SHADER);

		memset(buffer, 0, sizeof(buffer));
        glGetShaderInfoLog(fs, 1024, NULL, buffer);
		if (strlen(buffer) != 0)
			fprintf(stderr, "fs log: %s\n", buffer);

		
		render_prog = glCreateProgram();
        glAttachShader(render_prog, vs);
        glAttachShader(render_prog, fs);
		glLinkProgram(render_prog);

		glDeleteShader(vs);
		glDeleteShader(fs);

		//generate and bind vertex array object.
        glGenVertexArrays(1, &render_vao);
        glBindVertexArray(render_vao);

        tex_alien_array = sb6::ktx::file::load("../bin/media/textures/aliens.ktx");
		if (0 == tex_alien_array) {
	        fprintf(stderr, "Unable to create alien texture.\n");
		}
        glBindTexture(GL_TEXTURE_2D_ARRAY, tex_alien_array);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenBuffers(1, &rain_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, rain_buffer);
        glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_DRAW);

        for (int i = 0; i < 256; i++)
        {
            droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
            droplet_rot_speed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
            droplet_fall_speed[i] = random_float() + 0.2f;
        }

		glBindVertexArray(render_vao);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		fprintf(stdout, "size of long %lu %lu \n", sizeof(long), SIZE_MAX);
		vmath::vec4 v1(1.0, 2.0, 3.0, 4.0);
		vmath::vec4 v2(2.0, 1.0, 3.0, 4.0);
		vmath::mat4 m1(vmath::vec4(1.0, 2.0, 3.0, 4.0),
						vmath::vec4(4.0, 3.0, 2.0, 1.0),
						vmath::vec4(5.0, 6.0, 7.0, 8.0),
						vmath::vec4(8.0, 7.0, 6.0, 5.0));
		vmath::mat4 m2(vmath::vec4(1.0, 0.0, 1.0, 1.0),
						vmath::vec4(0.0, 1.0, 1.0, 0.0),
						vmath::vec4(1.0, 1.0, 1.0, 1.0),
						vmath::vec4(1.0, 0.0, 0.0, 1.0));
		//vmath::vec4 v3 = v1 / v2;
		vmath::mat4 m3 = vmath::mat4().identity() * m2;
		vmath::vec4 v3 = m3[0];
		fprintf(stdout, "vec4 %f %f %f %f\n", v3[0], v3[1], v3[2], v3[3]);

		/*
		glEnable(GL_SCISSOR_TEST);
		
		int scissor_width = (7 * info.windowWidth) / 16;
		int scissor_height = (7 * info.windowHeight) / 16;

		glScissorIndexed(0, 0, 0, scissor_width, scissor_height);

		glScissorIndexed(0,
						info.windowWidth - scissor_width, 0,
						info.windowWidth - scissor_width, scissor_height);

		glScissorIndexed(0,
						0, info.windowHeight - scissor_height,
						scissor_width, scissor_height);

		glScissorIndexed(0,
						info.windowWidth - scissor_width,
						info.windowHeight - scissor_height,
						scissor_width, scissor_height);
		*/

		
        static const char * stvs_source[] =
        {
            "#version 330 core                                                 \n"
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

        static const char * stfs_source[] =
        {
            "#version 330 core                                                 \n"
            "                                                                  \n"
            "out vec4 color;                                                   \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    color = vec4(0.0f, 0.0f, 0.0f, 0.0f);                         \n"
            "}                                                                 \n"
        };

        stprogram = glCreateProgram();
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, stfs_source, NULL);
        glCompileShader(fs);

        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, stvs_source, NULL);
        glCompileShader(vs);

        glAttachShader(stprogram, vs);
        glAttachShader(stprogram, fs);

        glLinkProgram(stprogram);

		glDeleteShader(vs);
		glDeleteShader(fs);

	}

    void render(double currentTime)
    {
        static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float t = (float)currentTime;
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, black);

		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_REPLACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		//glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 1, 0xff);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glUseProgram(stprogram);		
        glDrawArrays(GL_TRIANGLES, 0, 3);

		glEnable(GL_STENCIL_TEST);

		//glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_NOTEQUAL, 1, 0xff);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(render_prog);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, rain_buffer);
		
		vmath::vec4 * droplet =
					(vmath::vec4 *)glMapBufferRange(
												GL_UNIFORM_BUFFER,
												0,
												256 * sizeof(vmath::vec4),
												GL_MAP_WRITE_BIT |
												GL_MAP_INVALIDATE_BUFFER_BIT);
		
		for (int i = 0; i < 256; i++)
		{
			droplet[i][0] = droplet_x_offset[i];
			droplet[i][1] = 2.0f - fmodf((t + float(i)) *
										droplet_fall_speed[i], 4.31f);
			droplet[i][2] = t * droplet_rot_speed[i];
			droplet[i][3] = 0.0f;
		}		
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		
		int alien_index;
		for (alien_index = 0; alien_index < 256; alien_index++)
		{
			glVertexAttribI1i(0, alien_index);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		//glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);
	}

	protected:
		GLuint			stprogram;
		GLuint			render_prog;
		GLuint			render_vao;
		
		GLuint			tex_alien_array;
		GLuint			rain_buffer;
		
		float			droplet_x_offset[256];
		float			droplet_rot_speed[256];
		float			droplet_fall_speed[256];
};

DECLARE_MAIN(agao_app)

