#include <sb6.h>
#include <sb6ktx.h>
#include <vmath.h>

#include <cmath>
#include <shader.h>


static unsigned int seed = 0x13733713;
static const GLfloat one = 1.0;
#define CheckGLErr() {checkGLErr(__LINE__);}

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
		//info.flags.stereo = 1;
		//info.flags.fullscreen = 1;
    }

    void startup()
    {
	    GLuint  vs, fs, gs;
		
		vs = sb6::shader::load("../bin/media/shaders/agao/agao.vs.glsl", GL_VERTEX_SHADER);
		checkShaderCompile(vs, "render_prog vs");

		fs = sb6::shader::load("../bin/media/shaders/agao/agao.fs.glsl", GL_FRAGMENT_SHADER);
		checkShaderCompile(fs, "render_prog fs");

		gs = sb6::shader::load("../bin/media/shaders/agao/agao.gs.glsl", GL_GEOMETRY_SHADER);
		checkShaderCompile(fs, "render_prog gs");

		render_prog = glCreateProgram();
        glAttachShader(render_prog, vs);
		glAttachShader(render_prog, gs);
        glAttachShader(render_prog, fs);
		glLinkProgram(render_prog);

		glDeleteShader(vs);
		glDeleteShader(gs);
		glDeleteShader(fs);

		vs = sb6::shader::load("../bin/media/shaders/agao/strender.vs.glsl", GL_VERTEX_SHADER);
		checkShaderCompile(vs, "strender vs");

		fs = sb6::shader::load("../bin/media/shaders/agao/strender.fs.glsl", GL_FRAGMENT_SHADER);
		checkShaderCompile(fs, "strender fs");

		strender = glCreateProgram();
        glAttachShader(strender, vs);
        glAttachShader(strender, fs);
		glLinkProgram(strender);

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

		glGenTextures(1, &color_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, color_texture);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 512, 512, 2);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLuint depth_texture;
		glGenTextures(1, &depth_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, depth_texture);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, 512, 512, 2);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glFramebufferTexture(GL_FRAMEBUFFER,
							GL_COLOR_ATTACHMENT0,
							color_texture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER,
							GL_DEPTH_ATTACHMENT,
							depth_texture, 0);

		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_buffers);
	}

    void render(double currentTime)
    {
	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        static const GLfloat green[] = { 0.0f, 1.0f, 0.0f, 0.0f };
		glViewport(0, 0, 640, 640);
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);
		float t = (float)currentTime;

		glUseProgram(render_prog);
		CheckGLErr();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		CheckGLErr();

		static const vmath::vec3 origin(0.0f);
		static const vmath::vec3 up_vector(0.0f, 1.0f, 0.0f);
		static const vmath::vec3 eye_separation(0.01f, 0.0f, 0.0f);
		static const vmath::vec3 eye_location(0.0f, 0.0f, -0.4f);

		vmath::mat4 left_view_matrix =
							vmath::lookat(eye_location - eye_separation,
										origin,
										up_vector);

		vmath::mat4 right_view_matrix =
							vmath::lookat(eye_location + eye_separation,
										origin,
										up_vector);

		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//glDrawBuffer(GL_BACK);
		//CheckGLErr();
		//glClearBufferfv(GL_COLOR, 0, black);
		///glClearBufferfv(GL_DEPTH, 0, &one);
		//glDrawBuffer(GL_BACK_LEFT);

		//draw_scene(left_view_matrix);
		draw_scene();
	}

	private:
		void draw_scene()
		{
		    glBindFramebuffer(GL_FRAMEBUFFER, 0);
			static const GLfloat blue[] = { 1.0f, 1.0f, 0.0f, 0.0f };
			glViewport(0, 0, info.windowWidth, info.windowHeight);
			glClearBufferfv(GL_COLOR, 0, blue);
			glClearBufferfv(GL_DEPTH, 0, &one);
			
			glBindTexture(GL_TEXTURE_2D_ARRAY, color_texture);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			glUseProgram(strender);
			/*
			vmath::mat4 model_matrix = vmath::mat4::identity();
			

			glUniformMatrix4fv(model_view_loc, 1, GL_FALSE,
							eye_view_matrix * model_matrix);
			*/

			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 2);
			
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		
		void checkShaderCompile(GLuint shader, const GLchar * desc)
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			glGetShaderInfoLog(shader, 1024, NULL, buffer);
			
			if (strlen(buffer) != 0)
				fprintf(stderr, "error happened in %s shader(%d): %s\n", desc, shader, buffer);
		}

		void checkGLErr(GLint line)
		{
			for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
				fprintf(stdout, "line %d: Error found 0x%04x\n", line, err);
		}

	protected:
		GLuint			strender;
		GLuint			render_prog;
		GLuint			render_vao;
		
		GLuint			tex_alien_array;
		GLuint			rain_buffer;
		
		float			droplet_x_offset[256];
		float			droplet_rot_speed[256];
		float			droplet_fall_speed[256];
		
		GLuint fbo;
		GLuint color_texture;
		GLint model_view_loc;
};

DECLARE_MAIN(agao_app)

