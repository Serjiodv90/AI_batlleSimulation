
#include "Warrior.h"



Warrior::~Warrior()
{
}

Warrior::Warrior(Point2D& initialLocation, int warriorBehaviour, vector<vector<int>>& maze, vector<int>& colors, Room& currentRoom)
	: warriorLocation(initialLocation), currentRoom(&currentRoom)
{
	srand(time(0));
	behaviour = warriorBehaviour;
	hp = (rand() % (MAX_WARRIOR_HP / 2)) + (MAX_WARRIOR_HP / 10);
	ammoCounter = (rand() % (MAX_WARRIOR_BULLETS / 2)) + (MAX_WARRIOR_BULLETS / 10);
	medicineCounter = 5;
	this->status = this->targetPointType = Status(rand() % 2);
	this->warriorLocation.set_g(0);
	this->grayPQ.push(&this->warriorLocation);
	copy(maze.begin(), maze.end(), back_inserter(this->warriorMaze));
	this->gameMaze = &maze;
	this->warriorColors = &colors;
	this->message = "";
	this->behavior = Behavior(warriorBehaviour);
	insertVisitedRoom();
}

void Warrior::setWarriorLocation(Point2D& location)
{
	warriorLocation = location;
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
	else	//if there is no where to move, find new target point, so it won't stuck in place
	{
		this->targetPoint = nullptr;
		this->status = this->targetPointType;
		clearAllPathes();
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

void Warrior::insertVisitedRoom()
{
	if (this->currentRoom)
	{
		if (this->visitedRooms.empty())
			this->visitedRooms.push_back(this->currentRoom);
		else
		{
			vector<Room*>::iterator it = find(this->visitedRooms.begin(), this->visitedRooms.end(), this->currentRoom);
			if (it != this->visitedRooms.end())
				this->visitedRooms.erase(it);
			this->visitedRooms.push_back(this->currentRoom);
		}
	}
}

void Warrior::setCurrentRoom(Room& room)
{
	this->currentRoom = new Room(room);
	insertVisitedRoom();
}

void Warrior::setNotInRoom()
{
	this->currentRoom = nullptr;
}

Room & Warrior::getCurrentRoom()
{
	return *this->currentRoom;
}

Room & Warrior::getPreviousRoomVisitet()
{
	if(this->visitedRooms.size() > 1)
		return *this->visitedRooms.at(this->visitedRooms.size() - 2);
	else
		return *this->visitedRooms.at(this->visitedRooms.size() - 1);
}

bool Warrior::checkWarriorsInTheSameRoom(Warrior & otherWarrior)
{
	int enemy_x = otherWarrior.getWarriorLocation().GetX();
	int enemy_y = otherWarrior.getWarriorLocation().GetY();

	if(this->currentRoom)
		if (enemy_x >= this->currentRoom->getLeft() && enemy_x <= this->currentRoom->getRight()
			&& enemy_y >= this->currentRoom->getTop() && enemy_y <= this->currentRoom->getBottom())
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

	return pixelCounter / (double)maxEmptyPixels;
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

Point2D& Warrior::findTheFarestCornerFromEnemyInRoom(Warrior& enemy)
{
	int left = this->currentRoom->getLeft();
	int right = this->currentRoom->getRight();
	int top = this->currentRoom->getTop();
	int bottom = this->currentRoom->getBottom();
	int sides[2][2] = { {left, right}, {top, bottom} };

	Point2D tmp = Point2D(left, top);
	Point2D retPoint = tmp;
	Point2D& enemyLoc = enemy.getWarriorLocation();
	int max = tmp.calcDistanceFromTarget(&enemyLoc);
	int tmpMax;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			tmp = Point2D(sides[0][i], sides[1][j]);
			tmpMax = tmp.calcDistanceFromTarget(&enemyLoc);
			if (tmpMax > max)
			{
				max = tmpMax;
				retPoint = tmp;
			}
		}
	}

	return retPoint;

}

void Warrior::deffensiveDesicions(Warrior& enemy)
{
	srand(time(0));
	int hpLowBound = 4 * MAX_WARRIOR_HP / 5; /*(rand() % (2 * MAX_WARRIOR_HP / 5)) + (2 * MAX_WARRIOR_HP / 5);*/
	int ammoLowBound = (rand() % (2 * MAX_WARRIOR_BULLETS / 3)) + (MAX_WARRIOR_BULLETS / 3);
	bool areWarriorsInTheSameRoom = checkWarriorsInTheSameRoom(enemy);

	if (!areWarriorsInTheSameRoom)
	{
		if (this->hp > hpLowBound)
		{
			if (this->ammoCounter > ammoLowBound)
			{
				this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
			}
			else
			{
				if (!this->noMoreAmmoInGame)	//if there is more ammo in the playground
					this->status = this->targetPointType = SEARCHING_FOR_AMMO;
				else
				{
					if (this->ammoCounter > 0)
						this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
					
				}
			}
		}
		else
		{
			if (!this->noMoreMedicineInGame)	//if there is more life in the playground
				this->status = this->targetPointType = SEARCHING_FOR_MEDICINE;
			else
			{
				if (this->ammoCounter > 0)
					this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
				

			}
		}
	}
	else	//in the same room
	{
		if (this->ammoCounter > 0 && enemy.getLife() > 0)
		{
			shootEnemy(enemy);
			this->status = this->targetPointType = IN_BATTLE;
			Point2D runFromEnemyTarget;
			if (enemy.getLife() > 0)
			{
				if (this->getWarriorLocation().calcDistanceFromTarget(&enemy.getWarriorLocation()) < 10
					&& this->visitedRooms.size() > 1)
				{
					runFromEnemyTarget = this->visitedRooms.at(this->visitedRooms.size() - 2)->GetCenter();
					this->addToMessage("\nGoing to the last room I was in to hide");
				}
				else
				{
					runFromEnemyTarget = findTheFarestCornerFromEnemyInRoom(enemy);
					this->addToMessage("\nGoing to the farest corner to hide");
				}
				clearAllPathes();
				while (this->status != IN_MOVEMENT)
				{
					AstarSearch(runFromEnemyTarget);
					if (this->grayPQ.empty())
						clearAllPathes();
				}
			}
		}
		else
		{
			if (!this->noMoreAmmoInGame)	//if there is more ammo in the playground
				this->status = this->targetPointType = SEARCHING_FOR_AMMO;
		}
	}

}

void Warrior::offensiveDesicions(Warrior& enemy)
{
	int hpLowBound = (rand() % (2 * MAX_WARRIOR_HP / 5)) + (2 * MAX_WARRIOR_HP / 5);
	int ammoLowBound = (rand() % (2 * MAX_WARRIOR_BULLETS / 3)) + (MAX_WARRIOR_BULLETS / 3);
	bool areWarriorsInTheSameRoom = checkWarriorsInTheSameRoom(enemy);
	srand(time(0));

	if (!areWarriorsInTheSameRoom)
	{
		if (this->hp > MAX_WARRIOR_HP / 2)
		{
			if (this->ammoCounter > MAX_WARRIOR_BULLETS / 2)
			{
				this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
			}
			else
			{
				if (!this->noMoreAmmoInGame)	//if there is more ammo in the playground
					this->status = this->targetPointType = SEARCHING_FOR_AMMO;
				else
				{
					if (this->ammoCounter > 0)
						this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
					
				}
			}
		}
		else
		{
			if (!this->noMoreMedicineInGame)	//if there is more life in the playground
				this->status = this->targetPointType = SEARCHING_FOR_MEDICINE;
			else
			{
				if (this->ammoCounter > 0)
					this->status = this->targetPointType = SEARCHING_FOR_ENEMY;
				

			}
		}
	}
	else	//in the same room
	{
		if (this->ammoCounter > 0 && enemy.getLife() > 0)
		{
			shootEnemy(enemy);
			this->status = this->targetPointType = IN_BATTLE;
			if (enemy.getLife() > 0)
			{
				Point2D enemyInRoom = enemy.getWarriorLocation();
				this->addToMessage("\nATTAAACKKKK!!!!!");
				clearAllPathes();
				while (this->status != IN_MOVEMENT)
				{
					AstarSearch(enemyInRoom);
					if (this->grayPQ.empty())
						clearAllPathes();
				}
			}
		}
		else
		{
			if (!this->noMoreAmmoInGame)	//if there is more ammo in the playground
				this->status = this->targetPointType = SEARCHING_FOR_AMMO;
		}
	}
}

void Warrior::makeDecision(Warrior& enemy)
{
	if (this->behavior == DEFFENSIVE)
		deffensiveDesicions(enemy);
	else
		offensiveDesicions(enemy);
}

bool Warrior::shootEnemy(Warrior & enemy)
{
	srand(time(0));
	double damageToEnemy = calcPotentialDamage(enemy);

	if (this->ammoCounter <= 0)
		return false;

	double chanceToHit = rand() % 100;

	if (chanceToHit > 60)
	{
		ostringstream s;
		s << "\nI hit the enemy!!! With damage: " << damageToEnemy;
		this->addToMessage(s.str());
		enemy.getHurt(damageToEnemy);
		shoot();
		if (enemy.getLife() <= 0)
			this->addToMessage("\nENEMY IS DOWN! I WON!!!");
	}
	else
		this->addToMessage("\nDAMN! I have missed!");
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

string Warrior::getMessage()
{
	return this->specs + this->message;
}

void Warrior::addToMessage(string message)
{
	if (this->message.find(message) == string::npos ||
		(this->message.substr(this->message.size() - message.size(), message.size()) != message))
	{
		if (count(this->message.begin(), this->message.end(), '\n') > 12)
			this->message = "";
		this->message += message;
	}
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
	if (medicineCounter > 0 && hp < 15)
	{
		hp += 15;
		medicineCounter--;
	}
}

void Warrior::shoot()
{
	ammoCounter--;
}

void Warrior::getHurt(int damage)
{
	ostringstream s;
	s << "\nI am hit, damage: " << damage;
	this->addToMessage(s.str());
	hp -= damage;
	if (this->hp <= 0)
		this->addToMessage("\nI AM DEAD!!!");
	this->status = this->targetPointType = IN_BATTLE;
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

void Warrior::storeCurrentPointForAstar(int row, int col, Point2D* parentPoint, Point2D* targetPoint)
{
	Point2D* ptAddToGray = new Point2D(col, row);
	ptAddToGray->set_f(targetPoint, parentPoint->get_g() + 1);
	if (!this->parentPointsForPath[row][col])
		this->parentPointsForPath[row][col] = parentPoint;
	grayPQ.push(ptAddToGray);
}

void Warrior::setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint, Point2D* targetPoint)
{
	if (mazeRow < (*this->gameMaze).size() && mazeCol < (*this->gameMaze).size())
	{
		if (targetPoint->GetX() == mazeCol && targetPoint->GetY() == mazeRow)
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint, targetPoint);
		else if ((*this->gameMaze)[mazeRow][mazeCol] == SPACE && this->warriorMaze[mazeRow][mazeCol] == SPACE)
		{
			this->warriorMaze[mazeRow][mazeCol] = GRAY;
			storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint, targetPoint);
		}
	}
}

