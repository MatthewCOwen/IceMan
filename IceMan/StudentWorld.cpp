#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctype.h>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld* StudentWorld::world;

StudentWorld::StudentWorld(string assetDir) :	GameWorld(assetDir)
{
	world = this;
}

int StudentWorld::init() 
{
	m_currTime = static_cast<unsigned int>(time(nullptr));
	m_ticksThisSec = 0;

	int curLevel = getLevel();
	m_worldAge = 0;
	
	m_numMaxProtesters = min(15, 2 + (int)(curLevel * 1.5));
	m_numProtesters = 0;

	m_minTicksBetweenProtesterSpawn = max(25, 200 - curLevel);
	m_ticksSinceLastProtesterAdded = 0;

	m_itemSpawnChance = curLevel * 25 + 300;

	m_hitESC = false;

	bool isTooClose;
	bool isBlockingShaft;
	int x;
	int y;

	// Spawn Iceman
	m_player = new Iceman();

	// Spawn Ice
	m_ice = new IceManager();

	// Spawn Boulders
	int numBoulders = min(curLevel / 2 + 2, 9);
		
	for (int i = 0; i < numBoulders; i++)
	{
		do 
		{
			/*	generate random x and y coords to possibly place a boulder at.
				0 <= x <= 60
				20 <= y <= 56
			*/

			x = rand() % 61;
			y = 20 + rand() % 37;

			isTooClose = !isValidSpawnLocation(x, y);
			isBlockingShaft = x > 26 && x < 34;

			if (!isTooClose && !isBlockingShaft)
			{
				m_listActors.push_back(new Boulder(x, y));
				m_ice->clearIce(x, y);
			}

		} while (isTooClose || isBlockingShaft);
	}

	// Spawn Oil Barrels

	m_numOilBarrels = min(2 + curLevel, 21);

	for (int i = 0; i < m_numOilBarrels; i++)
	{
		do
		{
			/*	generate random x and y coords to possibly place a boulder at.
				0 <= x <= 60
				0 <= y <= 56
			*/

			x = rand() % 61;
			y = rand() % 57;

			isTooClose = !isValidSpawnLocation(x, y); 

			if (!isTooClose)
			{
				m_listActors.push_back(new OilBarrel(x, y));
			}
		} while (isTooClose);
	}

	// Spawn Gold Nuggets

	int numGoldNuggets = max(5 - curLevel / 2, 2);

	for (int i = 0; i < numGoldNuggets; i++)
	{
		do
		{
			/*	generate random x and y coords to possibly place a boulder at.
				0 <= x <= 60
				0 <= y <= 56
			*/

			x = rand() % 61;
			y = rand() % 57;

			isTooClose = !isValidSpawnLocation(x, y);

			if (!isTooClose)
			{
				m_listActors.push_back(new GoldNugget(x, y, Item::States::Permanent));
			}
		} while (isTooClose);
	}

	// Create PathFinder
	m_path = new PathFinder(this);	

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	unsigned int t = static_cast<unsigned int>(time(nullptr));

	if (m_currTime != t)
	{
		m_currTime = t;		
		
		system("cls");
		cout << m_ticksThisSec << endl;
		m_path->showPath();
		
		m_ticksThisSec = 0;
	}
	else
	{
		m_ticksThisSec++;
	}

	setGameStatText(getGameText());

	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	
	if (!m_player->isAlive() || m_hitESC)
	{
		//decLives();

		playSound(SOUND_PLAYER_GIVE_UP);

		return GWSTATUS_PLAYER_DIED;
	}
	else
	{
		if (m_numOilBarrels == 0)
		{
			return GWSTATUS_FINISHED_LEVEL;
		}

		if (m_worldAge == 0 || m_ticksSinceLastProtesterAdded == m_minTicksBetweenProtesterSpawn)
		{
			if (rand() % m_chanceHardcore == 0)
			{
				m_listActors.push_back(new HardcoreProtester());
			}
			else
			{
				m_listActors.push_back(new RegularProtester());
			}

			m_ticksSinceLastProtesterAdded = 0;
		}
		else
		{
			m_ticksSinceLastProtesterAdded++;
		}

		if (rand() % m_itemSpawnChance == 0)
		{
			spawnGoodie();
		}
		
		m_player->move();
		
		for (unsigned int i = 0; i < m_listActors.size(); i++)
		{
			m_listActors[i]->move();
		}

		for (unsigned int i = 0; i < m_listActors.size(); i++)
		{
			if (!m_listActors[i]->isAlive())
			{
				m_listActors[i]->setDead();
				Actor* temp = m_listActors.back();
				m_listActors.back() = m_listActors[i];
				m_listActors[i] = temp;

				delete m_listActors.back(), m_listActors.pop_back();
			}
		}
	}

	m_worldAge++;
	
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	while (!m_listActors.empty())
	{
		delete m_listActors.back(), m_listActors.pop_back();
	}

	delete m_player;
	delete m_ice;
}

