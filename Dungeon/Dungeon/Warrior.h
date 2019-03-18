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

using namespace std;



class Warrior
{
public:
	enum Status { SEARCHING_FOR_MEDICINE, SEARCHING_FOR_AMMO, SEARCHING_FOR_OPPONENT, IN_MOVEMENT };
    Warrior();
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
	Point2D* targetPoint;

	bool AstarSearch(Point2D& startPoint, Point2D& targetPoint, int maze[MSIZE][MSIZE], int goalPointNumber, int warriorVisitedNumber, int warriorMazeNumber, int warriorPathNumber);

    void fightAgainstEnemy();

	bool isBfsFoundPath(int row, int col, int goalPoint, int maze[MSIZE][MSIZE]);

	void storeCurrentPointForAstar(int row, int col, Point2D * parentPoint, Point2D * targetPoint);

	void setPointAsGrayForAStar(int & mazeRow, int & mazeCol, Point2D *& parentPoint, int maze[MSIZE][MSIZE], int goalPointNumber, Point2D* targetPoint);

	void savePath(Point2D * pt, int beginPoint, int goalPoint, int warriorPathNumber, int maze[MSIZE][MSIZE]);

public:
    Warrior(Point2D& initialLocation, int warriorBehaviour);
    
    void searchEnemy(const Warrior& enemy);
    void escapeFromEnemy(const Warrior& enemy);
    void searchMedicine(Point2D& medicinePoint, int maze[MSIZE][MSIZE],
		int goalPointNumber, int warriorVisitedNumber, int warriorMazeNumber, int warriorPathNumber);
    void searchAmmo(Point2D& ammoPoint, int maze[MSIZE][MSIZE],
		int goalPointNumber, vector<int> warrior_colors/*int warriorVisitedNumber, int warriorMazeNumber, int warriorPathNumber*/);
    
    void addAmmo();
    void addMedicine();

    int getAmmo();
    int getMedicine();
    int getLife();

    void useMedicine();
    void shoot();
    void getHurt(int damage);
    
    Point2D getWarriorLocation();
    void setWarriorLocation(Point2D& location);

	void moveWarriorByOne();

	Status getWarriorStatus();
};
