// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <random>

#define min(a,b) (a < b) ? a : b
#define max(a,b) (a > b) ? a : b
#define smooth(a) ((3 - 2 * a) * a * a)
#define clamp(a) max(0, (min(a, 1)))

int main(void)
{
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(600, 600, "Perlin Noise Generation", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    float points[] = {
        -0.8f, -0.8f,  0.0f,
        -0.8f,  0.8f,  0.0f,
         0.8f, -0.8f,  0.0f,
		-0.8f,  0.8f,  0.0f,
		 0.8f,  0.8f,  0.0f,
		 0.8f, -0.8f,  0.0f
    };

	float uv[] = {
		0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint ubo = 0;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_ARRAY_BUFFER, ubo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), uv, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ubo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);  
	srand(66);

	#define SIZE 512
	#define GSIZE 8
	#define SAFE_GSIZE GSIZE + 1
	float gradient[SAFE_GSIZE][SAFE_GSIZE][2];
	uint gsize = SAFE_GSIZE;

	float g0, g1, gmag;
	for(size_t i = 0; i < gsize; i++)
	{
		memset(gradient[i], 0, gsize * sizeof(float));
		for(size_t j = 0; j < gsize; j++)
		{
			g0 = ((rand() / double(RAND_MAX)) * 2 - 1);
			g1 = ((rand() / double(RAND_MAX)) * 2 - 1);
			gmag = sqrt(g0*g0 + g1*g1);
			gradient[i][j][0] = g0 / gmag;
			gradient[i][j][1] = g1 / gmag;
		}
	}

	float noise[SIZE][SIZE][3];
	int x0, x1, y0, y1;
	float x, y, n0, n1, u, v, value;
	
	uint size = SIZE;
	gsize = GSIZE;
	uint csize = size / gsize;
	float step = GSIZE / (float) SIZE;

	for(size_t i = 0; i < size; i++)
	{
		memset(noise[i], 0, size*sizeof(float));
		for(size_t j = 0; j < size; j++)
		{
			x = i * step;
			y = j * step;

			x0 = (int) x;
			x1 = x0 + 1;

			y0 = (int) y;
			y1 = y0 + 1;

			float sx = x - (float)x0;
			float sy = y - (float)y0;

			sx = smooth(sx);
			sy = smooth(sy);

			n0 = (x - (float)x0)*gradient[x0][y0][0] + (y - (float)y0)*gradient[x0][y0][1];
			n1 = (x - (float)x1)*gradient[x1][y0][0] + (y - (float)y0)*gradient[x1][y0][1];
			u = n0 + sx * (n1 - n0);

			n0 = (x - (float)x0)*gradient[x0][y1][0] + (y - (float)y1)*gradient[x0][y1][1];
			n1 = (x - (float)x1)*gradient[x1][y1][0] + (y - (float)y1)*gradient[x1][y1][1];
			v = n0 + sx * (n1 - n0);
			value = u + sy * (v - u);
			value = clamp(((value * 2.0f) + 1) / 2.0f);
			noise[i][j][0] = value;
			noise[i][j][1] = value;
			noise[i][j][2] = value;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_FLOAT, noise);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    const char* vertex_shader =
    "#version 400\n"
	"layout (location = 0) in vec3 vp; "
	"layout (location = 1) in vec2 uv; " 
	"out vec2 vv;"
    "void main() {"
    "  gl_Position = vec4(vp, 1.0);"
	"  vv = uv;"
    "}";

    const char* fragment_shader =
    "#version 400\n"
	"in vec2 vv;"
    "out vec4 frag_colour;"
	"uniform sampler2D ourTexture;"
    "void main() {"
	"  vec4 texx = texture(ourTexture, vv);"
    "  frag_colour = vec4(texx.r, texx.g, texx.b, 1.0);"
    "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
	GLint success = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
		int maxLength = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> errorLog(maxLength);
        glGetShaderInfoLog(vs, maxLength, NULL, &errorLog[0]);
        std::string errorMessage(begin(errorLog), end(errorLog));
		printf("Error: %s", errorMessage.c_str());
        glDeleteShader(vs);
	} else {
		printf("Shader compiled with success.");
	}

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if(success == GL_FALSE) {
		GLint maxLength = 0;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fs, maxLength, NULL, &errorLog[0]);
        std::string errorMessage(begin(errorLog), end(errorLog));
		printf("Error: %s", errorMessage.c_str());
        glDeleteShader(fs);
	} else {
		printf("Shader compiled with success.");
	}

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);
	glUseProgram(shader_programme);
	
	glBindVertexArray(vao);
	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}