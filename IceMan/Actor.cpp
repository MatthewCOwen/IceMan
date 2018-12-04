#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <string>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Point 

using namespace std;

Point::Point(int x, int y) : m_x(x), m_y(y) 
{

}

Point::Point(const Point& p) : m_x(p.m_x), m_y(p.m_y)
{

}

int Point::getX() const
{
	return m_x;
}

int Point::getY() const
{
	return m_y;
}
	
Point& Point::operator=(Point& p)
{
	this->m_x = p.m_x;
	this->m_y = p.m_y;

	return *this;
}

bool Point::operator==(Point& p)
{
	return this->m_x == p.m_x && this->m_y == p.m_y;
}

bool Point::operator!=(Point& p)
{
	return !(*this == p);
}

bool Point::isValid() const
{
	return	(m_x != -1 && m_y != -1);
}

bool Point::isInBounds() const
{
	return	(m_x >= 0 && m_x <= 60) &&
			(m_y >= 0 && m_y <= 60);
}

Point Point::getAdjLeft() const
{
	return m_x > 0 ? Point(m_x - 1, m_y) : Point(-1, -1);
}

Point Point::getAdjUp() const
{
	return m_y < 63 ? Point(m_x, m_y + 1) : Point(-1, -1);
}

Point Point::getAdjRight() const
{
	return m_x < 63 ? Point(m_x + 1, m_y) : Point(-1, -1);
}

Point Point::getAdjDown() const
{
	return m_y > 0 ? Point(m_x, m_y - 1) : Point(-1, -1);
}

// BoundingBox

BoundingBox::BoundingBox(int x, int y) : m_size(4)
{
	m_box = new Point[m_size * m_size];
	updateBB(x, y);
}

BoundingBox::BoundingBox(int x, int y, int size) : m_size(size)
{
	m_box = new Point[m_size * m_size];
	updateBB(x, y);
}

void BoundingBox::updateBB(int x, int y)
{
	for (int i = 0; i < m_size; i++)
		for (int j = 0; j < m_size; j++)
			m_box[m_size * i + j] = Point(x + i, y + j);
}

Point* BoundingBox::getBB()
{
	return m_box;
}

Point BoundingBox::getXY()
{
	return m_box[0];
}

bool BoundingBox::intersects(BoundingBox BB)
{
	for (int i = 0; i < m_size * m_size; i++)
	{
		for (int j = 0; j < BB.m_size * BB.m_size; j++)
		{
			if (m_box[i] == BB.m_box[j])
			{
				return true;
			}
		}
	}
	return false;
}

bool BoundingBox::operator==(BoundingBox &BB)
{
	bool foundMatch;

	for (int i = 0; i < m_size * m_size; i++)
	{
		foundMatch = false;

		for (int j = 0; j < BB.m_size * BB.m_size; j++)
		{
			if (m_box[i] == BB.m_box[j])
			{
				foundMatch = true;
			}
		}

		if (!foundMatch)
		{
			return false;
		}
	}
	return true;
}

bool BoundingBox::operator!=(BoundingBox &BB)
{
	return !(*this == BB);
}

BoundingBox::~BoundingBox()
{

}

Actor::Actor(int imageID, int startX, int startY,
			 Direction dir, double size,
			 unsigned int depth, int health, 
			 bool isDamageable, bool isPassable) :	GraphObject(imageID, startX, startY, dir, size, depth),
													m_BB(BoundingBox(startX, startY)),
													m_isPassable(isPassable),
													m_isDamageable(isDamageable),
													m_health(health),
													m_isAlive(true),
													m_ticksAlive(0)
{

}

void Actor::move()
{
	if (!m_isAlive)
	{
		return;
	}

	doSomething();

	if (getX() != m_BB.getXY().m_x || getY() != m_BB.getXY().m_y)
	{
		m_BB.updateBB(getX(), getY());
	}

	m_ticksAlive++;
}

StudentWorld* Actor::getWorld()
{
	return StudentWorld::getInstance();
}

BoundingBox Actor::getBB()
{
	return m_BB;
}

bool Actor::isDamageable() const
{
	return m_isDamageable;
}

bool Actor::isAlive() const
{
	return m_isAlive;
}

bool Actor::isPassable() const
{
	return m_isPassable;
}

