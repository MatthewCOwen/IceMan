#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

/*
Inheritance Hierarchy

GraphObject
	Actor
		Iceman

		Protester
			RegularProtester
			HardcoreProtester

		Ice
		Boulder
		Squirt

		Items
			WaterPool
			SonarKit
			GoldNugget
			OilBarrel
*/

const double SIZE_NORMAL = 1.0;

const double SIZE_SMALL = 0.25;


class StudentWorld;

class Point
{
public:

	Point(int x = -1, int y = -1);

	Point(const Point& p);

	int getX() const;

	int getY() const;

	Point getAdjLeft() const;

	Point getAdjUp() const;

	Point getAdjRight() const;

	Point getAdjDown() const;

	Point& operator=(Point& p);

	bool operator==(Point& p);

	bool operator!=(Point& p);
	
	bool isValid() const;

	bool isInBounds() const;

	int m_x;

	int m_y;
};

class BoundingBox
{
public:
	
	BoundingBox(int x = 0, int y = 0);

	BoundingBox(int x, int y, int size);

	void updateBB(int x, int y);

	Point* getBB();

	Point getXY();

	bool intersects(BoundingBox BB);

	bool operator==(BoundingBox &BB);

	bool operator!=(BoundingBox &BB);

	~BoundingBox();

private:

	int m_size;

	Point* m_box;
};

class Actor : public GraphObject
{
public:

	enum DamageSource {waterSpray, rockFall, protest};
		
	Actor(int imageID, int startX, int startY, Direction dir,
		  double size, unsigned int depth, int health,
		  bool isDamageable, bool isPassable);

	bool isFacing(Actor* other) const;

	void faceTowards(Actor* other);
		
	virtual void move();

	virtual void takeDamage(DamageSource src) = 0;

	StudentWorld* getWorld();

	BoundingBox getBB();

	bool isDamageable() const;

	bool isPassable() const;

	bool isAlive() const;

	void setDead();

	int getHealth() const;

	void incHealth();

	void decHealth();

	void decHealth(int i);

	int getTicksAlive() const;

	virtual ~Actor();

private:
	
	virtual void doSomething() = 0;

	BoundingBox m_BB;

	int m_ticksAlive;

	int m_health;

	int m_iFrames;

	bool m_isAlive;

	bool m_isDamageable;

	bool m_isPassable;

};


//Iceman Class

class Iceman : public Actor
{
public:
		
	Iceman(int imageID = IID_PLAYER,
		   int startX = 30,
		   int startY = 60,
		   Direction dir = right,
		   double size = 1.0,
		   unsigned int depth = 0,
		   int health = 10,
		   bool isDamageable = true,
		   bool isPassable = true);

	void doSomething();
	
	void takeDamage(DamageSource src);

	void gotSonarKit();
	
	void gotGoldNugget();
	
	void gotWater();

	int getNumSquirts();
	
	int getNumSonarKits();
	
	int getNumGoldNuggets();

	virtual ~Iceman();
	
private:

	int m_numSonarKits;
	
	int m_numGoldNuggets;
	
	int m_numSquirts;

};


//Protester Classes

class Protester : public Actor
{
public:

	enum States { InOilField, LeaveOilField };

	Protester(int imageID, int health, int x, int y);

	States getState() const;

	virtual void doSomething();
		
	virtual void takeDamage(DamageSource src);
	
	virtual void foundGold() = 0;	

	virtual ~Protester();

private:
		
	virtual int getGiveUpPoints() = 0;
	
	virtual void pathTowardsPlayer() = 0;

protected:
		
	int m_stepsInCurrDir;

	int m_ticksSinceAxisSwap;

	int m_nonShoutingActions;
	
	int m_restingTickCount;
	
	int m_stunTicksLeft;

	std::string m_pathOut;

	States m_state;

	bool m_isBribed;
};

class RegularProtester : public Protester
{
public:

	RegularProtester(int x = 60, int y = 60);

	virtual ~RegularProtester();

private:

	virtual int getGiveUpPoints();

	virtual void pathTowardsPlayer();

	virtual void foundGold();
};

class HardcoreProtester : public Protester
{
public:

	HardcoreProtester(int x = 60, int y = 60);

	virtual ~HardcoreProtester();

private:

	virtual int getGiveUpPoints();

	virtual void pathTowardsPlayer();

	virtual void foundGold();

	std::string m_pathToPlayer;

	std::size_t m_maxPathSize;
};


//Environmental Objects

class Ice : public Actor
{
public:

	Ice(int startX, int startY);

	virtual void doSomething();
	
	virtual void takeDamage(DamageSource src);

	virtual ~Ice();
};

class Boulder : public Actor
{
public:
		
	Boulder(int startX, int startY);
		
	virtual void doSomething();

	virtual void takeDamage(DamageSource src);

	virtual ~Boulder();
	
private:

	int m_ticksUnstable;

	bool m_isStable;
	
	bool m_isFalling;
};

class Squirt : public Actor
{
public:

	Squirt(int startX, int startY, Direction dir);

	virtual void takeDamage(DamageSource src);

	virtual void doSomething();

	virtual ~Squirt();

private:

	int m_movesLeft;

};


//Items

class Item : public Actor
{
public:

	enum States {Permanent, Temporary};	
		
	Item(int imageID, int startX, int startY, 
		 Direction dir, double size, 
		 unsigned int depth, States state);

	virtual ~Item();

	virtual void takeDamage(DamageSource src);

	virtual void doSomething();

	void setPickedUp();

	bool hasBeenPickedUp() const;

	States getState() const;

	void setTempLifetime(int ticks = 100);

	int getTempTicksLeft();

private:

	virtual void ItemDoSomething() = 0;

	int m_tempLifetime;

	States m_state;
	
	bool m_hasBeenPickedUp;

};

class OilBarrel : public Item
{
public:

	OilBarrel(int x, int y);

	~OilBarrel();

private:

	virtual void ItemDoSomething();

};

class GoldNugget : public Item
{
public:

	GoldNugget(int x, int y, States state);

	~GoldNugget();

private:

	virtual void ItemDoSomething();

};

class SonarKit : public Item
{
public: 

	SonarKit();

	~SonarKit();

private:

	virtual void ItemDoSomething();

};

class WaterPool : public Item
{
public:

	WaterPool(int x, int y);

	~WaterPool();

private:

	virtual void ItemDoSomething();

};

#endif // ACTOR_H_
