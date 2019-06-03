#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <queue>
#include <ctype.h>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld* StudentWorld::world;

StudentWorld::StudentWorld(string assetDir) : GameWorld(assetDir)
{
	world = this;

	m_player = nullptr;
	
	m_ice = nullptr;
}

int StudentWorld::init() 
{
	int curLevel = getLevel();
	m_worldAge = 0;
	
	m_numMaxProtesters = min(15, 2 + (int)(curLevel * 1.5));
	m_numProtesters = 0;

	m_minTicksBetweenProtesterSpawn = max(25, 200 - curLevel);
	m_ticksSinceLastProtesterAdded = 0;

	m_itemSpawnChance = curLevel * 25 + 300;

	m_chanceHardcore = min(90, curLevel * 10 + 30);

	m_hitESC = false;

	bool isTooClose;
	bool isBlockingShaft;
	int x;
	int y;

	// Spawn Iceman
	m_player = new Iceman();

	// Spawn Ice
	m_ice = new IceManager();

	// Create PathFinder
	m_path = new PathFinder(this);

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
				m_ice->hideIce(x, y);
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
		

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	auto t1 = Clock::now();

	setGameStatText(getGameText());

	if (!m_player->isAlive() || m_hitESC)
	{
		decLives();

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
			if (rand() % 100 <= m_chanceHardcore)
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

				delete m_listActors.back();
				m_listActors.pop_back();
			}
		}
	}

	m_worldAge++;
	chrono::duration<double> span;

	do
	{
		auto t2 = Clock::now();
		span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	} 
	while (span.count() < 0.05);

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	while (!m_listActors.empty())
	{
		delete m_listActors.back();
		m_listActors.pop_back();
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

Actor* StudentWorld::collisionWith(Actor * a, BoundingBox BB)
{
	if (&*a != &*m_player && BB.intersects(m_player->getBB()))
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
	m_listActors.push_back(new HardcoreProtester(x, y));

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

int StudentWorld::getDistSquared(double x1, double y1, double x2, double y2) 
{
	return abs((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int StudentWorld::getDistSquared(Point p1, Point p2)
{
	return abs((p2.m_x- p1.m_x) * (p2.m_x - p1.m_x) + (p2.m_y- p1.m_y) * (p2.m_y - p1.m_y));
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
	
	if (m_player != nullptr)
	{
		delete m_player;
	}

	if (m_ice != nullptr)
	{
		delete m_ice;
	}

	/*
		This feels really hacky, but it stops the read-access violations that
		would happen if you hit 'q' at the title screen. Without this fix,
		this destructor tries to delete uninitialized pointers.
	
	if ((unsigned int)(void *)(m_player) != 0xCDCDCDCD)
	{
		delete m_player;
	}

	if ((unsigned int)(void *)(m_ice) != 0xCDCDCDCD)
	{
		delete m_ice;
	}	
	*/
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
	if (y > 60)
	{
		return false;
	}

	bool didMine = false;

	for (int j = y; j < (y + 4); j++)
	{
		for (int i = x; i < (x + 4); i++)
		{
			if (i < 64 && i >= 0)
			{
				Ice* temp = m_arr[64 * j + i];

				if (temp != nullptr)
				{
					didMine = true;
					
					delete temp;
					
					m_arr[64 * j + i] = nullptr;
				}
			}
		}
	}
	return didMine;
}

bool IceManager::checkIce(int x, int y)
{
	if (x > 60 || y > 60)
	{
		return false;
	}

	for (int j = y; j < y + 4; j++)
	{
		for (int i = x; i < x + 4; i++)
		{
			if (m_arr[64 * j + i] != nullptr)
			{
				return false;
			}
		}
	}
	return true;
}

void IceManager::hideIce(int x, int y)
{
	for (int j = y; j < y + 4; j++)
	{
		for (int i = x; i < x + 4; i++)
		{
			m_arr[64 * j + i]->setVisible(false);
		}
	}
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

			m_arr[i] = nullptr;
		}
	}

	delete[] m_arr;
}


// PathFinder

PathFinder::PathFinder(StudentWorld* world) : m_world(world), m_needsUpdating(true)
{
	for (int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (m_world->getIceManager()->checkIce(x, y))
			{
				m_pathToExit[x][y] = '#';
			}
			else
			{
				m_pathToExit[x][y] = ' ';
			}
		}
	}

	int x = 60;
	int y = 60;

	m_pathToExit[x][y] = 'E';

	maxPathLength = world->getLevel();
}

void PathFinder::updateGrid()
{
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			if (!isalpha(m_pathToExit[x][y]) && 
				m_world->getIceManager()->checkIce(x, y))
			{
				m_pathToExit[x][y] = '#';
				m_needsUpdating = true;
			}
		}
	}
}