bool Actor::isFacing(Actor* other) const
{
	int deltaX = getX() - other->getX();
	int deltaY = getY() - other->getY();
	Direction dir = getDirection();

	if (deltaX >= 0)
	{
		if (dir == left)
		{
			return true;
		}
	}
	
	if (deltaX <= 0)
	{
		if (dir == right)
		{
			return true;
		}
	}

	if (deltaY >= 0)
	{
		if (dir == up)
		{
			return true;
		}
	}

	if (deltaY <= 0)
	{
		if (dir == down)
		{
			return true;
		}
	}

	return false;
}

void Actor::faceTowards(Actor* other)
{
	int deltaX = getX() - other->getX();
	int deltaY = getY() - other->getY();
	Direction dir = getDirection();

	if (deltaY < 0)
	{
		setDirection(up);
		return;
	}	

	if (deltaY > 0)
	{
		setDirection(down);
		return;
	}
	
	if (deltaX > 0)
	{
		setDirection(left);
		return;
	}

	if (deltaX < 0)
	{
		setDirection(right);
		return;
	}
}

int Actor::getHealth() const
{
	return m_health;
}

void Actor::incHealth()
{
	m_health++;
}

void Actor::decHealth()
{
	m_health--;
}

void Actor::decHealth(int i)
{
	m_health -= i;
} 

void Actor::setDead()
{
	m_isAlive = false;
	setVisible(false);
}

int Actor::getTicksAlive() const
{
	return m_ticksAlive;
}

Actor::~Actor()
{

}


// Iceman

Iceman::Iceman(int imageID, int startX, int startY, 
			   Direction dir, double size, 
			   unsigned int depth, int health,
			   bool isDamageable, bool isPassable) :	Actor(imageID, startX, startY, dir, size, depth, health, isDamageable, isPassable),
														m_numSonarKits(1),
														m_numGoldNuggets(0),
														m_numSquirts(5)
{
	setVisible(true);
}

