#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera things
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = true;
float pitch = 0;
float yaw = -90;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mimocake's OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 150, 150);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// pyramid
	float pyramid_data[] = {
		-0.5f, -0.5f, -0.5f, // 1
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f, // 2
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f, // 3
		 0.0f,  0.5f,  0.0f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f, // 4
		 0.0f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f, // 5
		 0.0f,  0.5f,  0.0f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f, // 6
		 0.0f,  0.5f,  0.0f,
		-0.5f, -0.5f, -0.5f
	};

	unsigned int VAO_pyramid;
	glGenVertexArrays(1, &VAO_pyramid);
	glBindVertexArray(VAO_pyramid);

	unsigned int VBO_pyramid;
	glGenBuffers(1, &VBO_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_data), pyramid_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// cube
	float cube_data[] = {
	    -0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f,  0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	    -0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f
	};

	unsigned int VAO_cube;
	glGenVertexArrays(1, &VAO_cube);
	glBindVertexArray(VAO_cube);

	unsigned int VBO_cube;
	glGenBuffers(1, &VBO_cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// light source
	unsigned int VAO_light;
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);

	unsigned int VBO_light;
	glGenBuffers(1, &VBO_light);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// lighting shader
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(RESOURCES_PATH"shaders/lighting.vert");
		fShaderFile.open(RESOURCES_PATH"shaders/lighting.frag");
		std::stringstream vShaderStream, fShaderStream;
		// read file’s buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	
	// create vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	int success;
	char infoLog[512];
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	// print compile errors if any
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// create fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	// print compile errors if any
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	unsigned int shaderProgramLighting = glCreateProgram();
	glAttachShader(shaderProgramLighting, vertexShader);
	glAttachShader(shaderProgramLighting, fragmentShader);
	glLinkProgram(shaderProgramLighting);
	// print linking errors if any
	glGetProgramiv(shaderProgramLighting, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramLighting, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// light source shader
	std::string vertexCode2;
	std::string fragmentCode2;
	std::ifstream vShaderFile2;
	std::ifstream fShaderFile2;
	try
	{
		// open files
		vShaderFile2.open(RESOURCES_PATH"shaders/light_source.vert");
		fShaderFile2.open(RESOURCES_PATH"shaders/light_source.frag");
		std::stringstream vShaderStream2, fShaderStream2;
		// read file’s buffer contents into streams
		vShaderStream2 << vShaderFile2.rdbuf();
		fShaderStream2 << fShaderFile2.rdbuf();
		// close file handlers
		vShaderFile2.close();
		fShaderFile2.close();
		// convert stream into string
		vertexCode2 = vShaderStream2.str();
		fragmentCode2 = fShaderStream2.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode2 = vertexCode2.c_str();
	const char* fShaderCode2 = fragmentCode2.c_str();

	// create vertex shader
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vShaderCode2, NULL);
	glCompileShader(vertexShader2);
	// print compile errors if any
	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// create fragment shader
	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fShaderCode2, NULL);
	glCompileShader(fragmentShader2);
	// print compile errors if any
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	unsigned int shaderProgramLightSource = glCreateProgram();
	glAttachShader(shaderProgramLightSource, vertexShader2);
	glAttachShader(shaderProgramLightSource, fragmentShader2);
	glLinkProgram(shaderProgramLightSource);
	// print linking errors if any
	glGetProgramiv(shaderProgramLightSource, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramLightSource, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	while (!glfwWindowShouldClose(window))
	{
		// input
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame; // time between frames
		lastFrame = currentFrame;

		const float cameraSpeed = 3 * deltaTime; 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
			cameraPos += cameraSpeed * cameraFront; // forward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront; // back
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // left
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // right
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // up
			cameraPos += cameraUp * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
			glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) // down
			cameraPos -= cameraUp * cameraSpeed;

		// clear
		glClearColor(42.0f/255, 42.0f/255, 53.0f/255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw pyramid
		glBindVertexArray(VAO_pyramid);
		glUseProgram(shaderProgramLighting);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		glm::vec3 col(1.0, 1.0, 0.0);
		int color_uni = glGetUniformLocation(shaderProgramLighting, "Color");
		glUniform3f(color_uni, col.x, col.y, col.z);
		int uni_model = glGetUniformLocation(shaderProgramLighting, "model");
		int uni_view = glGetUniformLocation(shaderProgramLighting, "view");
		int uni_proj = glGetUniformLocation(shaderProgramLighting, "proj");
		glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(proj));

		glDrawArrays(GL_TRIANGLES, 0, 18);

		// draw cube
		glBindVertexArray(VAO_cube);
		glUseProgram(shaderProgramLighting);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 2.0f));

		col = glm::vec3(1.0, 0.0, 0.0);
		color_uni = glGetUniformLocation(shaderProgramLighting, "Color");
		glUniform3f(color_uni, col.x, col.y, col.z);
		uni_model = glGetUniformLocation(shaderProgramLighting, "model");
		uni_view = glGetUniformLocation(shaderProgramLighting, "view");
		uni_proj = glGetUniformLocation(shaderProgramLighting, "proj");
		glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(proj));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw light source
		glUseProgram(shaderProgramLightSource);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f));
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		col = glm::vec3(1.0, 1.0, 1.0);
		color_uni = glGetUniformLocation(shaderProgramLightSource, "lightColor");
		glUniform3f(color_uni, col.x, col.y, col.z);
		uni_model = glGetUniformLocation(shaderProgramLightSource, "model");
		uni_view = glGetUniformLocation(shaderProgramLightSource, "view");
		uni_proj = glGetUniformLocation(shaderProgramLightSource, "proj");
		glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(proj));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_pyramid);
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_pyramid);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteProgram(shaderProgramLighting);
	glDeleteProgram(shaderProgramLightSource);

	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = xposIn;
	float ypos = yposIn;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}