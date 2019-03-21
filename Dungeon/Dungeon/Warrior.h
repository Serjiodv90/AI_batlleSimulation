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

	bool AstarSearch(Point2D& startPoint, Point2D& targetPoint,vector<vector<int>>& maze, int goalPointNumber, int warriorVisitedNumber, int warriorMazeNumber, int warriorPathNumber);

    void fightAgainstEnemy();

	bool isBfsFoundPath(int row, int col, int goalPoint, vector<vector<int>>& maze);

	void storeCurrentPointForAstar(int row, int col, Point2D * parentPoint, Point2D * targetPoint);

	void setPointAsGrayForAStar(int & mazeRow, int & mazeCol, Point2D *& parentPoint, vector<vector<int>>& maze, int goalPointNumber, Point2D* targetPoint);

	void savePath(Point2D * pt, int beginPoint, int goalPoint, int warriorPathNumber, vector<vector<int>>& maze);

public:
    Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors);
    
    void searchEnemy(const Warrior& enemy);
    void escapeFromEnemy(const Warrior& enemy);
    void searchMedicine(Point2D& medicinePoint, vector<vector<int>>& maze,
		int goalPointNumber, vector<int> warrior_colors);

    void searchAmmo(Point2D& ammoPoint, vector<vector<int>>& maze,
		int goalPointNumber, vector<int> warrior_colors);
    
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
	vector<vector<int>>& getWarriorMaze();
	void moveWarriorByOne();

	Status getWarriorStatus();
};
