#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <array>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int CreateShaderProgram(const char* vert, const char* frag)
{
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
		vShaderFile.open(vert);
		fShaderFile.open(frag);
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

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// print linking errors if any
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glLinkProgram(0);
	return shaderProgram;
}

unsigned int CreateTexture(const char* path) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

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
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // 1
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // 2
		 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   -4.0f,  2.0f,  0.0f,   0.0f, 1.0f, // 3
		 0.0f,  0.5f,  0.0f,   -4.0f,  2.0f,  0.0f,   0.5f, 0.0f,
		-0.5f, -0.5f,  0.5f,   -4.0f,  2.0f,  0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  2.0f,  4.0f,   0.0f, 1.0f, // 4
		 0.0f,  0.5f,  0.0f,    0.0f,  2.0f,  4.0f,   0.5f, 0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f,  2.0f,  4.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,    4.0f,  2.0f,  0.0f,   0.0f, 1.0f, // 5
		 0.0f,  0.5f,  0.0f,    4.0f,  2.0f,  0.0f,   0.5f, 0.0f,
		 0.5f, -0.5f, -0.5f,    4.0f,  2.0f,  0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    0.0f,  2.0f, -4.0f,   0.0f, 1.0f, // 6
		 0.0f,  0.5f,  0.0f,    0.0f,  2.0f, -4.0f,   0.5f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  2.0f, -4.0f,   1.0f, 1.0f
	};

	unsigned int VAO_pyramid;
	glGenVertexArrays(1, &VAO_pyramid);
	glBindVertexArray(VAO_pyramid);

	unsigned int VBO_pyramid;
	glGenBuffers(1, &VBO_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_data), pyramid_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// cube
	float cube_data[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  6.0f, 6.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 6.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  6.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  6.0f, 6.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 6.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  6.0f, 6.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  6.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  6.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 6.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  6.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 6.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 6.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  6.0f, 6.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  6.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  6.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  6.0f, 6.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  6.0f, 6.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 6.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  6.0f, 6.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 6.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  6.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  6.0f, 6.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  6.0f, 6.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 6.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  6.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  6.0f, 6.0f
	};

	unsigned int VAO_cube;
	glGenVertexArrays(1, &VAO_cube);
	glBindVertexArray(VAO_cube);

	unsigned int VBO_cube;
	glGenBuffers(1, &VBO_cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// light source
	unsigned int VAO_light;
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);

	unsigned int VBO_light;
	glGenBuffers(1, &VBO_light);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int SPLighting = CreateShaderProgram(
						RESOURCES_PATH"shaders/lighting.vert", RESOURCES_PATH"shaders/lighting.frag");
	unsigned int SPLightSource = CreateShaderProgram(
				RESOURCES_PATH"shaders/light_source.vert", RESOURCES_PATH"shaders/light_source.frag");

	unsigned int texture1 = CreateTexture(RESOURCES_PATH"textures/brick.png");
	unsigned int texture2 = CreateTexture(RESOURCES_PATH"textures/ore.png");

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
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO_pyramid);
		glUseProgram(SPLighting);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		glm::vec3 ambient_light(0.15f, 0.15f, 0.15f);
		glm::vec3 dir_light_col(0.4f, 0.4f, 0.4f);
		glm::vec3 dir_light_dir(-2.0f, 1.0f, -0.5f);

		std::array<glm::vec3, 3> sl_col = { glm::vec3(0.0f, 0.0f, 1.5f), 
											glm::vec3(0.0f, 1.5f, 0.0f),
											glm::vec3(1.5f, 0.0f, 0.0f) };
		std::array<glm::vec3, 3> sl_pos = { glm::vec3(1.0f, -2.0f, 1.5f),
											glm::vec3(2.73f, 1.0f, 1.5f),
											glm::vec3(-0.73f, 1.0f, 1.5f) };
		std::array<glm::vec3, 3> sl_spot_dir = { glm::vec3(0.0f, 2.0f, 1.5f),
												 glm::vec3(-1.73f, -1.0f, 1.5f),
												 glm::vec3(1.73f, -1.0f, 1.5f) };
		std::array<float, 3> sl_angle = { glm::cos(glm::radians(20.0)), glm::cos(glm::radians(20.0)), glm::cos(glm::radians(20.0)) };
		std::array<float, 3> sl_outer_angle = { glm::cos(glm::radians(25.0)), glm::cos(glm::radians(25.0)), glm::cos(glm::radians(25.0)) };

		std::array<glm::vec3, 3> pl_col = { glm::vec3(1.0f, 1.0f, 1.0f),
											glm::vec3(1.0f, 0.0f, 0.7f),
											glm::vec3(0.3f, 1.0f, 0.0f) };
		std::array<glm::vec3, 3> pl_pos = { glm::vec3(0.7f, 1.0f, -3.2f),
											glm::vec3(1.0f, 4.0f, 5.0f),
											glm::vec3(0.0f, 4.0f, 6.0f) };

		glUniform3f(glGetUniformLocation(SPLighting, "AmbientLight"), ambient_light.x, ambient_light.y, ambient_light.z);
		glUniform3f(glGetUniformLocation(SPLighting, "dl.col"), dir_light_col.x, dir_light_col.y, dir_light_col.z);
		glUniform3f(glGetUniformLocation(SPLighting, "dl.dir"), dir_light_dir.x, dir_light_dir.y, dir_light_dir.z);
		for (int i = 0; i < 3; i++) {
			std::string tmp = "sl[" + std::to_string(i) + "].col";
			glUniform3f(glGetUniformLocation(SPLighting, tmp.c_str()), sl_col[i].x, sl_col[i].y, sl_col[i].z);
			tmp = "sl[" + std::to_string(i) + "].pos";
			glUniform3f(glGetUniformLocation(SPLighting, tmp.c_str()), sl_pos[i].x, sl_pos[i].y, sl_pos[i].z);
			tmp = "sl[" + std::to_string(i) + "].spotDir";
			glUniform3f(glGetUniformLocation(SPLighting, tmp.c_str()), sl_spot_dir[i].x, sl_spot_dir[i].y, sl_spot_dir[i].z);
			tmp = "sl[" + std::to_string(i) + "].angle";
			glUniform1f(glGetUniformLocation(SPLighting, tmp.c_str()), sl_angle[i]);
			tmp = "sl[" + std::to_string(i) + "].outerAngle";
			glUniform1f(glGetUniformLocation(SPLighting, tmp.c_str()), sl_outer_angle[i]);

			tmp = "pl[" + std::to_string(i) + "].col";
			glUniform3f(glGetUniformLocation(SPLighting, tmp.c_str()), pl_col[i].x, pl_col[i].y, pl_col[i].z);
			tmp = "pl[" + std::to_string(i) + "].pos";
			glUniform3f(glGetUniformLocation(SPLighting, tmp.c_str()), pl_pos[i].x, pl_pos[i].y, pl_pos[i].z);
		}
		glUniform3f(glGetUniformLocation(SPLighting, "CameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(glGetUniformLocation(SPLighting, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SPLighting, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f));
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
		glUniformMatrix4fv(glGetUniformLocation(SPLighting, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 18);

		// draw cube
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO_cube);
		glUseProgram(0);
		glUseProgram(SPLighting);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 6.0f));
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(6.0f));

		glUniformMatrix4fv(glGetUniformLocation(SPLighting, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw light source
		glUseProgram(SPLightSource);

		model = glm::translate(glm::mat4(1.0f), sl_pos[0]);
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glUniformMatrix4fv(glGetUniformLocation(SPLightSource, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SPLightSource, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

		for (int i = 0; i < 3; i++) {
			model = glm::translate(glm::mat4(1.0f), sl_pos[i]);
			model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			glUniform3f(glGetUniformLocation(SPLightSource, "lightColor"), sl_col[i].x, sl_col[i].y, sl_col[i].z);
			glUniformMatrix4fv(glGetUniformLocation(SPLightSource, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		for (int i = 0; i < 3; i++) {
			model = glm::translate(glm::mat4(1.0f), pl_pos[i]);
			model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			glUniform3f(glGetUniformLocation(SPLightSource, "lightColor"), pl_col[i].x, pl_col[i].y, pl_col[i].z);
			glUniformMatrix4fv(glGetUniformLocation(SPLightSource, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_pyramid);
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_pyramid);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteProgram(SPLighting);
	glDeleteProgram(SPLightSource);

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