#include <iostream>
#include <vector>
#include <SDL2\SDL.h>
#include <SOIL.h>
#undef main
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertSimple =
"#version 330 core\n"
"layout (location = 0) in vec3 position;"
"layout (location = 1) in vec2 texCoord;"
"out vec2 texOut;"
"void main(){"
"texOut = texCoord;"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);"
"}";

const char* fragSimple =
"#version 330 core\n"
"out vec4 color;"
"in vec2 texOut;"
"uniform sampler2DArray ourTexture;"
"uniform int layerIndex;"
"void main(){"
"	color = texture(ourTexture,vec3(texOut, layerIndex));"
"\n}";

const char* vert =
"#version 330 core\n"
"layout (location = 0) in vec3 position;"
"layout (location = 1) in vec4 colorDst;"
"layout (location = 2) in vec2 texCoord;"
"uniform mat4 transform;"
"out vec2 texOut;"
"out vec4 colorOut;"
"void main(){"
"texOut = texCoord;"
"colorOut = colorDst;"
"gl_Position = transform * vec4(position.x, position.y, position.z, 1.0);"
"}";

const char* frag =
"#version 330 core\n"
"out vec4 color;"
"in vec2 texOut;"
"in vec4 colorOut;"
"uniform sampler2D ourTexture;"
"void main(){"
"	color = texture(ourTexture,texOut)*colorOut;"
"}";

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window *m_window = SDL_CreateWindow("window", 100, 100, 200, 200, SDL_WINDOW_OPENGL);
	SDL_GLContext m_GLcontext = SDL_GL_CreateContext(m_window);

	SDL_Window *m_window2 = SDL_CreateWindow("window2", 400, 100, 200, 200, SDL_WINDOW_OPENGL);
	SDL_GLContext m_GLcontext2 = SDL_GL_CreateContext(m_window2);
	SDL_GL_MakeCurrent(m_window, m_GLcontext);
	SDL_Event e;
	bool is_Close = false;
	//OPENGL PART
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("glew doesn't work");
		return -1;
	}

	//SHADER PART##################################################################################
	GLuint ProgramID = glCreateProgram();
	GLuint vID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vID, 1, &vert, NULL);
	glShaderSource(fID, 1, &frag, NULL);

	glCompileShader(vID);
	glCompileShader(fID);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glAttachShader(ProgramID, vID);
	glAttachShader(ProgramID, fID);

	glLinkProgram(ProgramID);

	glUseProgram(ProgramID);
	//SHADER TO PRINT THE TEXTURE
	GLuint ProgramIDsimple = glCreateProgram();
	GLuint vIDsimple = glCreateShader(GL_VERTEX_SHADER);
	GLuint fIDsimple = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vIDsimple, 1, &vertSimple, NULL);
	glShaderSource(fIDsimple, 1, &fragSimple, NULL);

	glCompileShader(vIDsimple);
	glCompileShader(fIDsimple);

	//GLint success;
	//GLchar infoLog[512];
	glGetShaderiv(vIDsimple, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vIDsimple, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fIDsimple, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fIDsimple, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glAttachShader(ProgramIDsimple, vIDsimple);
	glAttachShader(ProgramIDsimple, fIDsimple);

	glLinkProgram(ProgramIDsimple);
	//##############################################################################Program suplies
	GLuint PositionID;
	GLuint ColorID;
	GLuint texCoord;
	PositionID = glGetAttribLocation(ProgramID, "position");
	ColorID = glGetAttribLocation(ProgramID, "colorDst");
	texCoord = glGetAttribLocation(ProgramID, "texCoord");
	std::cout << ColorID << std::endl;
	std::cout << PositionID << std::endl;
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	struct vec2 {
		vec2(float _r = 1, float _g = 1) :r(_r), g(_g) {}
		float r, g;
	};
	struct vec3 {
		vec3(float _r = 1, float _g = 1, float _b = 1) :r(_r), g(_g), b(_b) {}
		float r, g, b;
	};
	struct vec4 {
		vec4(float _r = 1, float _g = 1, float _b = 1, float _a = 1) :r(_r), g(_g), b(_b), a(_a) {}
		float r, g, b, a;
	};
	struct Vertex {
		vec3 position;
		vec4 color;
		vec2 texture;
	};
	std::vector<Vertex> triangle;
	Vertex v1 = { vec3(-1.0f, -1.0f, 0.0f),vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) };
	Vertex v2 = { vec3(1.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.5f, 1.0f) };
	Vertex v3 = { vec3(1.0f, 1.0f, .0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) };
	triangle.push_back(v1);
	triangle.push_back(v2);
	triangle.push_back(v3);
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(Vertex), &triangle[0], GL_STATIC_DRAW);
	
	int width, height;
	unsigned char* image = SOIL_load_image("tex1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//transformations
	glm::mat4 transform;
	glm::mat4 transform2;
	transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
	transform2 = glm::rotate(transform2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	GLuint transfID = glGetUniformLocation(ProgramID, "transform");
	glUniformMatrix4fv(transfID, 1, GL_FALSE, glm::value_ptr(transform));

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(PositionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(ColorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3)+ sizeof(vec4)));
	glBindVertexArray(VertexArrayID);

	//FRAMEBUFFER
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	GLuint textureFBO;
	glGenTextures(1, &textureFBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureFBO);
	//glTexImage2D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, 200, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 2, GL_RGBA8, 200, 200, 2);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	//#########################################THE PLACE WHERE MAGIC HAPPENS
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureFBO, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureFBO, 0, 1);
	glUniformMatrix4fv(transfID, 1, GL_FALSE, glm::value_ptr(transform2));
	glBindTexture(GL_TEXTURE_2D, texture);
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//GLubyte data[3 * 200 * 200];
	//#########################################USE THE TEXTURE RENDERED
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureFBO);
	glUseProgram(ProgramIDsimple);

	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));	
	
	//#########################################WINDOW LOOP
	
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(g_quad_vertex_buffer_data[0]), 0);

	GLenum glCheck = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (glCheck != GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer not complete %x\n", glCheck);
	}
	GLuint layerIndex = glGetUniformLocation(ProgramIDsimple, "layerIndex");
	while (!is_Close) {
		SDL_GL_MakeCurrent(m_window, m_GLcontext);
		SDL_GL_SwapWindow(m_window);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureFBO);
		glUniform1i(layerIndex, 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				is_Close = true;
		}
		SDL_GL_MakeCurrent(m_window2, m_GLcontext);
		SDL_GL_SwapWindow(m_window2);
		
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureFBO);
		glUniform1i(layerIndex, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(0);
		//glReadPixels(0, 0, 200, 200, GL_RGB, GL_UNSIGNED_BYTE, data);

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				is_Close = true;
		}
	}
	/*for (int i = 0; i < 3 * 200 * 200; i++) {
		printf("%i ", data[i]);
	}*/
	SDL_GL_DeleteContext(m_GLcontext);
	SDL_DestroyWindow(m_window);
	SDL_DestroyWindow(m_window2);
	SDL_Quit();
	getchar();
	return 0;
}