void Iceman::doSomething()
{
	if (getHealth() > 0)
	{
		int action;
		StudentWorld* world = getWorld();

		if (world->getKey(action))
		{
			BoundingBox BB;
			Actor* collidedWith;
			bool canMove = true;

			switch (action)
			{
			case KEY_PRESS_LEFT:
				if (getDirection() != left)
				{
					setDirection(left);
					break;
				}

				BB = BoundingBox(getX() - 1, getY());
 				collidedWith = world->collisionWith(this, BB);

				if (collidedWith != nullptr)
				{
					if (!collidedWith->isPassable() && &*collidedWith != &*this)
					{
						canMove = false;
					}
				}

				if (getX() > 0 && canMove)
				{
					if (world->getIceManager()->clearIce(getX() - 1, getY()))
					{
						world->getPathFinder()->updateGrid();
						world->playSound(SOUND_DIG);
					}

					moveTo(getX() - 1, getY());
				}
				break;

			case KEY_PRESS_UP:
				if (getDirection() != up)
				{
					setDirection(up);
					break;
				}

				BB = BoundingBox(getX(), getY() + 1);
 				collidedWith = world->collisionWith(this, BB);

				if (collidedWith != nullptr && &*collidedWith != &*this)
				{
					if (!collidedWith->isPassable())
					{
						canMove = false;
					}
				}
				
				if (getY() < 60 && canMove)
				{
					if (world->getIceManager()->clearIce(getX(), getY() + 1))
					{
						world->getPathFinder()->updateGrid();
						world->playSound(SOUND_DIG);
					}

					moveTo(getX(), getY() + 1);
				}
				break;

			case KEY_PRESS_RIGHT:
				if (getDirection() != right)
				{
					setDirection(right);
					break;
				}
				
				BB = BoundingBox(getX() + 1, getY());
 				collidedWith = world->collisionWith(this, BB);

				if (collidedWith != nullptr && &*collidedWith != &*this)
				{
					if (!collidedWith->isPassable())
					{
						canMove = false;
					}
				}
				
				if (getX() < 60 && canMove)
				{
					if (world->getIceManager()->clearIce(getX() + 1, getY()))
					{
						world->getPathFinder()->updateGrid();
						world->playSound(SOUND_DIG);
					}

					moveTo(getX() + 1, getY());
				}
				break;

			case KEY_PRESS_DOWN:
				if (getDirection() != down)
				{
					setDirection(down);
					break;
				}
				
				BB = BoundingBox(getX(), getY() - 1);
 				collidedWith = world->collisionWith(this, BB);

				if (collidedWith != nullptr && &*collidedWith != &*this)
				{
					if (!collidedWith->isPassable())
					{
						canMove = false;
					}
				}
				
				if (getY() > 0 && canMove)
				{
					if (world->getIceManager()->clearIce(getX(), getY() - 1))
					{
						world->getPathFinder()->updateGrid();
						world->playSound(SOUND_DIG);
					}

					moveTo(getX(), getY() - 1);
				}
				break;
			
			case KEY_PRESS_SPACE:
				if (m_numSquirts > 0)
				{
					world->playSound(SOUND_PLAYER_SQUIRT);
					m_numSquirts--;

					Direction dir = getDirection();

					int squirtX = getX() + (dir == left || dir == right ? (dir == left ? -4 : 4) : 0);
					int squirtY = getY() + (dir == down || dir == up ? (dir == down ? -4 : 4) : 0);
					
					if (squirtX < 0 || squirtX > 63 || squirtY < 0 || squirtY > 63)
					{
						break;
					}

					BB = BoundingBox(squirtX, squirtY);

					collidedWith = world->collisionWith(this, BB);

					if (collidedWith != nullptr)
					{
						if (!collidedWith->isPassable())
						{
							break;
						}
					}

					world->acceptActor(new Squirt(squirtX, squirtY, dir));
				}
				break;

			case KEY_PRESS_TAB:
				if (m_numGoldNuggets > 0)
				{
					world->acceptActor(new GoldNugget(getX(), getY(), Item::States::Temporary));

					m_numGoldNuggets--;
				}
				break;

			case KEY_PRESS_ESCAPE:
				
				world->hitESC();
				break;

			case 'Z':
			case 'z':
				if (m_numSonarKits > 0)
				{
					world->playSound(SOUND_SONAR);
					world->scan(getX() + 1.5, getY() + 1.5);
					m_numSonarKits--;
				}
				break;
			// For testing purposes
			case 'R':
			case 'r':

				//world->getPathFinder()->buildPaths();
				world->placePathTester(getX(), getY());

				break;
			}
		}
	}
	else
	{
		setDead();
	}
}


void Iceman::takeDamage(DamageSource src)
{
	if (src == protest)
	{
		decHealth(2);
	}
	else if (src == rockFall)
	{
		setDead();
	}
}

void Iceman::gotSonarKit()
{
	m_numSonarKits++;
}

void Iceman::gotGoldNugget()
{
	m_numGoldNuggets++;
}

void Iceman::gotWater()
{
	m_numSquirts += 5;
}

int Iceman::getNumSquirts()
{
	return m_numSquirts;
}

int Iceman::getNumSonarKits()
{
	return m_numSonarKits;
}

int Iceman::getNumGoldNuggets()
{
	return m_numGoldNuggets;
}

Iceman::~Iceman()
{

}


// Protester

Protester::Protester(int imageID, int health,
					 int x, int y) :	Actor(imageID, x, y, left, SIZE_NORMAL, 0, health, true, true),
										m_state(InOilField),
										m_nonShoutingActions(0),
										m_stunTicksLeft(0),
										m_ticksSinceAxisSwap(0),
										m_pathOut("")
{
	setVisible(true);
	m_restingTickCount = max(0, 3 - (int)getWorld()->getLevel() / 4);
	m_stepsInCurrDir = rand() % 53 + 8;
}

