#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include "Camera.h"
#include "QuadTree.h"


using namespace std;


int importOBJ(const char* filename, vector<float> &verti, int &count);

void keyStatus(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouseStatus(GLFWwindow *window, double xPos, double yPos);
void move();

const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;
int ScreenWidth, ScreenHeight;

Camera camera(glm::vec3(0.0f, 5.0f, 5.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

glm::vec3 lightPos(5.0f, 20.0f, -5.0f);
glm::vec3 cubePos(0.0f, 0.0f, 0.0f);
glm::vec3 spherePos(5.0f, 0.0f, 0.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	srand(time(NULL));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Game", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}


	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &ScreenWidth, &ScreenHeight);
	glfwSetKeyCallback(window, keyStatus);
	glfwSetCursorPosCallback(window, mouseStatus);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, ScreenWidth, ScreenHeight);
	glEnable(GL_DEPTH_TEST);

	Shader lightingShader("lighting.vs", "lighting.frag");
	Shader lampShader("lamp.vs", "lamp.frag");

	int count = 0, count2=0;
	vector<GLfloat> verticles, verticles2;
	importOBJ("kostkat.obj", verticles, count);
	GLuint VBO, boxVAO, lightVAO, cVAO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticles.size() * sizeof(GLfloat), &verticles.front(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		
		importOBJ("kula.obj", verticles2, count2);
		glGenVertexArrays(1, &cVAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(cVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verticles2.size() * sizeof(GLfloat), &verticles2.front(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		

	// Load and create a texture
	GLuint texture;

	int width, height;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char *image = SOIL_load_image("tex.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)ScreenWidth / (GLfloat)ScreenHeight, 0.1f, 1000.0f);
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		cubePos.x += 0.001f;


		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		move();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		lightingShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "Texture1"), 0);

		//GLint objectColorLoc = glGetUniformLocation(lightingShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(lightingShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		//glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		glm::mat4 view;
		view = camera.getViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(boxVAO);

		glm::mat4 model;
		glm::mat4 trM = glm::translate(model, cubePos);
		glm::mat4 roM = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 1.0f));
		model =  trM;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, count);
		glBindVertexArray(0);
		
		glBindVertexArray(cVAO);

		model = glm::mat4();
		model *= trM *roM * glm::translate(model, spherePos) * glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, count2);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &cVAO);
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return EXIT_SUCCESS;
}
void move()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.processKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.processKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.processKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.processKeyboard(RIGHT, deltaTime);
	}
}
void keyStatus(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}
void mouseStatus(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.processMouseMovement(xOffset, yOffset);
}

