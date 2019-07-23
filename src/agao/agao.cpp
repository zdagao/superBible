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
	}

    void render(double currentTime)
    {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float t = (float)currentTime;
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		
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
	}

	protected:
		GLuint			render_prog;
		GLuint			render_vao;
		
		GLuint			tex_alien_array;
		GLuint			rain_buffer;
		
		float			droplet_x_offset[256];
		float			droplet_rot_speed[256];
		float			droplet_fall_speed[256];
};

DECLARE_MAIN(agao_app)

