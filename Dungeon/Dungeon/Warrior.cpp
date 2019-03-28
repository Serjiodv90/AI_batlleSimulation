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
	this->status = this->targetPointType = Warrior::SEARCHING_FOR_AMMO;
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

void Warrior::setTragetPoint(Point2D& target)
{
	if (&target)
		this->targetPoint = new Point2D(target.GetX(), target.GetY());
	else
		this->targetPoint = nullptr;
}

void Warrior::setCurrentRoom(Room& room)
{
	this->currentRoom = &room;
}

Room & Warrior::getCurrentRoom()
{
	return *this->currentRoom;
}

bool Warrior::checkWarriorsInTheSameRoom(Warrior & otherWarrior)
{
	if (*this->currentRoom == *otherWarrior.currentRoom)
		return true;
	return false;
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

bool Warrior::isBfsFoundPath(int row, int col, int goalPoint)
{
	if ((*this->gameMaze)[row][col] == goalPoint)   //found target
		return true;
	return false;
}

void Warrior::storeCurrentPointForAstar(int row, int col, Point2D* parentPoint)
{
	Point2D* ptAddToGray = new Point2D(col, row);
	ptAddToGray->set_f(this->targetPoint, parentPoint->get_g() + 1);
	if(!this->parentPointsForPath[row][col])
		this->parentPointsForPath[row][col] = parentPoint;
	grayPQ.push(ptAddToGray);
}

void Warrior::setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, int goalPointNumber)
{
	if (mazeRow < (*this->gameMaze).size() && mazeCol < (*this->gameMaze).size())
	{
		if (isBfsFoundPath(mazeRow, mazeCol, goalPointNumber))	//found target			
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint);
		else if ((*this->gameMaze)[mazeRow][mazeCol] == SPACE && this->warriorMaze[mazeRow][mazeCol] == SPACE)
		{
			this->warriorMaze[mazeRow][mazeCol] = GRAY;
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint);
		}
	}
}

void Warrior::savePath(Point2D* pt, int goalPoint)
{
	Point2D* pt1 = pt;

	while (pt1 != NULL && (*this->gameMaze)[pt1->GetY()][pt1->GetX()] != (*this->warriorColors)[0])
	{
		if ((*this->gameMaze)[pt1->GetY()][pt1->GetX()] != goalPoint)
			this->warriorMaze[pt1->GetY()][pt1->GetX()] = (*this->warriorColors)[1];
		this->path.insert(this->path.begin(), pt1);
		pt1 = this->parentPointsForPath[pt1->GetY()][pt1->GetX()];
	}
}

/*************************************************/
//consider stop ASTAR when get out of current room - problom in tunnels may occure 
/*************************************************/
bool Warrior::AstarSearch(Point2D& targetPoint,	int goalPointNumber)
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

		//paint pt VISITED
		if (*pt == *this->targetPoint)	//found target	
		{
			storeCurrentPointForAstar(mazeRow, mazeCol, pt);
			this->status = IN_MOVEMENT;
			savePathBool = true;
		}
		
		else
		{
			if ((*this->gameMaze)[mazeRow][mazeCol] != (*this->warriorColors)[0])
				this->warriorMaze[mazeRow][mazeCol] = (*this->warriorColors)[2];	//y is i, x is j

			//check down
			mazeRow = pt->GetY() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt,  goalPointNumber);

			//check up
			mazeRow = pt->GetY() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, goalPointNumber);

			//check right
			mazeRow = pt->GetY();
			mazeCol = pt->GetX() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, goalPointNumber);

			//check left
			mazeCol = pt->GetX() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, goalPointNumber);

		}

		if (savePathBool)	//target was found
		{
			savePath(pt, goalPointNumber);
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

void Warrior::searchMedicine(Point2D& medicinePoint, int goalPointNumber)
{
	if (*this->targetPoint != medicinePoint)
	{
		this->targetPoint = &(medicinePoint);
		this->targetPointType = Warrior::SEARCHING_FOR_MEDICINE;

		//clear the warriorMaze, path, and PQ
		this->path.clear();
		clearwarriorMaze();	
		this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
	}

	 AstarSearch(medicinePoint, goalPointNumber);
}

void Warrior::searchAmmo(Point2D& ammoPoint, int goalPointNumber)
{
	if (this->targetPoint)
	{
		if (*this->targetPoint != ammoPoint)
		{
			delete this->targetPoint;
			this->targetPoint = new Point2D(ammoPoint.GetX(), ammoPoint.GetY());
			this->targetPointType = Warrior::SEARCHING_FOR_AMMO;

			//clear the warriorMaze, path, and PQ
			this->path.clear();
			clearwarriorMaze();
			this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
			this->grayPQ.push(&this->warriorLocation);
		}
	}
	else
		this->targetPoint = new Point2D(ammoPoint.GetX(), ammoPoint.GetY());;

	AstarSearch(ammoPoint, goalPointNumber);

}