void PathFinder::buildPathToExit()
{
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			if (isalpha(m_pathToExit[x][y]))
			{
				m_pathToExit[x][y] = '#';
			}
		}
	}

	queue<Point> q;
	Point* adjPoints;

	// start from the exit point for protesters
	Point start = Point(60, 60);
	
	m_pathToExit[start.m_x][start.m_y] = 'E';

	q.push(start);

	while (!q.empty())
	{
		Point current = q.front();
		q.pop();

		adjPoints = getValidAdjPoints(current);

		if (adjPoints[GraphObject::Direction::left].isValid())
		{
			m_pathToExit[adjPoints[GraphObject::Direction::left].m_x][adjPoints[GraphObject::Direction::left].m_y] = 'R';
			q.push(adjPoints[GraphObject::Direction::left]);
		}

		if (adjPoints[GraphObject::Direction::up].isValid())
		{
			m_pathToExit[adjPoints[GraphObject::Direction::up].m_x][adjPoints[GraphObject::Direction::up].m_y] = 'D';
			q.push(adjPoints[GraphObject::Direction::up]);
		}

		if (adjPoints[GraphObject::Direction::right].isValid())
		{
			m_pathToExit[adjPoints[GraphObject::Direction::right].m_x][adjPoints[GraphObject::Direction::right].m_y] = 'L';
			q.push(adjPoints[GraphObject::Direction::right]);
		}

		if (adjPoints[GraphObject::Direction::down].isValid())
		{
			m_pathToExit[adjPoints[GraphObject::Direction::down].m_x][adjPoints[GraphObject::Direction::down].m_y] = 'U';
			q.push(adjPoints[GraphObject::Direction::down]);
		}
	}

	m_needsUpdating = false;

	delete[] adjPoints;
}

bool PathFinder::hasUnobstructedPathToPlayer(Actor* a)
{
	int a_x = a->getX();
	int a_y = a->getY();
	
	int p_x = m_world->getPlayer()->getX();
	int p_y = m_world->getPlayer()->getY();

	if (a_x == p_x || a_y == p_y)
	{
		if (a_y == p_y)
		{
			int start = a_x < p_x ? a_x : p_x;
			int end = start == a_x ? p_x : a_x;

			char ch;

			for (int i = start; i < end; i++)
			{
				ch = m_pathToExit[i][a_y];

				if (!(isalpha(ch) || ch == '#'))
				{
					return false;
				}
			}
		}
		else
		{
			int start = a_y < p_y ? a_y : p_y;
			int end = start == a_y ? p_y : a_y;

			char ch;

			for (int j = start; j < end; j++)
			{
				ch = m_pathToExit[a_x][j];

				if (!(isalpha(ch) ||  ch == '#'))
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

Point* PathFinder::getValidAdjPoints(const Point p)
{
	// array is of size 5 because I chose to use literal enum 
	// values in order to determine the direction the path
	// needs to follow.

	char m_grid[64][64];

	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			m_grid[x][y] = m_pathToExit[x][y];
		}
	}

	Point* adjPoints = new Point[5];

	Point temp;
	
	temp = p.getAdjLeft();

	if (temp.isValid() && m_grid[temp.m_x][temp.m_y] == '#')
	{
		adjPoints[GraphObject::Direction::left] = temp;
	}

	temp = p.getAdjUp();
	
	if (temp.isValid() && m_grid[temp.m_x][temp.m_y] == '#')
	{
		adjPoints[GraphObject::Direction::up] = temp;
	}

	temp = p.getAdjRight();

	if (temp.isValid() && m_grid[temp.m_x][temp.m_y] == '#')
	{
		adjPoints[GraphObject::Direction::right] = temp;
	}

	temp = p.getAdjDown();

	if (temp.isValid() && m_grid[temp.m_x][temp.m_y] == '#')
	{
		adjPoints[GraphObject::Direction::down] = temp;
	}

	return adjPoints;
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
			cout << m_pathToExit[x][63 - y];
		}

		cout << endl;
	}
}

