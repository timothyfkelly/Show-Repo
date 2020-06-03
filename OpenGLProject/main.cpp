#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include "mobs.h"
#include "main.h"

using namespace std;

float playerX = 0.0;
float playerY = 6.0;
float playerZ = 0.0;
float currentRotation = 0.0;

Sheep* ptr[2];
Sheep mobs[2];

const int chunkSize = 10;
const int chunksX = 5;
const int chunksY = 5;
const int chunksZ = 2;
int gameTicks = 0;
bool observerMode = false;

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

Sheep** createSheep(Sheep* ptr[])
{
	 srand(time(NULL));
	 ptr[0] = &mobs[0];
	 ptr[1] = &mobs[1];
	 ptr[0]->setValues(rand() %20 + 1, 6, rand() %20 + 1);
	 ptr[1]->setValues(rand() % 20 + 1, 6, rand() % 20 + 1);
	 ptr[0]->setColour(1.0f, 1.0f, 1.0f);
	 ptr[1]->setColour(1.0f, 1.0f, 1.0f);
	 return ptr;
}

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

void restart() {
	playerX = 6.0;
	playerY = 6.0;
	playerZ = 6.0;
}

void drawStatusBar() {
	//Calculate stuff
	int xLeftover, zLeftover, yLeftover, xChunk, zChunk, tileID, yChunk, biome;
	xLeftover = (int)playerX % chunkSize;
	zLeftover = (int)playerZ % chunkSize;
	yLeftover = (int)playerY % chunkSize;
	xChunk = (int)playerX / chunkSize;
	zChunk = (int)playerZ / chunkSize;
	yChunk = (int)playerY / chunkSize;

	tileID = terrain[zChunk][xChunk][yChunk][xLeftover][zLeftover][yLeftover - 1];
	biome = chunkData[zChunk][xChunk][yChunk];

	std::string ticks;
	std::string chunk;
	std::string location;
	std::string tile;

	ticks.append("Ticks: ");
	ticks.append(std::to_string(gameTicks));

	chunk.append("Chunk X: ");
	chunk.append(std::to_string(xChunk));
	chunk.append(" Chunk Y: ");
	chunk.append(std::to_string(yChunk));
	chunk.append(" Chunk Z: ");
	chunk.append(std::to_string(zChunk));

	location.append("X: ");
	location.append(std::to_string(xLeftover) );
	location.append(" Y: ");
	location.append(std::to_string(yLeftover));
	location.append(" Z: ");
	location.append(std::to_string(zLeftover) );

	tile.append("Standing on: ");
	if (tileID == AIR)
		tile.append("air");
	if (tileID == BEDROCK)
		tile.append("bedrock");
	if (tileID == DIRT)
		tile.append("dirt");
	if (tileID == LAVA)
	{
		tile.append("lava");
		restart();
	}
	if (tileID == LEAF)
		tile.append("leaf");
	if (tileID == STUMP)
		tile.append("stump");
	if (tileID == WATER)
	{
		tile.append("water");
		playerY--;
	}

	drawText(ticks.data(), ticks.size(), 0, 788);
	drawText(chunk.data(), chunk.size(), 0, 773);
	drawText(location.data(), location.size(), 0, 758);
	drawText(tile.data(), tile.size(), 0, 743);

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
	cout << "Generating chunks..." << endl;
	srand(time(NULL));
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
	cout << "Generating terrain..." << endl;
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
								if (z <= 5)
									terrain[cx][cy][cz][x][y][z] = LAVA;
								else
									terrain[cx][cy][cz][x][y][z] = AIR;
							}
							if (chunkData[cx][cy][cz] == OCEAN)
							{
								if (z <= 5)
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
	gluPerspective(90, 1024/768, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void renderFront(int cx, int cy, int cz, int x, int y, int z)
{
	if ((cx == 0 && x == 0)||(terrain[cx][cy][cz][x][y][z] == STUMP) || (terrain[cx][cy][cz][x][y][z] == LEAF &&
		terrain[cx][cy][cz][x-1][y][z] != LEAF))
	{
		glVertex3f((float)y + (chunkSize * cy), (float)z + (chunkSize * cz), (float)x + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	}
}
void renderTop(int cx, int cy, int cz, int x, int y, int z)
{
	if ((terrain[cx][cy][cz][x][y][z+1] == AIR) || (terrain[cx][cy][cz][x][y][z] == STUMP && terrain[cx][cy][cz][x][y][z+1] != STUMP) 
		|| (terrain[cx][cy][cz][x][y][z] == LEAF && terrain[cx][cy][cz][x][y][z + 1] != LEAF))
	{
		if (terrain[cx][cy][cz][x][y][z] == DIRT)
			glColor3f(0.16, 0.5, 0.0);
		glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1 + (chunkSize * cz), x + (chunkSize * cx));
		if (terrain[cx][cy][cz][x][y][z] == DIRT)
			glColor3f(0.2, 0.1, 0.0);
	}

}
void renderBack(int cx, int cy, int cz, int x, int y, int z) {
	if (((cx == chunksX - 1) && (x == chunkSize - 1)) || (terrain[cx][cy][cz][x][y][z] == STUMP) ||
		(terrain[cx][cy][cz][x][y][z] == LEAF && terrain[cx][cy][cz][x + 1][y][z] != LEAF)) {

		glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
	}
}
void renderBottom(int cx, int cy, int cz, int x, int y, int z)
{




	if ((cz == 0 && z == 0) || (terrain[cx][cy][cz][x][y][z] == LEAF && terrain[cx][cy][cz][x][y][z -1] != LEAF))
	{
		glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	}
}
void renderRight(int cx, int cy, int cz, int x, int y, int z)
{
	if ((cy == 0 && y == 0) || (terrain[cx][cy][cz][x][y][z] == STUMP) ||
		(terrain[cx][cy][cz][x][y][z] == LEAF && terrain[cx][cy][cz][x][y+1][z] != LEAF)) {
		glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));
	}
}
void renderLeft(int cx, int cy, int cz, int x, int y, int z)
{
	if ((cy == chunksY - 1) && (y == 9) || (terrain[cx][cy][cz][x][y][z] == STUMP) ||
		(terrain[cx][cy][cz][x][y][z] == LEAF && terrain[cx][cy][cz][x][y - 1][z] != LEAF))
	{
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + 1.0 + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + 1.0 + (chunkSize * cx));
		glVertex3f(y + 1.0 + (chunkSize * cy), z + (chunkSize * cz), x + (chunkSize * cx));

	}
}