void Protester::doSomething()
{
	StudentWorld* world = getWorld();

	if (getTicksAlive() % m_restingTickCount == 0)
	{
		if (m_state == LeaveOilField)
		{

			if (getX() == 60 && getY() == 60)
			{
				setDead();
				return;
			}
			else
			{
				if (m_pathOut.empty())
				{
					m_pathOut = world->getPathFinder()->getPathToExitFrom(getX(), getY());
				}
				
				string temp_path = world->getPathFinder()->getPathToExitFrom(getX(), getY());

				if (temp_path.length() < m_pathOut.length())
				{
					m_pathOut = temp_path;
				}

				switch (m_pathOut[0])
				{
				case 'L':

					if (getDirection() != left)
					{
						setDirection(left);
					}

					moveTo(getX() - 1, getY());
					break;
				case 'U':

					if (getDirection() != up)
					{
						setDirection(up);
					}

					moveTo(getX(), getY() + 1);
					break;
				case 'R':

					if (getDirection() != right)
					{
						setDirection(right);
					}

					moveTo(getX() + 1, getY());
					break;
				case 'D':

					if (getDirection() != down)
					{
						setDirection(down);
					}

					moveTo(getX(), getY() - 1);
					break;
				case 'E':
					setDead();
					break;
				}

				m_pathOut = m_pathOut.substr(1);
				return;
			}
		}

		if (m_stunTicksLeft == 0)
		{
			/*	added additional bounding box to check for the player,
			*	because the protesters don't need to be shouting in the
			*	iceman's ear.
			*/

			BoundingBox BB = BoundingBox(getX() - 1, getY() - 1, 6);

			Actor* collidedWith = world->collisionWith(this, BB);
			Iceman* player = world->getPlayer();

			if (m_nonShoutingActions >= 15 && (*&collidedWith == *&player))
			{
				if (isFacing(collidedWith))
				{
					world->playSound(SOUND_PROTESTER_YELL);
					player->takeDamage(DamageSource::protest);

					m_nonShoutingActions = 0;
					return;
				}
			}
			else
			{
				m_nonShoutingActions++;
			}
			
			pathTowardsPlayer();
		}
		else
		{
			if (m_stunTicksLeft > 0)
			{
				m_stunTicksLeft--;
			}
			return;
		}
	}
}

Protester::States Protester::getState() const
{
	return m_state;
}

void Protester::takeDamage(DamageSource src)
{
	StudentWorld* world = getWorld();

	switch (src)
	{
	case DamageSource::protest:
		break;

	case DamageSource::rockFall:
		m_state = LeaveOilField;
		world->playSound(SOUND_PROTESTER_GIVE_UP);
		world->increaseScore(500);
		break;

	case DamageSource::waterSpray:
		decHealth(2);

		if (getHealth() <= 0)
		{
			world->playSound(SOUND_PROTESTER_GIVE_UP);
			m_state = LeaveOilField;
			world->increaseScore(getGiveUpPoints());
		}
		else
		{
			world->playSound(SOUND_PROTESTER_ANNOYED);
			m_stunTicksLeft = max(50, 100 - int(world->getLevel()) * 10);
		}
		break;
	}
}

Protester::~Protester()
{

}


// RegularProtester

RegularProtester::RegularProtester(int x, int y) : Protester(IID_PROTESTER, 5, x, y)
{

}

void RegularProtester::pathTowardsPlayer()
{
	if (getWorld()->getPathFinder()->hasUnobstructedPathToPlayer(this))
	{
		faceTowards(getWorld()->getPlayer());

		Direction dir = getDirection();
		
		moveTo(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
			   getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));

		m_stepsInCurrDir = 0;

		return;
	}
	else
	{
		Direction dir = getDirection();

		Point newXY = Point(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
							getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));

		string validDirs = getWorld()->getPathFinder()->getValidDirections(getBB().getXY());

		bool isXRoad = getWorld()->getPathFinder()->isIntersection(validDirs);

		if (m_stepsInCurrDir == 0 || !(newXY.isInBounds()))
		{
			char ch = validDirs[rand() % validDirs.length()];
			
			switch (ch)
			{
			case 'U':
				setDirection(up);
				break;
			case 'D':
				setDirection(down);
				break;
			case 'L':
				setDirection(left);
				break;
			case 'R':
				setDirection(right);
				break;
			}

			m_stepsInCurrDir = rand() % 52 + 8;
		}
		else if (isXRoad && m_ticksSinceAxisSwap >= 50)
		{
			validDirs = getWorld()->getPathFinder()->getValidPerpDirs(getBB().getXY(), dir);

			char ch = validDirs[rand() % validDirs.length()];
			
			switch (ch)
			{
			case 'U':
				setDirection(up);
				break;
			case 'D':
				setDirection(down);
				break;
			case 'L':
				setDirection(left);
				break;
			case 'R':
				setDirection(right);
				break;
			}

			m_stepsInCurrDir = rand() % 52 + 8;

			m_ticksSinceAxisSwap = 0;
		}
		
		dir = getDirection();
		
		newXY = Point(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
					  getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));
		
		if (validDirs.length() >= 1 && getWorld()->getIceManager()->checkIce(newXY.m_x, newXY.m_y))
		{	
			moveTo(newXY.m_x, newXY.m_y);

			m_stepsInCurrDir--;

			m_ticksSinceAxisSwap++;
		}
		else
		{
			m_stepsInCurrDir = 0;
		}
	}
}