void Warrior::savePath(Point2D* pt, Point2D* targetPoint)
{
	Point2D* pt1 = pt;

	while (pt1 != NULL && (*this->gameMaze)[pt1->GetY()][pt1->GetX()] != (*this->warriorColors)[0])
	{
		if (targetPoint->GetX() != pt1->GetX() && targetPoint->GetY() != pt1->GetY())
			this->warriorMaze[pt1->GetY()][pt1->GetX()] = (*this->warriorColors)[1];
		this->path.insert(this->path.begin(), pt1);
		int y = pt1->GetY();
		int x = pt1->GetX();
		pt1 = this->parentPointsForPath[y][x];

	}
}

bool Warrior::AstarSearch(Point2D& targetPoint/*,	int goalPointNumber*/)
{
	Point2D* pt;
	int mazeRow, mazeCol;
	bool savePathBool = false;


	if (grayPQ.empty())	//grey is the edges that didn't visited yet
		return false;
	else
	{
		astartCount++;
		pt = grayPQ.top();	//this will be the parent
		grayPQ.pop();

		mazeRow = pt->GetY();
		mazeCol = pt->GetX();

		//paint pt VISITED
		if (*pt == targetPoint || 
			((this->status == IN_BATTLE) && (this->behavior == OFFENSIVE) && (astartCount % 40 == 0)))	//found target	
		{
			storeCurrentPointForAstar(mazeRow, mazeCol, pt, &targetPoint);
			this->status = IN_MOVEMENT;
			savePathBool = true;
		}

		else
		{
			if ((*this->gameMaze)[mazeRow][mazeCol] != (*this->warriorColors)[0])
				this->warriorMaze[mazeRow][mazeCol] = (*this->warriorColors)[2];	//y is i, x is j

			//check down
			mazeRow = pt->GetY() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, &targetPoint);

			//check up
			mazeRow = pt->GetY() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, &targetPoint);

			//check right
			mazeRow = pt->GetY();
			mazeCol = pt->GetX() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, &targetPoint);

			//check left
			mazeCol = pt->GetX() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt, &targetPoint);

		}

		if (savePathBool)	//target was found
		{
			savePath(pt, &targetPoint);
			return true;
		}
	}
}


