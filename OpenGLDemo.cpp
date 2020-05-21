#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;

float x_position = -5.0;
float camera[3] = { -18,6,63 };
int rotateQ = 0;
int currentRotation = 0;
const int chunkSize = 10;
const int chunksX = 10;
const int chunksY = 10;
const int chunksZ = 2;
int gameTicks = 0;


int terrain[chunksX][chunksY][chunksZ][chunkSize][chunkSize][chunkSize];
int chunkData[chunksX][chunksY][chunksZ];

enum block {
	AIR = 0,
	DIRT = 1,
	BEDROCK = 2,
	WATER = 3,
	LEAF = 4,
	STUMP = 5,
	LAVA = 6
}blockType;

enum chunk {
	PLAINS = 0,
	COAST = 1,
	FOREST = 2,
	LAVASEA = 3,
	SKY = 4,
	OCEAN = 5
}chunkType;

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

void drawStatusBar() {
	int totalChunks = chunksX * chunksY * chunksZ;

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
	text.append(" Chunks Rendered: ");
	text.append(std::to_string(totalChunks));
	text.append(" Ticks: ");
	text.append(std::to_string(gameTicks));

	drawText(text.data(), text.size(), 0, 0);

}

void generateTree(int chunkX, int chunkY, int chunkZ)
{
	int treeY = rand() % 9 + 1;
	int treeX = rand() % 9 + 1;

	//leaves part
	for (int x = -2; x < 2;x++)
		for (int y = -2; y < 2;y++)
		{
			int resultX = treeX + x;
			int resultY = treeY + y;
			if (resultX > 9)
				resultX = 9;
			if (resultX < 0)
				resultX = 0;
			if (resultY > 9)
				resultY = 9;
			if (resultY < 0)
				resultY = 0;


			terrain[chunkX][chunkY][chunkZ][resultX][resultY][9] = LEAF;
		}
	//stump
	for (int treeHeight = 6; treeHeight < 10; treeHeight++)
		terrain[chunkX][chunkY][chunkZ][treeX][treeY][treeHeight] = STUMP;



	cout << "generating tree at CH: " << chunkX << ", " << chunkY << ", " << " X: " << treeX << " Y: " << treeY << endl;
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

void generateChunks()
{
	for (int x = 0; x < chunksX;x++)
		for (int y = 0; y < chunksY;y++)
			{
			int chomnk = rand() % 6;
			if (chomnk == 4)
				chomnk = 1;
			chunkData[x][y][0] = chomnk;
			}

	for (int x = 0; x < chunksX;x++)
	{
		for (int y = 0; y < chunksY;y++)
			for (int z = 1; z < chunksZ;z++)
			{
				chunkData[x][y][z] = SKY;
			}
	}
	return;
}

void generateTerrain()
{
	cout << "Generating Terrain..." << endl;
	srand(time(NULL));

	for (int cx = 0; cx < chunksX; cx++)
	{
		for (int cy = 0; cy < chunksY; cy++)
		{
			for (int cz = 0; cz < chunksZ; cz++)
			{
				cout << "Chunk: " << cx << ", " << cy << ", " << cz;
				cout << "Type: " << std::to_string(chunkData[cx][cy][cy]) << endl;
				//Set bottom layer to bedrock

				for (int z = 0; z < chunkSize; z++)
				{
					for (int x = 0; x < chunkSize; x++)
					{
						for (int y = 0; y < chunkSize; y++)
						{
							int type = rand() % 10 + 1;

							if (chunkData[cx][cy][cz] == PLAINS)
							{

								//deep ground should be mostly bedrock/dirt
								if (z <= 3)
								{
									if (type == 1)
										terrain[cx][cy][cz][x][y][z] = BEDROCK;
									else
										terrain[cx][cy][cz][x][y][z] = DIRT;
								}
								else if (z > 3 && z <= 5)
									terrain[cx][cy][cz][x][y][z] = DIRT;
								else if (z > 5)
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == COAST)
							{

								//deep ground should be mostly bedrock/dirt
								if (z <= 3)
								{
									if (type == 1)
										terrain[cx][cy][cz][x][y][z] = BEDROCK;
									else
										terrain[cx][cy][cz][x][y][z] = DIRT;
								}
								else if (z > 3 && z <= 5)
									terrain[cx][cy][cz][x][y][z] = DIRT;
								else if (z > 5)
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == LAVASEA)
							{
								if (z < 5)
									terrain[cx][cy][cz][x][y][z] = LAVA;
								else
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == OCEAN)
							{
								if (z < 5)
									terrain[cx][cy][cz][x][y][z] = WATER;
								else
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == FOREST)
							{
								//deep ground should be mostly bedrock/dirt
								if (z <= 3)
								{
									if (type == 1)
										terrain[cx][cy][cz][x][y][z] = BEDROCK;
									else
										terrain[cx][cy][cz][x][y][z] = DIRT;
								}
								else if (z > 3 && z <= 5)
									terrain[cx][cy][cz][x][y][z] = DIRT;
								else if (z > 5)
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == SKY)
							{
								terrain[cx][cy][cz][x][y][z] = AIR;
							}
							terrain[cx][cy][cz][x][y][0] = BEDROCK;
						}	
					}
				}	
				if (chunkData[cx][cy][cz] == PLAINS)
					generateTree(cx, cy, cz);
				if (chunkData[cx][cy][cz] == FOREST)
				{
					generateTree(cx, cy, cz);
					generateTree(cx, cy, cz);
				}
				cout << "... done" << endl;
			}
		}
	}
}

void spreadWater() {
	for (int cx = 0; cx < chunksX; cx++)
	{
		for (int cy = 0; cy < chunksY; cy++)
		{
			for (int cz = 0; cz < chunksZ; cz++)
			{
				for (int z = 0; z < chunkSize; z++)
				{
					for (int x = 0; x < chunkSize; x++)
					{
						for (int y = 0; y < chunkSize; y++)
						{
							if (terrain[cx][cy][cz][x][y][z] == WATER)
							{
								if (terrain[cx][cy][cz][x - 1][y][z] == AIR)
								{
									terrain[cx][cy][cz][x - 1][y][z] = WATER;
								}

								if (terrain[cx][cy][cz][x + 1][y][z] == AIR)
								{
									terrain[cx][cy][cz][x + 1][y][z] = WATER;
								}

								if (terrain[cx][cy][cz][x][y - 1][z] == AIR)
								{
									terrain[cx][cy][cz][x][y - 1][z] = WATER;
								}

								if (terrain[cx][cy][cz][x][y + 1][z] == AIR)
								{
									terrain[cx][cy][cz][x][y + 1][z] = WATER;
								}

								if (!z <= 0)
									if (terrain[cx][cy][cz][x][y][z - 1] == AIR)
									{
										terrain[cx][cy][cz][x][y][z - 1] = WATER;
									}

								if (terrain[cx][cy][cz][x][y][z + 1] == AIR && z < 5)
								{
									terrain[cx][cy][cz][x][y][z + 1] = WATER;
								}

							}
						}
					}
				}
			}
		}
	}
}

void spreadLava() {
	for (int cx = 0; cx < chunksX; cx++)
	{
		for (int cy = 0; cy < chunksY; cy++)
		{
			for (int cz = 0; cz < chunksZ; cz++)
			{
				for (int z = 0; z < chunkSize; z++)
				{
					for (int x = 0; x < chunkSize; x++)
					{
						for (int y = 0; y < chunkSize; y++)
						{
							if (terrain[cx][cy][cz][x][y][z] == LAVA)
							{
								if (terrain[cx][cy][cz][x - 1][y][z] == AIR)
								{
									terrain[cx][cy][cz][x - 1][y][z] = LAVA;
								}

								if (terrain[cx][cy][cz][x + 1][y][z] == AIR)
								{
									terrain[cx][cy][cz][x + 1][y][z] = LAVA;
								}

								if (terrain[cx][cy][cz][x][y - 1][z] == AIR)
								{
									terrain[cx][cy][cz][x][y - 1][z] = LAVA;
								}

								if (terrain[cx][cy][cz][x][y + 1][z] == AIR)
								{
									terrain[cx][cy][cz][x][y + 1][z] = LAVA;
								}

								if (!z <= 0)
									if (terrain[cx][cy][cz][x][y][z - 1] == AIR)
									{
										terrain[cx][cy][cz][x][y][z - 1] = LAVA;
									}

								if (terrain[cx][cy][cz][x][y][z + 1] == AIR && z < 5)
								{
									terrain[cx][cy][cz][x][y][z + 1] = LAVA;
								}

							}
						}
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
	gluPerspective(60, 1.0, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void renderGrassBlock(int cx, int cy, int cz, int x, int y, int z)
{
	//front
	glColor3f(0.2, 0.1, 0.0);
	glVertex3f((float)y + (chunkSize * cy), (float)z + (chunkSize * cz), (float)x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor3f(0.16, 0.5, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));

	//back 
	glColor3f(0.2, 0.1, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor3f(0.2, 0.1, 0.0);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor3f(0.2, 0.1, 0.0);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor3f(0.2, 0.1, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void renderStumpBlock(int cx, int cy, int cz, int x, int y, int z)
{
	//front
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f((float)y + (chunkSize * cy), (float)z + (chunkSize * cz), (float)x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1, x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1, x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1, x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1, x + (chunkSize * cx));

	//back 
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor3f(0.15, 0.05, 0.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void renderLeafBlock(int cx, int cy, int cz, int x, int y, int z)
{

	//front
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f((float)y + (chunkSize * cy), (float)z + (chunkSize * cz), (float)x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));

	//back 
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor3f(0.15, 0.95, 0.15);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void renderWaterBlock(int cx, int cy, int cz, int x, int y, int z)
{

	//front
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));

	//back 
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor4f(0.05, 0.05, 0.95, 0.65);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void renderLavaBlock(int cx, int cy, int cz, int x, int y, int z)
{

	//front
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));

	//back 
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor4f(0.95, 0.05, 0.05, 1.0);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void renderBedrockBlock(int cx, int cy, int cz, int x, int y, int z)
{
	//front
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//top
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));

	//back 
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));

	//bottom
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//right 
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	//left
	glColor3f(0.05, 0.05, 0.05);
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	glEnd();
	glPopMatrix();
}

void render()
{
	gameTicks++;
	if (gameTicks % 100 == 0)
	{
		cout << "Fluids tick" << endl;
	spreadWater();
	spreadLava();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 1, 0, 0);

	glTranslatef(0.0, 0.0, x_position);
	glRotatef((float)currentRotation, 1, 1, 1);

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

	for (int cx = 0; cx < chunksX; cx++)
	{
		for (int cy = 0; cy < chunksY; cy++)
		{
			for (int cz = 0; cz < chunksZ; cz++)
			{
				for (int x = 0; x < chunkSize; x++)
				{
					for (int y = 0; y < chunkSize; y++)
					{
						for (int z = 0; z < chunkSize; z++)
						{
							//draw a shape
							glPushMatrix();
							glBegin(GL_QUADS);

							//Render nothing if air
							if (terrain[cx][cy][cz][x][y][z] == AIR)
							{
								glEnd();
								glPopMatrix();
							}
							if (chunkData[cx][cy][cz] == SKY)
							{
								glEnd();
								glPopMatrix();
							}

							else if (terrain[cx][cy][cz][x][y][z] == DIRT)
								renderGrassBlock(cx, cy, cz, x, y, z);
							else if (terrain[cx][cy][cz][x][y][z] == STUMP)
								renderStumpBlock(cx, cy, cz, x, y, z);
							else if (terrain[cx][cy][cz][x][y][z] == LEAF)
								renderLeafBlock(cx, cy, cz, x, y, z);
							else if (terrain[cx][cy][cz][x][y][z] == WATER)
								renderWaterBlock(cx, cy, cz, x, y, z);
							else if (terrain[cx][cy][cz][x][y][z] == BEDROCK)
								renderBedrockBlock(cx, cy, cz, x, y, z);
							else if (terrain[cx][cy][cz][x][y][z] == LAVA)
								renderLavaBlock(cx, cy, cz, x, y, z);
							}
						}
					}
				}
			}
	}
	drawStatusBar();
	glutSwapBuffers();
}

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);

	if (GetAsyncKeyState('A')) {
		camera[1] = camera[1] - 1.0;
	}
	if (GetAsyncKeyState('D')) {
		camera[1] = camera[1] + 1.0;
	}
	if (GetAsyncKeyState('W')) {
		camera[0] = camera[0] - 1.0;
	}
	if (GetAsyncKeyState('S')) {
		camera[0] = camera[0] + 1.0;
	}
	if (GetAsyncKeyState('Q')) {
		camera[2] = camera[2] - 1.0;
	}
	if (GetAsyncKeyState('E')) {
		camera[2] = camera[2] + 1.0;
	}
	if (GetAsyncKeyState('R')) {
		currentRotation = 0;
		camera[0] = -50;
		camera[1] = 50;
		camera[2] = -50;
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
	generateChunks();
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

