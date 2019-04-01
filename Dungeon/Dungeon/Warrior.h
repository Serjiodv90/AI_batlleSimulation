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

using namespace std;



class Warrior
{
public:
	enum Status { SEARCHING_FOR_MEDICINE, SEARCHING_FOR_AMMO, SEARCHING_FOR_ENEMY, IN_MOVEMENT, IN_BATTLE };
	string statusString[5] = { "SEARCHING FOR MEDICINE", "SEARCHING FOR AMMO", "SEARCHING FOR ENEMY",
		"IN MOVEMENT", "IN BATTLE" };

    ~Warrior();
private:

	int astartCount = 0;
	Status status;
	string message;

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

	bool noMoreAmmoInGame = false;
	bool noMoreMedicineInGame = false;

	bool AstarSearch(Point2D& targetPoint, int goalPointNumber);

    void fightAgainstEnemy();

	bool isBfsFoundPath(int row, int col, int goalPoint);

	void storeCurrentPointForAstar(int row, int col, Point2D * parentPoint);

	void setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, int goalPointNumber);

	void savePath(Point2D * pt, int goalPoint);

	void clearwarriorMaze();

	//calculate the amount of empty (SPACE) pixels around the warrior and return percentage [0,1]
	double getEmptyPixelsPercentageAround();

public:
    Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom);
    
    void escapeFromEnemy(const Warrior& enemy);
   /* void searchEnemy(Point2D& medicinePoint, int goalPointNumber);
    void searchMedicine(Point2D& medicinePoint, int goalPointNumber);
    void searchAmmo(Point2D& ammoPoint, int goalPointNumber);*/
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
	Room& getCurrentRoom();

	bool checkWarriorsInTheSameRoom(Warrior& otherWarrior);

	//calculate own damage in case that enemy hits, depends on distance between warriors and surrounding environment
	int calcPotentialDamage(Warrior& enemy);

	void makeDecision();
	bool shootEnemy(Warrior& warrior);

	int getWarriorMazeColor();

	friend ostream& operator<<(ostream& os, Warrior& warrior);

	void setNoMoreAmmoInGame(bool noMoreAmmo);
	void setNoMoreMedicineInGame(bool noMoreMedicine);
	string getMessage();
	void addToMessage(string message);

};
