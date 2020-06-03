#pragma once

class Mob {
private:
	int x, y, z;
public:
	void setValues(const int x, const int y, const int z);
	int getX();
	int getY();
	int getZ();
	~Mob();
};

class Slime : public Mob {
private:
	int r, g, b;
public:
	void setColour(const float r, const float g, const float b);
	int getR();
	int getG();
	int getB();
};

class Sheep : public Mob {
private:
	int r, g, b;
	bool hasWool;
public:
	void setColour(const float r, const float g, const float b);
	void setWool(bool w);
	int getR();
	int getG();
	int getB();
};