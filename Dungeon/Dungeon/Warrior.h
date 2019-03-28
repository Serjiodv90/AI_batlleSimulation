//
//  Warrior.hpp
//  DungeonProject
//


#include "Point2D.h"
#include "ComparePointsByDist.h"
#include "Definitions.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <iterator>
#include "Room.h"
#include <utility>

using namespace std;



class Warrior
{
public:
	enum Status { SEARCHING_FOR_MEDICINE, SEARCHING_FOR_AMMO, SEARCHING_FOR_OPPONENT, IN_MOVEMENT };
    ~Warrior();
private:

	Status status;

    int ammoCounter, medicineCounter, lifeCounter;
    int behaviour;
    Point2D warriorLocation;
	
	vector<Point2D*> path;
	priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist> grayPQ;
	Point2D* parentPointsForPath[MSIZE][MSIZE];
	vector<vector<int>> warriorMaze;
	vector<vector<int>>* gameMaze;
	vector<int>* warriorColors;	// 0 : warrior color in the maze, 1 : warrior's path color, 2 : warrior's visited color
	
	Point2D* targetPoint;
	Status targetPointType;
	Room* currentRoom;

	bool AstarSearch(Point2D& targetPoint, int goalPointNumber);

    void fightAgainstEnemy();

	bool isBfsFoundPath(int row, int col, int goalPoint);

	void storeCurrentPointForAstar(int row, int col, Point2D * parentPoint);

	void setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, int goalPointNumber);

	void savePath(Point2D * pt, int goalPoint);

	void clearwarriorMaze();

public:
    Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom);
    
    void searchEnemy(const Warrior& enemy);
    void escapeFromEnemy(const Warrior& enemy);
    void searchMedicine(Point2D& medicinePoint, int goalPointNumber);

    void searchAmmo(Point2D& ammoPoint, int goalPointNumber);
    
    void addAmmo();
    void addMedicine();

    int getAmmo();
    int getMedicine();
    int getLife();

    void useMedicine();
    void shoot();
    void getHurt(int damage);
    
    Point2D& getWarriorLocation();
    void setWarriorLocation(Point2D& location);
	vector<vector<int>>& getWarriorMaze();
	void moveWarriorByOne();

	Status getWarriorStatus();
	void setWarriorStatus(Status status);

	Status getPreviousTargetPointType();
	Point2D& getPreviousTargetPoint();
	void setTragetPoint(Point2D& target);

	void setCurrentRoom(Room& room);
	Room& getCurrentRoom();

	bool checkWarriorsInTheSameRoom(Warrior& otherWarrior);
};
