#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;

float x_position = -5.0;
float camera[3] = { -23,35,0 };
int rotateQ = 0;
int currentRotation = 0;
const int worldSize = 100;
const int worldHeight = 15;
int terrain[worldSize][worldSize][worldHeight];

enum block {
	AIR = 0,
	DIRT = 1,
	BEDROCK = 2,
	WATER = 3
	}blockType;

void drawText(const char* text, int length, int x, int y) {
	glMatrixMode(GL_PROJECTION);
	double* matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glOrtho(0, 800, 0, 800, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0;i < length;i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

void init()
{
	//check for depth ON
	glEnable(GL_DEPTH_TEST);

	//enable blend for alpha
	glEnable(GL_BLEND);

	//smooth shading model
	glShadeModel(GL_SMOOTH);
}

void generateTerrain()
{
	srand(time(NULL));

	cout << "Z LEVEL: 0 " << endl;
	for (int x = 0; x < worldSize; x++)
	{
		for (int y = 0; y < worldSize; y++)
		{
			terrain[x][y][0] = BEDROCK;
			cout << terrain[x][y][0] << " ";
		}
		cout << endl;
	}

	for (int z = 1; z < worldHeight; z++)
	{
		cout << "Z LEVEL:" << z << endl;
		for (int x = 0; x < worldSize; x++) 
		{
			for (int y = 0; y < worldSize; y++)
			{
				if (z > 5)
				{
					terrain[x][y][z] = AIR;
					cout << terrain[x][y][z] << " ";
					continue;
				}
				int type = rand() % 4 + 1;
				if (type == 1)
				{
					terrain[x][y][z] = AIR;
				}
				if (type == 2)
				{
					if (z == 5) {
						terrain[x][y][z] = WATER;
					}
					else {
						terrain[x][y][z] = DIRT;
					}
				}
				if (type == 3 || type == 4)
				{
					terrain[x][y][z] = DIRT;
				}
				if (type == 5)
				{
					terrain[x][y][z] = BEDROCK;
				}
				cout << terrain[x][y][z] << " ";
			}
			cout << endl;
		}
		cout << "------------------------"<<endl<<endl;
	}
}

void spreadWater() {
	for (int z = 0; z < worldHeight; z++)
	{
		for (int x = 0; x < worldSize; x++)
		{
			for (int y = 0; y < worldSize; y++)
			{
				if (terrain[x][y][z] == WATER)
				{
					if (terrain[x - 1][y][z] == AIR || terrain[x + 1][y][z] == AIR
						|| terrain[x][y - 1][z] == AIR || terrain[x][y+1][z] == AIR)
					{
						cout << "Fixing water at :" << x << y << z << endl;
					}
				}
			}
		}
	}
}

void changeViewPort(int w, int h)
{
	//viewport
	glViewport(0, 0, w, h);

	//projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 1, 0, 0);

	glTranslatef(0.0, 0.0, x_position);
	glRotatef(currentRotation, 1, 1, 1);

	glPointSize(5);

	glPushMatrix();
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(0, 0, 0);
	glEnd();
	glPopMatrix();



	glPushMatrix();
	//draw a shape
	glBegin(GL_QUADS);
	glColor3f(0.9, 0.1, 0.1);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(0.0, 7.0, 0.0);
	glVertex3f(1.0, 7.0, 0.0);
	glVertex3f(1.0, 6.0, 0.0);

	glEnd();
	glPopMatrix();


	for (int x = 0; x < worldSize; x++) {
		for (int y = 0; y < worldSize; y++) {
			for (int z = 0; z < worldHeight; z++) {
				for (float i = 0.0; i < terrain[x][y][z]; i = i + 1)
				{
					if (terrain[x][y][z] == AIR) {
						continue;
					}
					else if (terrain[x][y][z] == DIRT) {

						glPushMatrix();
						//draw a shape
						glBegin(GL_QUADS);

						//front
						glColor3f(0.2, 0.1, 0.0);

						glVertex3f(y, z, x);
						glVertex3f(y, z + 1.0, x);
						glVertex3f(y + 1.0, z + 1.0, x);
						glVertex3f(y + 1.0, z, x);

						//top
						glColor3f(0.16, 0.5, 0.0);

						glVertex3f(y, z + 1, x);
						glVertex3f(y, z + 1, x + 1.0);
						glVertex3f(y + 1.0, z + 1, x + 1.0);
						glVertex3f(y + 1.0, z + 1, x);

						//back 
						glColor3f(0.2, 0.1, 0.0);

						glVertex3f(y, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y, z, x + 1.0);

						//bottom
						glColor3f(0.2, 0.1, 0.0);

						glVertex3f(y, z, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y + 1.0, z, x);
						glVertex3f(y, z, x);

						//right 
						glColor3f(0.2, 0.1, 0.0);

						glVertex3f(y + 1.0, z + 1.0, x);
						glVertex3f(y + 1.0, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y + 1.0, z, x);

						//left
						glColor3f(0.2, 0.1, 0.0);

						glVertex3f(y, z + 1.0, x);
						glVertex3f(y, z + 1.0, x + 1.0);
						glVertex3f(y, z, x + 1.0);
						glVertex3f(y, z, x);


						glEnd();
						glPopMatrix();
					}
					else if (terrain[x][y][z] == WATER) {

						glPushMatrix();
						//draw a shape
						glBegin(GL_QUADS);

						//front
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y, z, x);
						glVertex3f(y, z + 1.0, x);
						glVertex3f(y + 1.0, z + 1.0, x);
						glVertex3f(y + 1.0, z, x);

						//top
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y, z + 1, x);
						glVertex3f(y, z + 1, x + 1.0);
						glVertex3f(y + 1.0, z + 1, x + 1.0);
						glVertex3f(y + 1.0, z + 1, x);

						//back 
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y, z, x + 1.0);

						//bottom
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y, z, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y + 1.0, z, x);
						glVertex3f(y, z, x);

						//right 
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y + 1.0, z + 1.0, x);
						glVertex3f(y + 1.0, z + 1.0, x + 1.0);
						glVertex3f(y + 1.0, z, x + 1.0);
						glVertex3f(y + 1.0, z, x);

						//left
						glColor4f(0.05, 0.05, 0.95, 0.65);

						glVertex3f(y, z + 1.0, x);
						glVertex3f(y, z + 1.0, x + 1.0);
						glVertex3f(y, z, x + 1.0);
						glVertex3f(y, z, x);


						glEnd();
						glPopMatrix();
					}
					else if (terrain[x][y][z] == BEDROCK) {

					glPushMatrix();
					//draw a shape
					glBegin(GL_QUADS);

					//front
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y, z, x);
					glVertex3f(y, z + 1.0, x);
					glVertex3f(y + 1.0, z + 1.0, x);
					glVertex3f(y + 1.0, z, x);

					//top
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y, z + 1, x);
					glVertex3f(y, z + 1, x + 1.0);
					glVertex3f(y + 1.0, z + 1, x + 1.0);
					glVertex3f(y + 1.0, z + 1, x);

					//back 
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y, z + 1.0, x + 1.0);
					glVertex3f(y + 1.0, z + 1.0, x + 1.0);
					glVertex3f(y + 1.0, z, x + 1.0);
					glVertex3f(y, z, x + 1.0);

					//bottom
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y, z, x + 1.0);
					glVertex3f(y + 1.0, z, x + 1.0);
					glVertex3f(y + 1.0, z, x);
					glVertex3f(y, z, x);

					//right 
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y + 1.0, z + 1.0, x);
					glVertex3f(y + 1.0, z + 1.0, x + 1.0);
					glVertex3f(y + 1.0, z, x + 1.0);
					glVertex3f(y + 1.0, z, x);

					//left
					glColor3f(0.05, 0.05, 0.05);

					glVertex3f(y, z + 1.0, x);
					glVertex3f(y, z + 1.0, x + 1.0);
					glVertex3f(y, z, x + 1.0);
					glVertex3f(y, z, x);


					glEnd();
					glPopMatrix();
					}
				}
			}
		}

	}

	std::string cameraX = std::to_string(camera[0]);
	std::string cameraY = std::to_string(camera[1]);
	std::string cameraZ = std::to_string(camera[2]);
	std::string text;
	text.append("Eye at X: ");
	text.append(cameraX);
	text.append(" Y: ");
	text.append(cameraY);
	text.append(" Z: ");
	text.append(cameraZ);
	text.append(" Terrain Size: ");
	text.append(std::to_string(worldSize));
	text.append("X");
	text.append(std::to_string(worldSize));
	text.append("X");
	text.append(std::to_string(worldHeight));
	text.append(" Rotation: ");
	text.append(std::to_string(currentRotation));

	drawText(text.data(), text.size(), 0, 0);

	glutSwapBuffers();
}


