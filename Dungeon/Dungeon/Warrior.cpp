//
//  Warrior.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/03/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#include "Warrior.h"



Warrior::~Warrior()
{
}

Warrior::Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom)
	: warriorLocation(initialLocation), currentRoom(&currentRoom)
{
    behaviour = warriorBehaviour;
    lifeCounter = 15;
    ammoCounter = 5;
    medicineCounter = 5;
	this->status = Warrior::SEARCHING_FOR_AMMO;
	this->grayPQ.push(&this->warriorLocation);
	copy(maze.begin(), maze.end(), back_inserter(this->warriorMaze));
	this->gameMaze = &maze;
	this->warriorColors = &colors;
}

void Warrior::setWarriorLocation(Point2D& location)
{
    warriorLocation = location/*Point2D(location.GetX(), location.GetY())*/;
}

vector<vector<int>>& Warrior::getWarriorMaze()
{
	return this->warriorMaze;
}

void Warrior::moveWarriorByOne()
{
	if (this->path.size() > 0)
	{
		setWarriorLocation(*this->path[0]);
		this->path.erase(this->path.begin());
	}
}

Warrior::Status Warrior::getWarriorStatus()
{
	return this->status;
}

void Warrior::setWarriorStatus(Status status)
{
	this->status = status;
}

Warrior::Status Warrior::getPreviousTargetPointType()
{
	return this->targetPointType;
}

Point2D & Warrior::getPreviousTargetPoint()
{
	return *this->targetPoint;
}

void Warrior::setCurrentRoom(Room& room)
{
	this->currentRoom = &room;
}

Room & Warrior::getCurrentRoom()
{
	return *this->currentRoom;
}

Point2D & Warrior::getWarriorLocation()
{
    return warriorLocation;
}

int Warrior::getLife()
{
    return lifeCounter;
}

int Warrior::getMedicine()
{
    return medicineCounter;
}

int Warrior::getAmmo()
{
    return ammoCounter;
}

void Warrior::addMedicine()
{
    medicineCounter++;
}

void Warrior::addAmmo()
{
    ammoCounter++;
}

void Warrior::useMedicine()
{
    if (medicineCounter>0 && lifeCounter<15)
    {
        lifeCounter++;
        medicineCounter--;
    }
}

void Warrior::shoot()
{
    ammoCounter--;
}

void Warrior::getHurt(int damage)
{
    lifeCounter-=damage;
}

void Warrior::fightAgainstEnemy()
{
    
}

bool Warrior::isBfsFoundPath(int row, int col, int goalPoint, /*int maze[MSIZE][MSIZE]*/vector<vector<int>>& maze)
{
	if (/*maze*/(*this->gameMaze)[row][col] == goalPoint)   //found target
		return true;
	return false;
}

void Warrior::storeCurrentPointForAstar(int row, int col, Point2D* parentPoint, Point2D* targetPoint)
{
	Point2D* ptAddToGray = new Point2D(col, row);
	ptAddToGray->set_f(targetPoint, parentPoint->get_g() + 1);
	if(!this->parentPointsForPath[row][col])
		this->parentPointsForPath[row][col] = parentPoint;
	grayPQ.push(ptAddToGray);
}

void Warrior::setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, /*int maze[MSIZE][MSIZE]*/vector<vector<int>>& maze, int goalPointNumber, Point2D* targetPoint)
{
	if (mazeRow < (*this->gameMaze).size() && mazeCol < (*this->gameMaze).size())
	{
		if (isBfsFoundPath(mazeRow, mazeCol, goalPointNumber, maze))	//found target			
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint, targetPoint);
		else if (/*maze*/(*this->gameMaze)[mazeRow][mazeCol] == SPACE && this->warriorMaze[mazeRow][mazeCol] == SPACE)
		{
			/*maze*/this->warriorMaze[mazeRow][mazeCol] = GRAY;
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint, targetPoint);
		}
	}
}