void RegularProtester::foundGold()
{
	StudentWorld* world = getWorld();

	world->playSound(SOUND_PROTESTER_FOUND_GOLD);

	world->increaseScore(25);

	m_state = LeaveOilField;
}

int RegularProtester::getGiveUpPoints()
{
	return 100;
}

RegularProtester::~RegularProtester()
{

}


// HardcoreProtester

HardcoreProtester::HardcoreProtester(int x, int y) :	Protester(IID_HARD_CORE_PROTESTER, 20, x, y)
{
	m_maxPathSize = 16 + getWorld()->getLevel() * 2;
}

void HardcoreProtester::pathTowardsPlayer()
{
	StudentWorld* world = getWorld();

	m_pathToPlayer = world->getPathFinder()->getPathToPlayerFrom(getX(), getY());

	if (m_pathToPlayer.length() <= m_maxPathSize)
	{
		if (!m_pathToPlayer.empty())
		{
			char ch = m_pathToPlayer[0];

			switch (ch)
			{
			case 'L':

				if (getDirection() != left)
				{
					setDirection(left);
				}

				moveTo(getX() - 1, getY());

				break;

			case 'U':

				if (getDirection() != up)
				{
					setDirection(up);
				}

				moveTo(getX(), getY() + 1);

				break;

			case 'R':

				if (getDirection() != right)
				{
					setDirection(right);
				}

				moveTo(getX() + 1, getY());

				break;

			case 'D':

				if (getDirection() != down)
				{
					setDirection(down);
				}

				moveTo(getX(), getY() - 1);

				break;

			case 'E':

				setDead();

				break;
			}

			m_pathToPlayer = m_pathToPlayer.substr(1);
			return;
		}
	}
	else
	{
		m_pathToPlayer.clear();

		if (world->getPathFinder()->hasUnobstructedPathToPlayer(this))
		{
			faceTowards(getWorld()->getPlayer());

			Direction dir = getDirection();

			moveTo(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
				   getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));

			m_stepsInCurrDir = 0;

			return;
		}
		else
		{
			Direction dir = getDirection();

			Point newXY = Point(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
								getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));

			string validDirs = getWorld()->getPathFinder()->getValidDirections(getBB().getXY());

			bool isXRoad = getWorld()->getPathFinder()->isIntersection(validDirs);

			if (m_stepsInCurrDir == 0 || !(newXY.isInBounds()))
			{
				char ch = validDirs[rand() % validDirs.length()];

				switch (ch)
				{
				case 'U':
					setDirection(up);
					break;
				case 'D':
					setDirection(down);
					break;
				case 'L':
					setDirection(left);
					break;
				case 'R':
					setDirection(right);
					break;
				}

				m_stepsInCurrDir = rand() % 52 + 8;
			}
			else if (isXRoad && m_ticksSinceAxisSwap >= 50)
			{
				validDirs = getWorld()->getPathFinder()->getValidPerpDirs(getBB().getXY(), dir);

				char ch = validDirs[rand() % validDirs.length()];

				switch (ch)
				{
				case 'U':
					setDirection(up);
					break;
				case 'D':
					setDirection(down);
					break;
				case 'L':
					setDirection(left);
					break;
				case 'R':
					setDirection(right);
					break;
				}

				m_stepsInCurrDir = rand() % 52 + 8;

				m_ticksSinceAxisSwap = 0;
			}

			dir = getDirection();

			newXY = Point(getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0),
				getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0));

			if (validDirs.length() >= 1 && getWorld()->getIceManager()->checkIce(newXY.m_x, newXY.m_y))
			{
				moveTo(newXY.m_x, newXY.m_y);

				m_stepsInCurrDir--;

				m_ticksSinceAxisSwap++;
			}
			else
			{
				m_stepsInCurrDir = 0;
			}
		}
	}
}


