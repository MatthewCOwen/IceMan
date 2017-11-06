#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"

#include "Actor.h"

#include <string>
#include <vector>
#include <ctime>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class IceManager;

class PathFinder;


class StudentWorld : public GameWorld
{
public:

	StudentWorld(std::string assetDir);

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	std::string getGameText();
	
	static StudentWorld* getInstance();

 	IceManager* getIceManager();

	Actor* collisionWith(BoundingBox BB);

	Iceman* getPlayer();

	void decBarrelCount();

	void acceptActor(Actor* a);

	void spawnGoodie();

	bool squirtTargets(BoundingBox BB, Squirt* blackList);

	void scan(double x, double y);

	double getDistSquared(double x1, double y1, double x2, double y2);

	int getDistSquared(Point p1, Point p2);

	~StudentWorld();

private:

	std::vector<Actor*> m_listActors;
	IceManager* m_ice;
	Iceman* m_player;

	PathFinder* m_path;

	unsigned int m_currTime;
	int m_ticksThisSec;

	int m_numOilBarrels;

	int m_itemSpawnChance;

	int m_worldAge;

	int m_numProtesters;
	int m_numMaxProtesters;
	int m_minTicksBetweenProtesterSpawn;
	int m_ticksSinceLastProtesterAdded;
	int m_chanceHardcore;

	static StudentWorld* world;

	bool isValidSpawnLocation(int x, int y);

	

};


// Manages all of the ice objects.

class IceManager
{
public:

	IceManager();
		
	Ice*& operator()(int x, int y);

	bool checkIce(int x, int y);

	bool clearIce(int x, int y);
	
	~IceManager();

private:

	Ice** m_arr;
};


// Calculates path out of oil field.

class PathFinder
{
public: 

	PathFinder(StudentWorld* world);

	const std::string getPathFrom(int x, int y);

	void showPath();

	void updatePath();

private:

	StudentWorld* m_world;

	char m_grid[64][64];
};



#endif // STUDENTWORLD_H_
