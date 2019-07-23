#include <sb6.h>
#include <sb6ktx.h>
#include <vmath.h>

#include <cmath>

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
		
		static const char * vs_source[] =
		{
			"#version 330 core														\n"
			"																		\n"
			"layout (location = 0) in vec4 position; 								\n"
			"uniform mat4 mv_matrix;													\n"
			"uniform mat4 proj_matrix;												\n"
			"layout (location = 1) in vec4 color;  									\n"
			"out vec4 vs_color; 														\n"
			"void main(void)														\n"
			"{																		\n"
			"	gl_Position = proj_matrix * mv_matrix * position;	 				\n"
			//"	gl_Position = position;					\n"
			"	vs_color = color;													\n"
			"}																		\n"
		};
		
        static const char * fs_source[] =
        {
            "#version 330 core                                                      \n"
            "                                                                       \n"
            "in vec4 vs_color;														\n"
            "out vec4 color;                                 						\n"
            "uniform sampler2D tex_object; 											\n"
			"uniform ivec2 wsize;													\n"
            "                                                                       \n"
            "void main(void)                                                        \n"
            "{                                                                      \n"
            //"    color = texelFetch(tex_object, ivec2(gl_FragCoord.x*16/wsize.x, gl_FragCoord.y*16/wsize.y), 0);			\n"
            "    color = texture(tex_object, gl_FragCoord.xy / textureSize(tex_object, 0));			\n"
			//"	 color = vs_color; 		\n"
            "}                                                                      \n"
        };

        vs = glCreateShader(GL_VERTEX_SHADER);		
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);

        char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
        glGetShaderInfoLog(vs, 1024, NULL, buffer);
		fprintf(stderr, "vs log: %s\n", buffer);

        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

		memset(buffer, 0, sizeof(buffer));
        glGetShaderInfoLog(fs, 1024, NULL, buffer);
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

		GLuint buf;

		//generate and bind buffer object
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);

		static const GLfloat vertex_positions[] =
		{
			-0.25f, 0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			
			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			
			-0.25f, 0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			0.25f, -0.25f, 0.25f,
			
			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,

			-0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, 0.25f,
			
			0.25f, -0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			-0.25f, -0.25f, -0.25f,
			
			-0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, 0.25f,
			
			0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, -0.25f,
		};

		//alloc memory for the buffer.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_DYNAMIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(0);

#define B 0x00, 0x00, 0x00, 0x00
#define W 0xFF, 0xFF, 0xFF, 0xFF
		static const GLubyte tex_data[] =
		{
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
			B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
			W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
		};
#undef B
#undef W
		
		glGenTextures(1, &tex_object[0]);
		glBindTexture(GL_TEXTURE_2D, tex_object[0]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 16, 16);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		
        // Generate a name for the texture
        glGenTextures(1, &tex_object[1]);

		unsigned int ret = sb6::ktx::file::load("../bin/media/textures/brick.ktx", tex_object[1]);
		if (0 == ret) {
			fprintf(stderr, "load texture failed.\n");
		}
	}

    void render(double currentTime)
    {
    	// Simply clear the window with red
    	static const GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, red);

        glUseProgram(render_prog);

		GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
							(float)cos(currentTime) * 0.6f,
							0.0f, 0.0f };
		// Update the value of input attribute 0
		//glVertexAttrib4fv(0, attrib);
		glVertexAttrib4fv(1, attrib);
		glPointSize(40.0f);

		float f = (float)currentTime * (float)M_PI * 0.1f;
		vmath::mat4 mv_matrix =
			vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(2.1f * f) * 0.5f,
			cosf(1.7f * f) * 0.5f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

		GLint mv_location = glGetUniformLocation(render_prog, "mv_matrix");
		GLint proj_location = glGetUniformLocation(render_prog, "proj_matrix");
		GLint ws_location = glGetUniformLocation(render_prog, "wsize");
		vmath::ivec2 wsize = vmath::ivec2(info.windowWidth, info.windowHeight);

		// Set the model-view and projection matrices
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
		glUniform2iv(ws_location, 1, wsize);

		glBindTexture(GL_TEXTURE_2D, tex_object[1]);

		glDrawArrays(GL_TRIANGLES, 0, 24);
	}

	void onResize(int w, int h)
	{
		sb6::application::onResize(w, h);
		aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f,
										aspect,
										0.1f,
										1000.0f);
	}
	
	private:
		void generate_texture(float * data, int width, int height)
		{
			int x, y;
	
			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
					data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
					data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
					data[(y * width + x) * 4 + 3] = 1.0f;
				}
			}
		}

	
	protected:
		GLuint			render_prog;
		GLuint			render_vao;
		GLfloat 		aspect;
		vmath::mat4 	proj_matrix;
		GLuint          tex_object[2];
};

DECLARE_MAIN(agao_app)

