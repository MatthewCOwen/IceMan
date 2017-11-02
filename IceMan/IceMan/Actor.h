#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

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

class StudentWorld;

class Point
{
public:

	Point(int x = 0, int y = 0);

	int getX();

	int getY();
	
	Point& operator=(Point& p);

	bool operator==(Point& p);

	bool operator!=(Point& p);

private:

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

	Protester(int imageID, int startX, int startY, Direction dir,
			  double size, unsigned int depth, int health, 
			  bool isDamageable);

		
	virtual void takeDamage(DamageSource src);

	States getState() const;
	bool isStunned() const;

	virtual void foundGold() = 0;

	void toggleStunned();

	virtual ~Protester();

private:
		
	virtual void doSomething()=0;
		
	int m_nonShoutingActions;
	int m_restingTickCount;
		
	States m_state;
	bool m_isStunned;
	bool m_isBribed;
};

class RegularProtester : public Protester
{
public:

	RegularProtester(int imageID = IID_PROTESTER, 
						int startX = 60, 
						int startY = 60, 
						Direction dir = left,
						double size = 1.0, 
						unsigned int depth = 0, 
						int health = 5,
						bool isDamageable = true);

	virtual ~RegularProtester();

	virtual void foundGold();

private:

	virtual void doSomething();
};

class HardcoreProtester : public Protester
{
public:

	HardcoreProtester(int imageID = IID_HARD_CORE_PROTESTER, 
					  int startX = 60,
					  int startY = 60,
					  Direction dir = left,
					  double size = 1.0,
					  unsigned int depth = 0,
					  int health = 20,
					  bool isDamageable = true);

	virtual ~HardcoreProtester();

	virtual void foundGold();

private:

	virtual void doSomething();
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

	virtual void ItemDoSomething();

};

class GoldNugget : public Item
{
public:

	GoldNugget(int x, int y, States state);

	~GoldNugget();

	virtual void ItemDoSomething();
	
private:

};

class SonarKit : public Item
{
public: 

	SonarKit();

	~SonarKit();

	virtual void ItemDoSomething();

};

class WaterPool : public Item
{
public:

	WaterPool(int x, int y);

	~WaterPool();

	virtual void ItemDoSomething();

};

#endif // ACTOR_H_