void HardcoreProtester::foundGold()
{
	StudentWorld* world = getWorld();

	world->playSound(SOUND_PROTESTER_FOUND_GOLD);

}

int HardcoreProtester::getGiveUpPoints()
{
	return 250;
}

HardcoreProtester::~HardcoreProtester()
{

}


// Ice 

Ice::Ice(int startX, int startY) : Actor(IID_ICE, startX, startY, right, SIZE_SMALL, 3, 1, false, false)
{
	setVisible(true);
}

void Ice::doSomething() { return; }

void Ice::takeDamage(DamageSource src) { return; }

Ice::~Ice() 
{

}


// Boulder

Boulder::Boulder(int startX, int startY) :	Actor(IID_BOULDER, startX, startY, down, SIZE_NORMAL, 1, 1, false, false),
											m_isStable(true),
											m_isFalling(false)
{
	setVisible(true);
}

void Boulder::doSomething()
{
	StudentWorld* world = getWorld();

	if (world->getIceManager()->checkIce(getX(), getY() - 4) && m_isStable)
	{
		world->getIceManager()->clearIce(getX(), getY());

		m_isStable = false;
		m_ticksUnstable = 0;
	}
	
	if (!m_isStable && !m_isFalling)
	{
		if (m_ticksUnstable > 30)
		{
			world->playSound(SOUND_FALLING_ROCK); 
			m_isFalling = true;

			world->getPathFinder()->updateGrid();
		}
		else
		{
			m_ticksUnstable++;
		}
	}

	if (m_isFalling)
	{
		BoundingBox BB = BoundingBox(getX(), getY() - 1);

		Actor* collidedWith = world->collisionWith(this, BB);

		if (world->getIceManager()->checkIce(getX(), getY() - 1))
		{
			if (collidedWith != nullptr && &*collidedWith != &*this)
			{
				if (collidedWith->isDamageable())
				{
					collidedWith->takeDamage(rockFall);
					setDead();
				}
			}
			moveTo(getX(), getY() - 1);
		}
		else 
		{
			setDead();
		}
	}
}

void Boulder::takeDamage(DamageSource src) { return; }

Boulder::~Boulder()
{
	getWorld()->getIceManager()->clearIce(getX(), getY());
}


// Squirt

Squirt::Squirt(int startX, int startY, Direction dir) : Actor(IID_WATER_SPURT, startX, startY, dir, SIZE_NORMAL, 1, 1, false, true),
														m_movesLeft(4)
{
	setVisible(true);
}

void Squirt::doSomething()
{
	StudentWorld* world = getWorld();
	
	if (m_movesLeft > 0)
	{
		Direction dir = getDirection();

		int squirtX = getX() + (dir == left || dir == right ? (dir == left ? -1 : 1) : 0);
		int squirtY = getY() + (dir == down || dir == up ? (dir == down ? -1 : 1) : 0);

		BoundingBox BB = BoundingBox(squirtX, squirtY);

		if (!world->getIceManager()->checkIce(squirtX, squirtY) || world->squirtTargets(BB, this))
		{
			setDead();
			return;
		}

		moveTo(squirtX, squirtY);
		m_movesLeft--;
	}
	else
	{
		setDead();
	}
}

void Squirt::takeDamage(DamageSource src) { return; }

Squirt::~Squirt()
{

}


// Item

Item::Item(int imageID, int startX, int startY, 
		   Direction dir, double size, 
		   unsigned int depth, States state) :	Actor(imageID, startX, startY, dir, size, depth, 1, false, true),
												m_hasBeenPickedUp(false),
												m_state(state)
{

}

Item::~Item()
{

}

void Item::doSomething()
{
	ItemDoSomething();

	if (m_state == Temporary)
	{
		if (m_tempLifetime == getTicksAlive())
		{
			setDead();
		}
	}
}

bool Item::hasBeenPickedUp() const
{
	return m_hasBeenPickedUp;
}

void Item::setPickedUp()
{
	m_hasBeenPickedUp = true;
}

void Item::takeDamage(DamageSource src)
{
	return;
}