void renderBlock(int cx, int cy, int cz, int x, int y, int z, int block)
{
	if (block == WATER)
		glColor4f(0.05, 0.05, 0.95, 0.65);
	else if(block == BEDROCK)
			glColor3f(0.05, 0.05, 0.05);
	else if(block ==LAVA)
		glColor4f(0.95, 0.05, 0.05, 1.0);
	else if(block == STUMP)
		glColor3f(0.15, 0.05, 0.0);
	else if (block == DIRT)
		glColor3f(0.2, 0.1, 0.0);
	else if (block == LEAF)
		glColor3f(0.15, 0.95, 0.15);


	renderFront(cx, cy, cz, x, y, z);
	renderTop(cx, cy, cz, x, y, z);
	renderBack(cx, cy, cz, x, y, z);
	renderBottom(cx, cy, cz, x, y, z);
	renderRight(cx, cy, cz, x, y, z);
	renderLeft(cx, cy, cz, x, y, z);
	glEnd();
	glPopMatrix();
}
   
void renderPlayer()
{
	glPushMatrix();
	//draw player
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.45, 0.0);
	// front
	glVertex3f(playerX, playerY, playerZ);
	glVertex3f(playerX + 0.7, playerY, playerZ);
	glVertex3f(playerX + 0.7, playerY + 1.0, playerZ);
	glVertex3f(playerX, playerY + 1.0, playerZ);
	// back
	glVertex3f(playerX, playerY, playerZ - 0.65f);
	glVertex3f(playerX + 0.7, playerY, playerZ - 0.65f);
	glVertex3f(playerX + 0.7, playerY + 1.0, playerZ - 0.65f);
	glVertex3f(playerX, playerY + 1.0, playerZ - 0.65f);
	// right
	glVertex3f(playerX + 0.7, playerY, playerZ);
	glVertex3f(playerX + 0.7, playerY, playerZ - 0.65f);
	glVertex3f(playerX + 0.7, playerY + 1.0f, playerZ - 0.65f);
	glVertex3f(playerX + 0.7, playerY + 1.0f, playerZ);
	// left
	glVertex3f(playerX, playerY, playerZ);
	glVertex3f(playerX, playerY, playerZ - 0.65);
	glVertex3f(playerX, playerY + 1.0, playerZ - 0.65);
	glVertex3f(playerX, playerY + 1.0, playerZ);
	// top
	glVertex3f(playerX, playerY + 1, playerZ);
	glVertex3f(playerX +0.7, playerY + 1, playerZ);
	glVertex3f(playerX +0.7, playerY + 1, playerZ -0.65f);
	glVertex3f(playerX, playerY + 1, playerZ -0.65f);
	// bottom
	glVertex3f(playerX, playerY, playerZ );
	glVertex3f(playerX +0.7, playerY, playerZ);
	glVertex3f(playerX +0.7, playerY, playerZ -0.65f);
	glVertex3f(playerX, playerY, playerZ -0.65f);
	glPopMatrix();
}
void renderMobs()
{
	int size = *(&ptr + 1) - ptr;
	float x, y, z, r, g, b;
	glPushMatrix();
	for (int i = 0; i < size; i++)
	{
		x = ptr[i]->getX();
		y = ptr[i]->getY();
		z = ptr[i]->getZ();
		r = ptr[i]->getR();
		g = ptr[i]->getG();
		b = ptr[i]->getB();

		glBegin(GL_QUADS);
		glColor3f(r, g, b);
		// front
		glVertex3f(x,		y,			z);
		glVertex3f(1.0f + x,y,			z);
		glVertex3f(1.0f + x,0.7f + y,	z);
		glVertex3f(z,		0.7f + y,	z);
		////// back
		//glVertex3f(x,		y,			z-1.0f);
		//glVertex3f(x+1.0f,	y,			z-1.0f);
		//glVertex3f(x+1.0f,	y+0.7f,		z-1.0f);
		//glVertex3f(x,		y+0.7f,		z-1.0f);
		////// right
		//glVertex3f(x+1.0f,  y,			z);
		//glVertex3f(x+1.0f,  y,			z-1.0f);
		//glVertex3f(x+1.0f,  y + 0.7f,	z-1.0f);
		//glVertex3f(x+1.0f,  y + 0.7f,	z);
		////// left
		//glVertex3f(x,		y,			z);
		//glVertex3f(x,		y,			z-1.0f);
		//glVertex3f(x,		y + 0.7f,	z-1.0f);
		//glVertex3f(x,		y + 0.7f,	z);
		////// top
		//glVertex3f(x,		y+0.7f,		z);
		//glVertex3f(x+1.0f,	y+0.7f,		z);
		//glVertex3f(x+1.0f,	y+0.7f,		z-1.0f);
		//glVertex3f(x,		y+0.7f,		z-1.0f);
		////// bottom
		//glVertex3f(x,		y,			z);
		//glVertex3f(x+1.0f,	y,			z);
		//glVertex3f(x+1.0f,	y,			z-1.0f);
		//glVertex3f(x,		y,			z-1.0f);
		glPopMatrix();
	}
	glEnd();
}

