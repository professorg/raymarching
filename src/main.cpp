#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <vector>
#include <ios>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#define Point   0
#define Sphere  1
#define AACube  2

#define WIDTH   1024
#define HEIGHT  768

#define ROTATION    20.0f

struct Object
{
     GLuint type;
     glm::vec3 position;
     GLfloat size;
};

struct Camera
{
    glm::vec3 position;
    glm::vec3 direction;
};

static glm::vec2 resolution = { WIDTH, HEIGHT };

static const Object objects[] = {
    { Point,    glm::vec3( 0.0f,  1.0f,  4.0f),   },
    { Sphere,   glm::vec3( 0.0f,  0.0f,  4.0f), 1 },
    { Sphere,   glm::vec3(-1.0f,  0.0f,  2.0f), 1 },
    { AACube,   glm::vec3( 2.0f,  2.0f,  4.0f), 1 },
    { Sphere,   glm::vec3( 0.0f,  1.0f,  8.0f), 2 },
    { Sphere,   glm::vec3( 0.0f,  2.0f,  8.0f), 2 },
};

static Camera camera = { glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f) };

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,

    -1.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
};

static int frame = 0;

void update(GLFWwindow *window, GLuint vertexbuffer, GLuint programID);
void send_uniforms(GLuint programID);
void draw(GLFWwindow *window, GLuint vertexbuffer, GLuint programID);
GLuint InitOpenGL(GLFWwindow * &window, GLuint &vertexbuffer);
int LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
void on_scroll(GLFWwindow* window, double xoffset, double yoffset);
void on_key(GLFWwindow *window, int key, int scancode, int action, int mods);

int main()
{

    GLFWwindow *window;
    GLuint vertexbuffer;
    GLuint programID = InitOpenGL(window, vertexbuffer);
    // Main loop
    do
    {
        update(window, vertexbuffer, programID);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;
}

void update(GLFWwindow *window, GLuint vertexbuffer, GLuint programID)
{
    ++frame;
    send_uniforms(programID);
    draw(window, vertexbuffer, programID);
}

void send_uniforms(GLuint programID)
{
    GLuint loc;
    char base[20];
    char buffer[30];

    if ((frame %= 100) == 0)
    {
        std::cout << "Position: " << glm::to_string(camera.position) << std::endl
             << "Direction: " << glm::to_string(camera.direction) << std::endl;
    }
    
    loc = glGetUniformLocation(programID, "resolution");
    glUniform2f(loc, WIDTH, HEIGHT);

    loc = glGetUniformLocation(programID, "camera.position");
    glUniform3f(loc, camera.position.x, camera.position.y, camera.position.z);

    loc = glGetUniformLocation(programID, "camera.direction");
    glUniform3f(loc, camera.direction.x, camera.direction.y, camera.direction.z);
    
    int size = sizeof(objects) / sizeof(Object);
    loc = glGetUniformLocation(programID, "num_objects");
    glUniform1i(loc, size);

    for (int i = 0; i < size; ++i)
    {
        sprintf(base, "objects[%d]", i);

        sprintf(buffer, "%s.type", base);
        loc = glGetUniformLocation(programID, buffer);
        glUniform1i(loc, objects[i].type);

        sprintf(buffer, "%s.position", base);
        loc = glGetUniformLocation(programID, buffer);
        glm::vec3 pos = objects[i].position;
        glUniform3f(loc, pos.x, pos.y, pos.z);

        sprintf(buffer, "%s.size", base);
        loc = glGetUniformLocation(programID, buffer);
        glUniform1f(loc, objects[i].size);
    }
}

void draw(GLFWwindow *window, GLuint vertexbuffer, GLuint programID)
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        glUseProgram(programID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
}


// Returns programID
GLuint InitOpenGL(GLFWwindow * &window, GLuint &vertexbuffer)
{
    // Init GLFW
    glewExperimental = GL_TRUE; // For core profile
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(-1);
    }
    // Create OpenGL window
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "Creating a window", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(-1);
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetScrollCallback(window, on_scroll);
    glfwSetKeyCallback(window, on_key);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    return LoadShaders("shader.vert", "shader.frag");
}

// LoadShaders from opengl-tutorial.org
int LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void on_scroll(GLFWwindow *window, double xoffset, double yoffset)
{
    // On scroll
}

void on_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        glm::vec3 axis(0.0f, 1.0f, 0.0f);
        glm::mat4 trans = glm::mat4(1.0f);
        switch (key)
        {
            case GLFW_KEY_W:
                camera.position += camera.direction;
                break;
            case GLFW_KEY_A:
                camera.position += glm::cross(camera.direction, glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            case GLFW_KEY_S:
                camera.position -= camera.direction;
                break;
            case GLFW_KEY_D:
                camera.position -= glm::cross(camera.direction, glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            case GLFW_KEY_Q:
                trans = glm::rotate(trans, glm::radians(-ROTATION), axis);
                camera.direction = trans * glm::vec4(camera.direction, 1.0f);
                break;
            case GLFW_KEY_E:
                trans = glm::rotate(trans, glm::radians(ROTATION), axis);
                camera.direction = trans * glm::vec4(camera.direction, 1.0f);
                break;
        }
    }
}
