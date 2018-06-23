#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <Windows.h>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

//tekstury
GLuint programColor;
GLuint programTexture;
GLuint sandTexture;
GLuint rockTexture;
GLuint fishTexture;
GLuint dolphinTexture;
GLuint textureEarth;
GLuint textureAsteroid;
GLuint goldfishTexture;

Core::Shader_Loader shaderLoader;

float appLoadingTime;
//modele
obj::Model goldfishModel;
obj::Model shipModel;
obj::Model sphereModel;
obj::Model floorModel;
obj::Model rockModel;
obj::Model fishModel;
obj::Model dolphinModel;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 2, 0); 
glm::vec3 cameraDir;


glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));


static const int NUM_ASTEROIDS = 100;
glm::vec3 asteroidPositions[NUM_ASTEROIDS];
static const int NUM_DOLPHINS = 5;
glm::vec3 dolphinPositions[NUM_DOLPHINS];
static const int NUM_CAMERA_POINTS = 10;
glm::vec3 cameraKeyPoints[NUM_CAMERA_POINTS];

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.5f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'o': cameraPos += glm::cross(cameraDir, glm::vec3(0,0,1)) * moveSpeed; break;
	case 'l': cameraPos -= glm::cross(cameraDir, glm::vec3(0,0,1)) * moveSpeed; break;

	}
}

glm::mat4 createCameraMatrix()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;
	int T = floorf(time);
	float t = time - T;
	//glm::vec3 track = glm::catmullRom(cameraKeyPoints[(T - 1) % 10], cameraKeyPoints[T % 10], cameraKeyPoints[(T + 1) % 10], cameraKeyPoints[(T + 2) % 10], t);
	//glm::vec3 wektorStyczny = glm::normalize(glm::catmullRom(, s + 0.001) - glm::catmullRom(,  - 0.001));
	//// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	//cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	//glm::vec3 up = glm::vec3(0,1,0);
	//return Core::createViewMatrix(track, cameraDir, up);

	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);

}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,5)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.6f));
	drawObjectTexture(&floorModel, glm::translate(glm::vec3(0, -20, 0))* glm::rotate(glm::radians(270.0f), glm::vec3(0, 1, 0)), sandTexture);

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
				drawObjectTexture(&rockModel, glm::translate(glm::vec3(-j * +10, -19.2, (-1)*(-i + 4.7) * 10))* glm::rotate(glm::radians(270.0f), glm::vec3(-1, 1, 0)), rockTexture);
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			drawObjectTexture(&rockModel, glm::translate(glm::vec3(-j * -10, -19.2, (-1)*(-i + 4.7) * 10))* glm::rotate(glm::radians(270.0f), glm::vec3(-1, 1, 0)), rockTexture);
		}
	}

	glm::mat4 move;
	move[0][0] = 1;
	move[1][1] = 1;
	move[2][2] = 1;
	move[3][0] = time * 0.1;
	move[3][1] = time * 0.1;
	move[3][2] = time * 1;
	move[3][4] = 1;

	glm::mat4 raote1 = glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0))*move;
	glm::mat4 raote2= glm::rotate(glm::radians(90.0f), glm::vec3(0, -1, 0))*move;

	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		drawObjectTexture(&fishModel, glm::translate(asteroidPositions[i])* raote1, fishTexture);
	}
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		drawObjectTexture(&fishModel, glm::translate(asteroidPositions[i])* raote2, fishTexture);
	}

	//for (int i = 0; i < NUM_ASTEROIDS; i++)
	//{
	//	drawObjectTexture(&fishModel, glm::translate(asteroidPositions[i])* glm::rotate(glm::radians(90.0f - (i)), glm::vec3(-1, -1, 0))*move, fishTexture);
	//}

	//for (int i = 0; i < NUM_DOLPHINS; i++)
	//{
	//	drawObjectTexture(&dolphinModel, glm::translate(dolphinPositions[i])* glm::rotate(glm::radians(60.0f + (i)), glm::vec3(0, 1, 0)), dolphinTexture);
	//}
	//drawObjectTexture(&goldfishModel, glm::translate(glm::vec3(0, 0, 0))*cos(time), goldfishTexture);


	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	sphereModel = obj::loadModelFromFile("models/fish.obj");
	fishModel = obj::loadModelFromFile("models/fish.obj");
	floorModel = obj::loadModelFromFile("models/underwaterfloor.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	rockModel = obj::loadModelFromFile("models/rock.obj");
	dolphinModel = obj::loadModelFromFile("models/DOLPHIN.OBJ");
	textureEarth = Core::LoadTexture("textures/fish_texture.png");
	fishTexture = Core::LoadTexture("textures/fish_texture.png");
	sandTexture = Core::LoadTexture("textures/sand.png");
	rockTexture = Core::LoadTexture("textures/rock.png");
	dolphinTexture = Core::LoadTexture("textures/DOLPHIN.TIF");

	static const float astRadius = 6.0;
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		float angle = (float(i))*(2 * glm::pi<float>() / NUM_ASTEROIDS);
		asteroidPositions[i] = glm::ballRand(30.0);
	}

	static const float camRadius = 3.55;
	static const float camOffset = 0.6;
	for (int i = 0; i < NUM_CAMERA_POINTS; i++)
	{
		float angle = (float(i))*(2 * glm::pi<float>() / NUM_CAMERA_POINTS);
		float radius = camRadius *(0.95 + glm::linearRand(0.0f, 0.1f));
		cameraKeyPoints[i] = glm::vec3(cosf(angle) + camOffset, 0.0f, sinf(angle)) * radius;
	}

	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1980, 1080);
	glutCreateWindow("OpenGL Pierwszy Projekcik");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
