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
			"layout (location = 0) in vec4 offset; 									\n"
			"layout (location = 1) in vec4 color;  									\n"
			"out vec4 vs_color; 														\n"
			"void main(void)														\n"
			"{																		\n"
			"     const vec4 vertices[3] = vec4[3](vec4(0.0, 0.0, 0.5, 1.0), 		\n" 
			"     									vec4(-0.5, 0.5, 0.5, 1.0),	\n"                                  
			"									     vec4(1.0, 1.0, 0.5, 1.0));	\n"
			" int flag = (gl_VertexID == 1) ? -1 : 1;							 \n"
			//"	 gl_Position = vec4(0.0 + gl_VertexID/2.0*flag, 0.0 + gl_VertexID/2.0, 0.5, 1.0);								\n"
			"	 gl_Position = vertices[gl_VertexID] + offset;								\n"
			"	vs_color = color;													\n"
			"}																		\n"
		};
		
        static const char * fs_source[] =
        {
            "#version 330 core                                                      \n"
            "                                                                       \n"
            "in vec4 vs_color;														\n"
            "out vec4 color;                                 						\n"
            "                                                                       \n"
            "void main(void)                                                        \n"
            "{                                                                      \n"
            "    color = vs_color;					   								\n"
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

        glGenVertexArrays(1, &render_vao);
        glBindVertexArray(render_vao);
	}

    void render(double currentTime)
    {
    	// Simply clear the window with red
    	static const GLfloat red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, red);

        glUseProgram(render_prog);

		GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
							(float)cos(currentTime) * 0.6f,
							0.0f, 0.0f };
		// Update the value of input attribute 0
		glVertexAttrib4fv(0, attrib);
		glVertexAttrib4fv(1, attrib);
		glPointSize(40.0f);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	
	protected:
		GLuint			render_prog;
		GLuint			render_vao;
};

DECLARE_MAIN(agao_app)