void Warrior::savePath(Point2D* pt, int beginPoint, int goalPoint, int warriorPathNumber, /*int maze[MSIZE][MSIZE]*/vector<vector<int>>& maze)
{
	Point2D* pt1 = pt;

	while (pt1 != NULL && (*this->gameMaze)[pt1->GetY()][pt1->GetX()] != beginPoint)
	{
		if (/*maze*/(*this->gameMaze)[pt1->GetY()][pt1->GetX()] != goalPoint)
			/*maze*/this->warriorMaze[pt1->GetY()][pt1->GetX()] = warriorPathNumber;
		this->path.insert(this->path.begin(), pt1);
		pt1 = this->parentPointsForPath[pt1->GetY()][pt1->GetX()];
	}
}

bool Warrior::AstarSearch(Point2D& startPoint, Point2D& targetPoint, /*int maze[MSIZE][MSIZE]*/vector<vector<int>>& maze,
	int goalPointNumber, int warriorVisitedNumber, int warriorMazeNumber, int warriorPathNumber)
{
	Point2D* pt;
	int mazeRow, mazeCol;
	bool savePathBool = false;
	

	if (grayPQ.empty())	//grey is the edges that didn't visited yet
		return false;
	else
	{
		pt = grayPQ.top();	//this will be the parent
		grayPQ.pop();

		mazeRow = pt->GetY(); 
		mazeCol = pt->GetX();
		cout << "mazeRow: " << mazeRow << ",\tmazeCol: " << mazeCol << endl;

		//paint pt VISITED
		if (*pt == targetPoint)	//found target	
		{
			storeCurrentPointForAstar(mazeRow, mazeCol, pt, &targetPoint);
			this->status = IN_MOVEMENT;
			savePathBool = true;
		}
		
		else
		{
			if (/*maze*/(*this->gameMaze)[mazeRow][mazeCol] != warriorMazeNumber)
				/*maze*/this->warriorMaze[mazeRow][mazeCol] = warriorVisitedNumber;	//y is i, x is j

			//check down
			mazeRow = pt->GetY() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, maze, goalPointNumber, &targetPoint);

			//check up
			mazeRow = pt->GetY() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, maze, goalPointNumber, &targetPoint);

			//check right
			mazeRow = pt->GetY();
			mazeCol = pt->GetX() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, maze, goalPointNumber, &targetPoint);

			//check left
			mazeCol = pt->GetX() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, maze, goalPointNumber, &targetPoint);

		}

		if (savePathBool)	//target was found
		{
			savePath(pt, warriorMazeNumber, goalPointNumber, warriorPathNumber, maze);
			return true;
		}
	}
}

void Warrior::searchEnemy(const Warrior& enemy)
{
    
}

void Warrior::escapeFromEnemy(const Warrior& enemy)
{
    
}

void Warrior::clearwarriorMaze()
{
	for (int i = 0; i < this->warriorMaze.size(); i++)
		for (int j = 0; j < this->warriorMaze[i].size(); j++)
			if (find(this->warriorColors->begin(), this->warriorColors->end(), this->warriorMaze[i][j]) != this->warriorColors->end())
				this->warriorMaze[i][j] = SPACE;
}

void Warrior::searchMedicine(Point2D& medicinePoint, vector<vector<int>>& maze,	int goalPointNumber, vector<int> warrior_colors)
{
	if (*this->targetPoint != medicinePoint)
	{
		this->targetPoint = &medicinePoint;
		this->targetPointType = Warrior::SEARCHING_FOR_MEDICINE;

		//clear the warriorMaze, path, and PQ
		this->path.clear();
		clearwarriorMaze();	
		this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
	}

	 AstarSearch(this->warriorLocation, medicinePoint, maze,
		goalPointNumber, warrior_colors[2], warrior_colors[0], warrior_colors[1]);
}

void Warrior::searchAmmo(Point2D& ammoPoint, vector<vector<int>>& maze, int goalPointNumber, vector<int> warrior_colors)
{
	if (this->targetPoint && *this->targetPoint != ammoPoint)
	{
		this->targetPoint = &ammoPoint;
		this->targetPointType = Warrior::SEARCHING_FOR_MEDICINE;

		//clear the warriorMaze, path, and PQ
		this->path.clear();
		clearwarriorMaze();
		this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
	}

	AstarSearch(this->warriorLocation, ammoPoint, maze,
		goalPointNumber, warrior_colors[2], warrior_colors[0], warrior_colors[1]);
}