void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);

	if (GetAsyncKeyState('A')) {
		camera[1]= camera[1] - 0.1;
	}
	if (GetAsyncKeyState('D')) {
		camera[1] = camera[1] + 0.1;
	}
	if (GetAsyncKeyState('W')) {
		camera[0] = camera[0] - 0.1;
	}
	if (GetAsyncKeyState('S')) {
		camera[0] = camera[0] + 0.1;
	}
	if (GetAsyncKeyState('Q')) {
		camera[2] = camera[2] - 0.1;
	}
	if (GetAsyncKeyState('E')) {
		camera[2] = camera[2] + 0.1;
	}
	if (GetAsyncKeyState('R')) {
		//generateTerrain();
		currentRotation = 0;
		camera[0] = -2;
		camera[1] = 10;
		camera[2] = -9;
	}


	if (GetAsyncKeyState(VK_SPACE))
	{
		rotateQ = 1;
		currentRotation += 3;
	}
	else
	{
		rotateQ = 0;
	}

}

int main(int argc, char* argv[]) {
	generateTerrain();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Cube Demo");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutReshapeFunc(changeViewPort);
	glutDisplayFunc(render);
	init();
	glutTimerFunc(0, timer, 0);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	else
	{
		std::cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION) << endl;
	}

	glutMainLoop();
	return 0;
}

