
#include "Warrior.h"



Warrior::~Warrior()
{
}

Warrior::Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom)
	: warriorLocation(initialLocation), currentRoom(&currentRoom)
{
    behaviour = warriorBehaviour;
	hp = rand() % MAX_WARRIOR_HP + 2;
    ammoCounter = rand() % MAX_WARRIOR_BULLETS + 2;
    medicineCounter = 5;
	this->status = this->targetPointType = Status(rand() % 2); /*Warrior::SEARCHING_FOR_AMMO;*/

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

double Warrior::getEmptyPixelsPercentageAround()
{
	int startRow = this->warriorLocation.GetY() - PIXEL_RADIUS;
	int endRow = this->warriorLocation.GetY() + PIXEL_RADIUS;
	int startCol = this->warriorLocation.GetX() - PIXEL_RADIUS;
	int endCol = this->warriorLocation.GetX() + PIXEL_RADIUS;
	int maxEmptyPixels = pow(1 + (2 * PIXEL_RADIUS), 2) - 1; 
	int pixelCounter = 0;

	for (int i = startRow; i < endRow; i++)
		for (int j = startCol; j < endCol; j++)
			if ((*this->gameMaze)[i][j] == SPACE)
				pixelCounter++;

	return pixelCounter /(double)maxEmptyPixels;
}

int Warrior::calcPotentialDamage(Warrior & enemy)
{
	double distanceBetweenWarriors = this->warriorLocation.calcDistanceFromTarget(&enemy.getWarriorLocation());
	double maxDistBetweenWarriors = this->currentRoom->getMaxDistInRoom();	//maximal distance in room (when the room as big as possible) 
	//get distance between warriors in the range [1,36] and turn it to damage in range [25,5]
	double shootingDamageByDist = (distanceBetweenWarriors - maxDistBetweenWarriors)*
		((MIN_HIT_DAMAGE - MAX_HIT_DAMAGE) / (maxDistBetweenWarriors - 1)) + MIN_HIT_DAMAGE;

	return (int)ceil(shootingDamageByDist * getEmptyPixelsPercentageAround());
}

void Warrior::makeDecision()
{
	int hpLowBound = (rand() % (4 * MAX_WARRIOR_HP / 5)) + (2 * MAX_WARRIOR_HP / 5);
	int ammoLowBound = (rand() % (2 * MAX_WARRIOR_BULLETS / 3)) + (MAX_WARRIOR_BULLETS / 3);

	if (this->status != IN_BATTLE)
	{
		if (this->hp > hpLowBound)
		{
			if (this->ammoCounter > ammoLowBound)
			{
				this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
			}
			else
			{
				if(!this->noMoreAmmoInGame)	//if there is more ammo in the playground
					this->status = this->targetPointType = SEARCHING_FOR_AMMO;
				else
				{
					if(this->ammoCounter > 0)
						this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
					//else...run?!
				}
			}
		}
		else
		{
			if(!this->noMoreMedicineInGame)	//if there is more life in the playground
				this->status = this->targetPointType = SEARCHING_FOR_MEDICINE;
			else
			{
				if (this->ammoCounter > 0)
					this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
				//else...run?!

			}
		}
	}
}

bool Warrior::shootEnemy(Warrior & enemy)
{
	double damageToEnemy = calcPotentialDamage(enemy);
	
	if (this->ammoCounter <= 0)
		return false;

	double chanceToHit = rand() % 100;

	if (chanceToHit > 60) 
	{
		cout << "I hit the enemy!!! With damage: " << damageToEnemy << endl;
		enemy.getHurt(damageToEnemy);
	}
}

int Warrior::getWarriorMazeColor()
{
	return (*this->warriorColors)[0];
}

void Warrior::setNoMoreAmmoInGame(bool noMoreAmmo)
{
	this->noMoreAmmoInGame = noMoreAmmo;
}

void Warrior::setNoMoreMedicineInGame(bool noMoreMedicine)
{
	this->noMoreAmmoInGame = noMoreMedicine;
}

Point2D & Warrior::getWarriorLocation()
{
    return warriorLocation;
}

int Warrior::getLife()
{
    return hp;
}

int Warrior::getMedicine()
{
    return medicineCounter;
}

int Warrior::getAmmo()
{
    return ammoCounter;
}

void Warrior::addMedicine(int amount)
{
    hp = (hp + amount) > MAX_WARRIOR_HP ? MAX_WARRIOR_HP : (hp + amount);
}

void Warrior::addAmmo(int amount)
{
    ammoCounter = (ammoCounter + amount) > MAX_WARRIOR_BULLETS ? MAX_WARRIOR_BULLETS : (ammoCounter + amount);
}

void Warrior::useMedicine()
{
    if (medicineCounter>0 && hp<15)
    {
        hp++;
        medicineCounter--;
    }
}

void Warrior::shoot()
{
    ammoCounter--;
}

void Warrior::getHurt(int damage)
{
    hp-=damage;
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

void Warrior::searchEnemy(Point2D& enemyPoint, int goalPointNumber)
{
	if (this->targetPoint)
	{
		if (*this->targetPoint != enemyPoint)
		{
			delete this->targetPoint;
			this->targetPoint = new Point2D(enemyPoint.GetX(), enemyPoint.GetY());
			this->targetPointType = Warrior::SEARCHING_FOR_ENEMY;

			//clear the warriorMaze, path, and PQ
			this->path.clear();
			clearwarriorMaze();
			this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
			this->grayPQ.push(&this->warriorLocation);
		}
	}
	else
		this->targetPoint = new Point2D(enemyPoint.GetX(), enemyPoint.GetY());

	AstarSearch(enemyPoint, goalPointNumber);
}

void Warrior::escapeFromEnemy(const Warrior& enemy)
{
    
}

void Warrior::clearwarriorMaze()
{
	for (int i = 0; i < this->warriorMaze.size(); i++)
		for (int j = 0; j < this->warriorMaze[i].size(); j++)
			if (find(this->warriorColors->begin(), this->warriorColors->end(), this->warriorMaze[i][j]) != this->warriorColors->end()
				|| this->warriorMaze[i][j] == GRAY)
				this->warriorMaze[i][j] = SPACE;

	this->warriorMaze[this->warriorLocation.GetY()][this->warriorLocation.GetX()] = (*this->warriorColors)[0];
}



void Warrior::searchMedicine(Point2D& medicinePoint, int goalPointNumber)
{
	if (this->targetPoint)
	{
		if (*this->targetPoint != medicinePoint)
		{
			delete this->targetPoint;
			this->targetPoint = new Point2D(medicinePoint.GetX(), medicinePoint.GetY());
			this->targetPointType = Warrior::SEARCHING_FOR_MEDICINE;

			//clear the warriorMaze, path, and PQ
			this->path.clear();
			clearwarriorMaze();
			this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
			this->grayPQ.push(&this->warriorLocation);
		}
	}
	else
		this->targetPoint = new Point2D(medicinePoint.GetX(), medicinePoint.GetY());

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
		this->targetPoint = new Point2D(ammoPoint.GetX(), ammoPoint.GetY());

	AstarSearch(ammoPoint, goalPointNumber);

}

ostream & operator<<(ostream & os, Warrior & warrior)
{
		os << "position: (" << (warrior.warriorLocation.GetX()) << ", " << (warrior.warriorLocation.GetY()) << ")"
		<< "\ntarget: (" << (warrior.targetPoint->GetX()) << ", " << warrior.targetPoint->GetY() << ")"
		<<"\ntarget type: " << warrior.targetPointType
		<<"\nHP: " <<warrior.hp
		<<"\nAmmo: " << warrior.ammoCounter
		<< "\n\n" << endl;

		return os;
}