void playerOutOfBounds()
{
	if (playerX < 0)
		playerX = 0;
	if (playerZ < 0.5)
		playerZ = 0.5;
	if (playerZ > chunksY * chunkSize)
		playerZ = chunksY * chunkSize;
	if (playerX > chunksX * chunkSize -1)
		playerX = chunksX * chunkSize -1;
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

	if (observerMode ==false)
		gluLookAt(playerX, playerY + 1.0, playerZ - 3.0, playerX, playerY, playerZ, 0, 1, 0);
	else
		gluLookAt(10, 10, 10 , playerX, playerY, playerZ, 0, 1, 0);
	glRotatef((float)currentRotation, 1, 1, 1);

	glPointSize(5);

	glPushMatrix();
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(playerX, 6, playerZ - 5);
	glEnd();
	glPopMatrix();

	renderPlayer();
	renderMobs();

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
								renderBlock(cx, cy, cz, x, y, z, DIRT);
							else if (terrain[cx][cy][cz][x][y][z] == STUMP)
								renderBlock(cx, cy, cz, x, y, z, STUMP);
							else if (terrain[cx][cy][cz][x][y][z] == LEAF)
								renderBlock(cx, cy, cz, x, y, z, LEAF);
							else if (terrain[cx][cy][cz][x][y][z] == WATER)
								renderBlock(cx, cy, cz, x, y, z, WATER);
							else if (terrain[cx][cy][cz][x][y][z] == BEDROCK)
								renderBlock(cx, cy, cz, x, y, z, BEDROCK);
							else if (terrain[cx][cy][cz][x][y][z] == LAVA)
								renderBlock(cx, cy, cz, x, y, z, LAVA);
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
		playerX = playerX + 0.5;
		playerOutOfBounds();
	}
	if (GetAsyncKeyState('D')) {
		playerX = playerX - 0.5;
		playerOutOfBounds();
	}
	if (GetAsyncKeyState('W')) {
		playerZ = playerZ + 0.5;
		playerOutOfBounds();
	}
	if (GetAsyncKeyState('S')) {
		playerZ = playerZ - 0.5;
		playerOutOfBounds();
	}
	if (GetAsyncKeyState('Q')) {
		return;
	}
	if (GetAsyncKeyState('E')) {
		return;
	}
	if (GetAsyncKeyState('R')) {
		currentRotation = 0;
		if (observerMode == false)
			observerMode = true;
		else
			observerMode = false;
	}
	if (GetAsyncKeyState('X')) {

		playerX = 6.0;
		playerY = 6.0;
		playerZ = 6.0;
	}
}

int main(int argc, char* argv[]) {
	generateChunks();
	generateTerrain();
	createSheep(ptr);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
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