void Warrior::clearwarriorMaze()
{
	for (int i = 0; i < this->warriorMaze.size(); i++)
		for (int j = 0; j < this->warriorMaze[i].size(); j++)
		{
			
			if ((*this->gameMaze)[i][j] == WARRIOR_1 || (*this->gameMaze)[i][j] == WARRIOR_2)
				this->warriorMaze[i][j] = SPACE;
			else
				this->warriorMaze[i][j] = (*this->gameMaze)[i][j];


			this->parentPointsForPath[i][j] = nullptr;
		}

	this->warriorMaze[this->warriorLocation.GetY()][this->warriorLocation.GetX()] = (*this->warriorColors)[0];
}

void Warrior::clearAllPathes()
{
	this->path.clear();
	clearwarriorMaze();
	this->grayPQ = priority_queue<Point2D*, vector<Point2D*>, ComparePointsByDist>();	//clear the pq
	this->warriorLocation.set_g(0);
	this->grayPQ.push(&this->warriorLocation);
}

void Warrior::searchForTarget(Point2D& targetPoint, int goalPointNumber)
{
	if (this->targetPoint)
	{
		if (*this->targetPoint != targetPoint)
		{
			this->targetPoint = new Point2D(targetPoint.GetX(), targetPoint.GetY());
			//clear the warriorMaze, parentPointsForPath, path, and PQ
			clearAllPathes();
		}
	}
	else
		this->targetPoint = new Point2D(targetPoint.GetX(), targetPoint.GetY());
	if (grayPQ.size() <= 0)
		clearAllPathes();
	AstarSearch(targetPoint);
}

void Warrior::addSpecsToMessage()
{
	ostringstream s;
	if (this->targetPoint)
		s << "position: \t(" << (this->warriorLocation.GetX()) << ", " << (this->warriorLocation.GetY()) << ")"
		<< "\nbehavior: \t" << this->behaviorString[this->behavior]
		<< "\ntarget: \t(" << (this->targetPoint->GetX()) << ", " << this->targetPoint->GetY() << ")"
		<< "\ntarget type: \t" << this->statusString[this->targetPointType]
		<< "\nHP: \t\t" << this->hp
		<< "\nAmmo: \t\t" << this->ammoCounter;
	else
		s << "position: \t(" << (this->warriorLocation.GetX()) << ", " << (this->warriorLocation.GetY()) << ")"
		<< "\nbehavior: \t" << this->behaviorString[this->behavior]
		<< "\ntarget: \t( searching for target )"
		<< "\ntarget type: \t" << this->statusString[this->targetPointType]
		<< "\nHP: \t\t" << this->hp
		<< "\nAmmo: \t\t" << this->ammoCounter;

	this->specs = s.str();
}

ostream & operator<<(ostream & os, Warrior & warrior)
{
	return os << warrior.specs << warrior.message;
}