string StudentWorld::getGameText()
{
	ostringstream oss;

	oss << "Lvl: " << setw(2) << getLevel() << "  "
		<< "Lives: " << setw(1) << getLives() << "  "
		<< "Hlth: " << setw(2) << m_player->getHealth() * 10 << '%' << "  "
		<< "Wtr: " << setw(2) << m_player->getNumSquirts() << "  "
		<< "Gld: " << setw(2) << m_player->getNumGoldNuggets() << "  "
		<< "Oil Left: " << setw(2) << m_numOilBarrels << "  "
		<< "Sonar: " << setw(2) << m_player->getNumSonarKits() << "  "
		<< "Scr: " << setfill('0') << setw(6) << getScore() << setfill(' ');

	return oss.str();
}

StudentWorld* StudentWorld::getInstance()
{
	return StudentWorld::world;
}

Actor* StudentWorld::collisionWith(BoundingBox BB)
{
	if (BB.intersects(m_player->getBB()) && BB != m_player->getBB())
	{
		return m_player;
	}

	if (!m_listActors.empty())
	{
		for (unsigned int i = 0; i < m_listActors.size(); i++)
		{
			BoundingBox other = m_listActors[i]->getBB();

			if (BB.intersects(other) && other.getXY() != BB.getXY())
			{
				return m_listActors[i];
			}
		}
	}

	return nullptr;
}

Iceman* StudentWorld::getPlayer()
{
	return m_player;
}

void StudentWorld::acceptActor(Actor* a)
{
	m_listActors.push_back(a);
}

void StudentWorld::placePathTester(int x, int y)
{
	m_listActors.push_back(new RegularProtester(x, y));

	m_listActors.back()->takeDamage(Actor::DamageSource::rockFall);
}

void StudentWorld::hitESC()
{
	m_hitESC = true;
}

void StudentWorld::spawnGoodie()
{
	if (rand() % 5 == 0)
	{
		m_listActors.push_back(new SonarKit());
	}
	else
	{
		bool noConflict;

		do 
		{
			int x = rand() % 61;
			int y = rand() % 61;

			if ((noConflict = m_ice->checkIce(x, y)))
			{
				m_listActors.push_back(new WaterPool(x, y));
			}

		} while (!noConflict);
	}
}

void StudentWorld::scan(double x, double y)
{
	for (unsigned int i = 0; i < m_listActors.size(); i++)
	{
		int X = m_listActors[i]->getX();
		int Y = m_listActors[i]->getY();

		if (!m_listActors[i]->isVisible() && getDistSquared(x, y, X, Y) <= 144)
		{
			m_listActors[i]->setVisible(true);
		}
	}
}

bool StudentWorld::squirtTargets(BoundingBox BB, Squirt* blackList)
{
	bool hitTargets = false;

	for (unsigned int i = 0; i < m_listActors.size(); i++)
	{
		if (BB.intersects(m_listActors[i]->getBB()) && &*m_listActors[i] != &*blackList)
		{
			if (m_listActors[i]->isDamageable())
			{
				m_listActors[i]->takeDamage(Actor::DamageSource::waterSpray);
			}
			hitTargets = true;
		}
	}

	return hitTargets;
}

bool StudentWorld::hasLOSToPlayer(Actor* a) const
{
	int a_x = a->getX();
	int a_y = a->getY();
	
	int p_x = m_player->getX();
	int p_y = m_player->getY();

	bool hasLOS = false;

	for (int i = a_x; i < a_x + 4 && !hasLOS; i++)
	{
		if (i >= p_x || i < p_x + 4)
		{
			hasLOS = true;
		}
	}

	for (int j = a_y; j < a_y + 4 && !hasLOS; j++)
	{
		if (j >= p_y || j < p_y + 4)
		{
			hasLOS = true;
		}
	}

	return hasLOS;
}

bool StudentWorld::isValidSpawnLocation(int x, int y) 
{
	bool isValid;

	if (!m_listActors.empty())
	{
		for (unsigned int i = 0; i < m_listActors.size(); i++)
		{
			//hacky way of getting the center coordinates of an actor.
			isValid = getDistSquared(x, y, m_listActors[i]->getX() + 1.5, m_listActors[i]->getY() + 1.5) >= 36;
			
			if (!isValid)
			{
				return false;
			}
		}
	}
	return true;
}