int importOBJ(const char* filename, vector<GLfloat> &verti, int &count) {
	std::ifstream in(filename, ios::in);
	if (!in)
	{
		cout << "Cannot open " << filename << endl;
		exit(1);
	}
	int mode = 0;
	vector<glm::vec3> ver;
	vector<glm::vec3> nor;
	vector<glm::vec2> tex;
	int w1, w2, w12, w22, w13, w23;
	string f1, f2, f3, q1, q2, q12, q22, q13, q23;
	string line;
	while (getline(in, line))
	{

		if (line.substr(0, 2) == "v ")
		{
			stringstream s(line.substr(2));
			glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
			ver.push_back(v);
		}
		else if (line.substr(0, 2) == "vn")
		{
			stringstream s(line.substr(3));
			glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
			nor.push_back(v);
		}
		else if (line.substr(0, 2) == "vt")
		{
			stringstream s(line.substr(3));
			glm::vec2 v; s >> v.x; s >> v.y;
			tex.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			count += 3;
			stringstream s(line.substr(2));
			s >> f1; s >> f2; s >> f3;
			w1 = f1.find_first_of('/', 0);
			w12 = f2.find_first_of('/', 0);
			w13 = f3.find_first_of('/', 0);
			if (w1 == -1) {
				mode = 1;
				verti.push_back(ver[stoi(f1) - 1].x);
				verti.push_back(ver[stoi(f1) - 1].y);
				verti.push_back(ver[stoi(f1) - 1].z);

				verti.push_back(ver[stoi(f2) - 1].x);
				verti.push_back(ver[stoi(f2) - 1].y);
				verti.push_back(ver[stoi(f2) - 1].z);

				verti.push_back(ver[stoi(f3) - 1].x);
				verti.push_back(ver[stoi(f3) - 1].y);
				verti.push_back(ver[stoi(f3) - 1].z);
			}
			else {
				q1 = f1.substr(0, w1);
				q12 = f2.substr(0, w12);
				q13 = f3.substr(0, w13);
				f1 = f1.substr(w1 + 1);
				f2 = f2.substr(w12 + 1);
				f3 = f3.substr(w13 + 1);
				w2 = f1.find_first_of('/', 0);
				w22 = f2.find_first_of('/', 0);
				w23 = f3.find_first_of('/', 0);
				if (w2 == -1) {
					mode = 2;
					q2 = f1;
					q22 = f2;
					q23 = f3;
					verti.push_back(ver[stoi(q1) - 1].x);
					verti.push_back(ver[stoi(q1) - 1].y);
					verti.push_back(ver[stoi(q1) - 1].z);
					verti.push_back(tex[stoi(q2) - 1].x);
					verti.push_back(tex[stoi(q2) - 1].y);

					verti.push_back(ver[stoi(q12) - 1].x);
					verti.push_back(ver[stoi(q12) - 1].y);
					verti.push_back(ver[stoi(q12) - 1].z);
					verti.push_back(tex[stoi(q22) - 1].x);
					verti.push_back(tex[stoi(q22) - 1].y);

					verti.push_back(ver[stoi(q13) - 1].x);
					verti.push_back(ver[stoi(q13) - 1].y);
					verti.push_back(ver[stoi(q13) - 1].z);
					verti.push_back(tex[stoi(q23) - 1].x);
					verti.push_back(tex[stoi(q23) - 1].y);

				}
				else {
					if (f1[0] != '/') {
						mode = 3;
						q2 = f1.substr(0, w2);
						q22 = f2.substr(0, w22);
						q23 = f3.substr(0, w23);
						f1 = f1.substr(w2 + 1);
						f2 = f2.substr(w22 + 1);
						f3 = f3.substr(w23 + 1);

						verti.push_back(ver[stoi(q1) - 1].x);
						verti.push_back(ver[stoi(q1) - 1].y);
						verti.push_back(ver[stoi(q1) - 1].z);
						verti.push_back(tex[stoi(q2) - 1].x);
						verti.push_back(tex[stoi(q2) - 1].y);
						verti.push_back(nor[stoi(f1) - 1].x);
						verti.push_back(nor[stoi(f1) - 1].y);
						verti.push_back(nor[stoi(f1) - 1].z);

						verti.push_back(ver[stoi(q12) - 1].x);
						verti.push_back(ver[stoi(q12) - 1].y);
						verti.push_back(ver[stoi(q12) - 1].z);
						verti.push_back(tex[stoi(q22) - 1].x);
						verti.push_back(tex[stoi(q22) - 1].y);
						verti.push_back(nor[stoi(f2) - 1].x);
						verti.push_back(nor[stoi(f2) - 1].y);
						verti.push_back(nor[stoi(f2) - 1].z);

						verti.push_back(ver[stoi(q13) - 1].x);
						verti.push_back(ver[stoi(q13) - 1].y);
						verti.push_back(ver[stoi(q13) - 1].z);
						verti.push_back(tex[stoi(q23) - 1].x);
						verti.push_back(tex[stoi(q23) - 1].y);
						verti.push_back(nor[stoi(f3) - 1].x);
						verti.push_back(nor[stoi(f3) - 1].y);
						verti.push_back(nor[stoi(f3) - 1].z);
					}
					else {
						mode = 4;
						f1 = f1.substr(1);
						f2 = f2.substr(1);
						f3 = f3.substr(1);

						verti.push_back(ver[stoi(q1) - 1].x);
						verti.push_back(ver[stoi(q1) - 1].y);
						verti.push_back(ver[stoi(q1) - 1].z);
						verti.push_back(nor[stoi(f1) - 1].x);
						verti.push_back(nor[stoi(f1) - 1].y);
						verti.push_back(nor[stoi(f1) - 1].z);

						verti.push_back(ver[stoi(q12) - 1].x);
						verti.push_back(ver[stoi(q12) - 1].y);
						verti.push_back(ver[stoi(q12) - 1].z);
						verti.push_back(nor[stoi(f2) - 1].x);
						verti.push_back(nor[stoi(f2) - 1].y);
						verti.push_back(nor[stoi(f2) - 1].z);

						verti.push_back(ver[stoi(q13) - 1].x);
						verti.push_back(ver[stoi(q13) - 1].y);
						verti.push_back(ver[stoi(q13) - 1].z);
						verti.push_back(nor[stoi(f3) - 1].x);
						verti.push_back(nor[stoi(f3) - 1].y);
						verti.push_back(nor[stoi(f3) - 1].z);
					}
				}
			}
		}

	}
	return mode;
}