Item::States Item::getState() const
{
	return m_state;
}

void Item::setTempLifetime(int lifeTime)
{
	m_tempLifetime = lifeTime;
}

int Item::getTempTicksLeft()
{
	return m_tempLifetime;
}


// OilBarrel

OilBarrel::OilBarrel(int x, int y) : Item(IID_BARREL, x, y, right, SIZE_NORMAL, 2, Item::States::Permanent)
{
	setVisible(false);
	setTempLifetime();
}

OilBarrel::~OilBarrel()
{

}

void OilBarrel::ItemDoSomething()
{
	StudentWorld* world = getWorld();

	double x1 = world->getPlayer()->getX() + 1.5;
	double y1 = world->getPlayer()->getY() + 1.5;

	double distToPlayerSquared = world->getDistSquared(getX() + 1.5, getY() + 1.5, x1, y1);

	if (distToPlayerSquared <= 36.0 && !hasBeenPickedUp())
	{
		setVisible(true);
		setPickedUp();
		return;
	}
	
	if (getBB().intersects(world->getPlayer()->getBB()))
	{
		world->playSound(SOUND_FOUND_OIL);
		world->increaseScore(1000);

		world->decBarrelCount();

		setDead();
	}
}


// Gold Nugget

GoldNugget::GoldNugget(int x, int y, Item::States state) :	Item(IID_GOLD, x, y, right, SIZE_NORMAL, 2, state)
{
	setVisible(getState() == Permanent ? false : true);
	setTempLifetime();
}

GoldNugget::~GoldNugget()
{

}

void GoldNugget::ItemDoSomething()
{
	StudentWorld* world = getWorld();
	States state = getState();

	double x1 = world->getPlayer()->getX() + 1.5;
	double y1 = world->getPlayer()->getY() + 1.5;

	double distToPlayerSquared = world->getDistSquared(getX() + 1.5, getY() + 1.5, x1, y1);
		
	if (distToPlayerSquared <= 36.0 && !hasBeenPickedUp() && state == Permanent)
	{
		setVisible(true);
		setPickedUp();
		return;
	}
	
	if (getBB().intersects(world->getPlayer()->getBB()) && state == Permanent)
	{
		world->playSound(SOUND_GOT_GOODIE);
		world->increaseScore(10);

		world->getPlayer()->gotGoldNugget();

		setDead();
	}

	if (state == Temporary)
	{
		Actor* collidedWith = world->collisionWith(this, getBB());

		if (collidedWith != nullptr)
		{
			if (collidedWith->isDamageable() && &(*(world->getPlayer())) != &*collidedWith)
			{
				Protester* collidedWithProtester = dynamic_cast<Protester*>(collidedWith);

				if (collidedWithProtester->getState() == Protester::States::InOilField)
				{
					collidedWithProtester->foundGold();
					setDead();
				}
				return;
			}
		}
	}
}


// SonarKit

SonarKit::SonarKit() : Item(IID_SONAR, 0, 60, right, SIZE_NORMAL, 2, Item::States::Temporary)
{
	setVisible(true);

	int level = getWorld()->getLevel();

	setTempLifetime(std::max(100, 300 - level * 10));
}

SonarKit::~SonarKit()
{

}

void SonarKit::ItemDoSomething()
{
	StudentWorld* world = getWorld();

	Iceman* player = world->getPlayer();

	if (getBB().intersects(player->getBB()))
	{
		world->playSound(SOUND_GOT_GOODIE);
		world->increaseScore(75);

		player->gotSonarKit();
		
		setDead();
	}
}


// WaterPool

WaterPool::WaterPool(int x, int y) : Item(IID_WATER_POOL, x, y, right, SIZE_NORMAL, 2, Item::States::Temporary)
{
	setVisible(true);

	int level = getWorld()->getLevel();

	setTempLifetime(std::max(100, 300 - level * 10));
}

WaterPool::~WaterPool()
{

}

void WaterPool::ItemDoSomething()
{
	StudentWorld* world = getWorld();

	Iceman* player = world->getPlayer();

	if (getBB().intersects(player->getBB()))
	{
		world->playSound(SOUND_GOT_GOODIE);
		world->increaseScore(100);

		player->gotWater();
		
		setDead();
	}
}