double StudentWorld::getDistSquared(double x1, double y1, double x2, double y2) 
{
	return abs((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int StudentWorld::getDistSquared(Point p1, Point p2)
{
	return abs((p2.getX()- p1.getX()) * (p2.getX()- p1.getX()) + (p2.getY()- p1.getY()) * (p2.getY() - p1.getY()));
}

IceManager* StudentWorld::getIceManager()
{
	return m_ice;
}

PathFinder* StudentWorld::getPathFinder()
{
	return m_path;
}

void StudentWorld::decBarrelCount()
{
	m_numOilBarrels--;
}

StudentWorld::~StudentWorld()
{
	while (!m_listActors.empty())
	{
		delete m_listActors.back(), m_listActors.pop_back();
	}

	delete m_player;
	delete m_ice;
}


// IceManager

IceManager::IceManager()
{
	m_arr = new Ice*[4096];
	for (int i = 0; i < 4096; i++)
	{
		m_arr[i] = nullptr;
	}

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			m_arr[64 * y + x] = new Ice(x, y);
		}
	}

	for (int y = 4; y < 60; y++)
	{
		for (int x = 0; x < 30; x++)
		{
			m_arr[64 * y + x] = new Ice(x, y);
		}
		
		for (int x = 34; x < 64; x++)
		{
			m_arr[64 * y + x] = new Ice(x, y);
		}
	}
}
		
Ice*& IceManager::operator() (int x, int y)
{
	return m_arr[64 * y + x];
}

bool IceManager::clearIce(int x, int y)
{
	if (y + 3 > 63 || y < 0)
	{
		return false;
	}

	bool didMine = false;

	for (int y1 = y; y1 < (y + 4); y1++)
	{
		for (int x1 = x; x1 < (x + 4); x1++)
		{
			if (x1 < 64 && x1 >= 0)
			{
				Ice* temp = m_arr[64 * y1 + x1];

				if (temp != nullptr && !didMine)
				{
					didMine = true;
				}

				delete temp;
				m_arr[64 * y1 + x1] = nullptr;
			}
		}
	}
	return didMine;
}

bool IceManager::checkIce(int x, int y)
{
	for (int Y = y; Y < y + 4; Y++)
	{
		for (int X = x; X < x + 4; X++)
		{
			if (m_arr[64 * Y + X] != nullptr)
				return false;
		}
	}
	return true;
}

IceManager::~IceManager()
{
	for (int i = 0; i < 4096; i++)
	{
		if (m_arr[i] != nullptr)
		{
			/*
			Ice * temp = &(*m_arr[i]);
			m_arr[i] = nullptr;
			delete temp;
			*/

			delete m_arr[i];
		}
	}

	delete[] m_arr;
}


// PathFinder

PathFinder::PathFinder(StudentWorld* world) : m_world(world)
{
	for (int y = 0; y < 64; y++)
		for (int x = 0; x < 64; x++)
			m_grid[x][y] = '_';

	for (int y = 4; y < 61; y++)
	{
		for (int Y = y; Y < y + 4; Y++)
		{
			for (int x = 30; x < 34; x++)
			{
				m_grid[x][63 - Y] = '.';
			}
		}

		m_grid[30][63 - y] = 'U';
	}

	for (int x = 0; x < 61; x++)
	{
		for (int y = 60; y < 64; y++)
		{
			for (int X = x; X < x + 4; X++)
			{
				m_grid[X][63 - y] = '.';
			}
		}
		
		m_grid[x][63 - 60] = 'R';
	}

	m_grid[60][63 - 60] = 'E';
}

void PathFinder::updatePath(int x, int y, GraphObject::Direction dir)
{
	for (int Y = y; Y < y + 4; Y++)
	{
		for (int X = x; X < x + 4; X++)
		{
			if (!isalpha(m_grid[X][63 - Y]))
			{
				m_grid[X][63 - Y] = '.';
			}
		}
	}

	if (dir != GraphObject::Direction::none)
	{
		switch (dir)
		{
		case GraphObject::Direction::left:
			m_grid[x][63 - y] = 'R';
			break;
		case GraphObject::Direction::up:
			m_grid[x][63 - y] = 'D';
			break;
		case GraphObject::Direction::right:
			m_grid[x][63 - y] = 'L';
			break;
		case GraphObject::Direction::down:
			m_grid[x][63 - y] = 'U';
			break;
		}

		fixIssues(x, y);
	}
}

void PathFinder::fixIssues(int x, int y)
{
	if (!isValidPath(x, y))
	{
		if (isValidPath(x - 1, y))
		{
			m_grid[x][63 - y] = 'L';
		}
		else if (isValidPath(x, y + 1))
		{
			m_grid[x][63 - y] = 'U';
		}
		else if (isValidPath(x + 1, y))
		{
			m_grid[x][63 - y] = 'R';
		}
		else if (isValidPath(x, y - 1))
		{
			m_grid[x][63 - y] = 'D';
		}
		else
		{
			if (getNonPathCharCount(x, y) > 6)
			{
				GraphObject::Direction dir = getValidDirection(x, y);

				int x_increase = (dir == GraphObject::Direction::left || 
									dir == GraphObject::Direction::right ? (dir == GraphObject::Direction::left ? -1 : 1) : 0);
				int y_increase = (dir == GraphObject::Direction::down || 
									dir == GraphObject::Direction::up ? (dir == GraphObject::Direction::down ? -1 : 1) : 0);
				char ch;

				switch (dir)
				{
				case GraphObject::Direction::left:
					ch = 'L';
					break;
				case GraphObject::Direction::up:
					ch = 'U';
					break;
				case GraphObject::Direction::right:
					ch = 'R';
					break;
				case GraphObject::Direction::down:
					ch = 'D';
					break;
				}

				do
				{
					m_grid[x][63 - y] = ch;
						
					x += x_increase;
					y += y_increase;
						
				} while (!isalpha(m_grid[x][63 - y]));
			}
			else
			{
				switch (m_grid[x][63 - y])
				{
				case 'L':
					fixIssues(x - 1, y);
					break;
				case 'U':
					fixIssues(x, y + 1);
					break;
				case 'R':
					fixIssues(x + 1, y);
					break;
				case 'D':
					fixIssues(x, y - 1);
					break;
				}
			}
		}
	}
}

bool PathFinder::isValidPath(int x, int y)
{
	switch (m_grid[x][63 - y])
	{
	case 'U':
		return isValidPath(x, ++y);
		break;
	case 'R':
		return isValidPath(++x, y);
		break;
	case 'D':
		return isValidPath(x, --y);
		break;
	case 'L':
		return isValidPath(--x, y);
		break;
	case 'E':
		return true;
		break;
	default:
		return false;
		break;
	}
}

int PathFinder::getNonPathCharCount(int x, int y)
{
	int count = 0;
	int startX = x > 0 ? x - 1 : x;
	int startY = y > 0 ? y - 1 : y;

	for (int j = startY; j < y + 2; j++)
	{
		for (int i = startX; i < x + 2; i++)
		{
			if (!isalpha(m_grid[i][63 - j]))
			{
				count++;
			}
		}
	}

	return count - 1;
}

void PathFinder::showPath()
{
	cout << '\t';

	for (int x = 0; x < 64; x++)
	{
		cout << x / 10;
	}
	cout << endl;

	cout << '\t';

	for (int x = 0; x < 64; x++)
	{
		cout << x % 10;
	}
	
	cout << endl;

	for (int y = 0; y < 64; y++)
	{
		cout << 63 - y << '\t';

		for (int x = 0; x < 64; x++)
		{
			cout << m_grid[x][y];
		}

		cout << endl;
	}
}

void PathFinder::getClosestPath(int& x, int& y)
{
	if (!isValidPath(x, y))
	{
		for (int Y = y; Y < y + 4; Y++)
		{
			for (int X = x; X < x + 4; X++)
			{
				if (isValidPath(X, Y))
				{
					x = X;
					y = Y;
					return;
				}
			}
		}
	}
}

GraphObject::Direction PathFinder::getValidDirection(int x, int y)
{
	int X = x;
	int Y = y;

	do
	{
		if (isalpha(m_grid[--X][63 - y]))
		{
			return GraphObject::Direction::left;
		}
	} while (X > 0 && m_grid[X][63 - Y] != '_');

	X = x;

	do
	{
		if (isalpha(m_grid[X][63 - ++y]))
		{
			return GraphObject::Direction::up;
		}
	} while (Y < 63 && m_grid[X][63 - Y] != '_');

	Y = y;

	do
	{
		if (isalpha(m_grid[++X][63 - Y]))
		{
			return GraphObject::Direction::right;
		}
	} while (X < 63 && m_grid[X][63 - Y] != '_');

	X = x;

	do
	{
		if (isalpha(m_grid[X][63 - --Y]))
		{
			return GraphObject::Direction::down;
		}
	} while (Y > 0 && m_grid[X][63 - Y] != '_');

	return GraphObject::Direction::none;
}

const string PathFinder::getPathFrom(int x, int y)
{
	int X = x;
	int Y = y;

	getClosestPath(X, Y);

	ostringstream oss;

	if (X != x)
	{
		for (int i = 0; i < X - x; i++)
		{
			oss << 'R';
		}
	}

	if (Y != y)
	{
		for (int i = 0; i < Y - y; i++)
		{
			oss << 'U';
		}
	}

	do
	{
		switch (m_grid[X][63 - Y])
		{
		case 'L':
			oss << m_grid[X--][63 - Y];
			break;
		case 'U':
			oss << m_grid[X][63 - Y++];
			break;
		case 'R':
			oss << m_grid[X++][63 - Y];
			break;
		case 'D':
			oss << m_grid[X][63 - Y--];
			break;
		}
	} while (m_grid[X][63 - Y] != 'E');

	return oss.str();
}