bool PathFinder::isValidLocation(int x, int y)
{
	return m_pathToExit[x][y] != ' ';
}

const string PathFinder::getPathToExitFrom(int x, int y)
{
	if (m_needsUpdating)
	{
		buildPathToExit();
	}

	ostringstream oss;

	do
	{
		switch (m_pathToExit[x][y])
		{
		case 'L':
			oss << m_pathToExit[x--][y];
			break;
		case 'U':
			oss << m_pathToExit[x][y++];
			break;
		case 'R':
			oss << m_pathToExit[x++][y];
			break;
		case 'D':
			oss << m_pathToExit[x][y--];
			break;
		}
	} while (m_pathToExit[x][y] != 'E');

	return oss.str();
}

const string PathFinder::getValidDirections(Point p)
{
	string retVal = "";
	Point temp;
	char ch;
	
	temp = p.getAdjUp();
	if (temp.isValid())
	{
		ch = m_pathToExit[temp.m_x][temp.m_y];

		if (isalpha(ch) || ch == '#')
		{
			retVal += 'U';
		}
	}

	temp = p.getAdjDown();
	if (temp.isValid())
	{
		ch = m_pathToExit[temp.m_x][temp.m_y];

		if (isalpha(ch) || ch == '#')
		{
			retVal += 'D';
		}
	}

	temp = p.getAdjLeft();
	if (temp.isValid())
	{
		ch = m_pathToExit[temp.m_x][temp.m_y];

		if (isalpha(ch) || ch == '#')
		{
			retVal += 'L';
		}
	}

	temp = p.getAdjRight();
	if (temp.isValid())
	{
		ch = m_pathToExit[temp.m_x][temp.m_y];

		if (isalpha(ch) || ch == '#')
		{
			retVal += 'R';
		}
	}

	return retVal;
}

const string PathFinder::getValidPerpDirs(Point p, GraphObject::Direction dir)
{
	string retVal = "";

	string validDirs = getValidDirections(p);

	int pos;

	switch (dir)
	{
	case GraphObject::Direction::up:
	case GraphObject::Direction::down:
		
		pos = validDirs.find('L');

		if (pos != string::npos)
		{
			retVal += 'L';
		}

		pos = validDirs.find('R');

		if (pos != string::npos)
		{
			retVal += 'R';
		}

		break;
	
	case GraphObject::Direction::left:
	case GraphObject::Direction::right:

		pos = validDirs.find('U');

		if (pos != string::npos)
		{
			retVal += 'U';
		}

		pos = validDirs.find('D');
		
		if (pos != string::npos)
		{
			retVal += 'D';
		}

		break;
	}

	return retVal;
}

GraphObject::Direction PathFinder::getAdjPointClosestToPlayer(	Point &p, 
																GraphObject::Direction cur)
{
	StudentWorld* world = StudentWorld::getInstance();
	Iceman* player = world->getPlayer();

	Point temp;

	char ch;

	GraphObject::Direction retVal = cur;

	string protesterToExit = getPathToExitFrom(p.m_x, p.m_y);
	string playerToExit = getPathToExitFrom(player->getX(), player->getY());

	while (!protesterToExit.empty() && !playerToExit.empty())
	{
		if (protesterToExit.back() == playerToExit.back())
		{
			protesterToExit.pop_back();
			playerToExit.pop_back();
		}
		else
		{
			break;
		}
	} 

	int pathLength = protesterToExit.length() + playerToExit.length();

	if (pathLength < maxPathLength)
	{
		ch = protesterToExit.length() > playerToExit.length() ? protesterToExit.front() : playerToExit.back();
	}
	else
	{
		return GraphObject::Direction::none;
	}

	switch (ch)
	{
	case 'D':
		retVal = GraphObject::Direction::up;
		p.m_y++;
		break;
	case 'U':
		retVal = GraphObject::Direction::down;
		p.m_y--;
		break;
	case 'R':
		retVal = GraphObject::Direction::left;
		p.m_x--;
		break;
	case 'L':
		retVal = GraphObject::Direction::right;
		p.m_x++;
		break;
	}

	return retVal;
}

bool PathFinder::isIntersection(const string s)
{
	return ((s.find('U') != string::npos || s.find('D') != string::npos) && 
			(s.find('L') != string::npos || s.find('R') != string::npos));
}