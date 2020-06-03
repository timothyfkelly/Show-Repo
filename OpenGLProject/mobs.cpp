#include "mobs.h"
#include <iostream>

int Mob::getX()
{
	return x;
}
int Mob::getY()
{
	return y;
}
int Mob::getZ()
{
	return z;
}
void Mob::setValues(const int X, const int Y, const int Z)
{
	x = X;
	y = Y;
	z = Z;
}
Mob::~Mob(void) {
	std::cout << "Mob destroyed" << std::endl;
}

int Slime::getR()
{
	return r;
}
int Slime::getG()
{
	return g;
}
int Slime::getB()
{
	return b;
}
void Slime::setColour(const float R, const float G, const float B)
{
	r = R;
	g = G;
	b = B;
}

int Sheep::getR()
{
	return r;
}
int Sheep::getG()
{
	return g;
}
int Sheep::getB()
{
	return b;
}
void Sheep::setColour(const float R, const float G, const float B)
{
	r = R;
	g = G;
	b = B;
}
void Sheep::setWool(bool W)
{
	hasWool = W;
}
