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
#include <string>
#include <sstream>
#include <time.h>


using namespace std;



class Warrior
{
public:
	enum Status { SEARCHING_FOR_MEDICINE, SEARCHING_FOR_AMMO, SEARCHING_FOR_ENEMY, IN_MOVEMENT, IN_BATTLE };
	string statusString[5] = { "SEARCHING FOR MEDICINE", "SEARCHING FOR AMMO", "SEARCHING FOR ENEMY",
		"IN MOVEMENT", "IN BATTLE" };

	enum Behavior {OFFENSIVE, DEFFENSIVE};
	string behaviorString[2] = { "offensive", "deffensive" };

    ~Warrior();
private:

	/***********************		ATTRIBUTES			************************/

	int astartCount = 0;
	Status status;
	string message;
	string specs;
	Behavior behavior;

    int ammoCounter, medicineCounter, hp;
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
	vector<Room*> visitedRooms;

	bool noMoreAmmoInGame = false;
	bool noMoreMedicineInGame = false;

	/***********************		METHODS			************************/

	bool AstarSearch(Point2D& targetPoint);

    void fightAgainstEnemy();

	bool isBfsFoundPath(int row, int col, int goalPoint);

	void storeCurrentPointForAstar(int row, int col, Point2D * parentPoint, Point2D* targetPoint);

	void setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, Point2D* targetPoint);

	void savePath(Point2D * pt, Point2D* targetPoint);

	void clearwarriorMaze();

	void clearAllPathes();

	//calculate the amount of empty (SPACE) pixels around the warrior and return percentage [0,1]
	double getEmptyPixelsPercentageAround();

	void insertVisitedRoom();

	Point2D& findTheFarestCornerFromEnemyInRoom(Warrior& enemy);

	void offensiveDesicions(Warrior& enemy);

	void deffensiveDesicions(Warrior& enemy);


public:
    Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom);
    
	void searchForTarget(Point2D& medicinePoint, int goalPointNumber);

    void addAmmo(int amount);
    void addMedicine(int amount);

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
	void setNotInRoom();
	Room& getCurrentRoom();
	Room& getPreviousRoomVisitet();

	bool checkWarriorsInTheSameRoom(Warrior& otherWarrior);

	//calculate own damage in case that enemy hits, depends on distance between warriors and surrounding environment
	int calcPotentialDamage(Warrior& enemy);

	void makeDecision(Warrior& enemy);
	bool shootEnemy(Warrior& warrior);

	int getWarriorMazeColor();

	friend ostream& operator<<(ostream& os, Warrior& warrior);

	void setNoMoreAmmoInGame(bool noMoreAmmo);
	void setNoMoreMedicineInGame(bool noMoreMedicine);
	void addSpecsToMessage();
	string getMessage();
	void addToMessage(string message);

};
