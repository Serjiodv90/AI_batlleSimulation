#pragma warning(disable:4996)

#include "GLUT.H"
#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <queue>
#include <algorithm>
#include "Point2D.h"
#include "Room.h"
#include "Node.h"
#include "CompareNodes.h"
#include "Parent.h"
#include "Warrior.h"
#include <string.h>
#include "Definitions.h"

using namespace std;



//int maze[MSIZE][MSIZE];
vector<vector<int>> maze;
bool bfs_started = false;
int counterAmmo = 0;
int counterMedicine = 0;

Room all_rooms[NUM_ROOMS];
Warrior* warrior1;
Warrior* warrior2;

vector<Warrior*> warriors /*= { warrior1, warrior2 }*/;

// vector for tunnels
vector <Point2D> gray;
vector <Point2D> black;
vector <Parent> parents;

// medicine verctor
vector <Point2D> medicine;
// ammo verctor
vector <Point2D> ammo;

priority_queue<Node, vector<Node>, CompareNodes> pq;

Point2D start, target;
vector<vector<int>> warriors_color = { { WARRIOR_1, WARRIOR1_PATH,  VISITED_W1 },{ WARRIOR_2, WARRIOR2_PATH, VISITED_W2 } };


int idleCounter = 0;
bool drawWarrior1Maze = false;
bool drawWarrior2Maze = false;
bool isGameOver = false;
bool startGame = false;

void SetupMaze();

void init()
{
	int i, j;

	srand(time(0));

	maze.resize(MSIZE);
	for (i = 0; i < MSIZE; i++)
		maze[i].resize(MSIZE);

	// clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
			maze[i][j] = WALL;

	SetupMaze();

	glClearColor(0.7, 0.7, 0.7, 0);

	glOrtho(-1, 1, -1, 1, -1, 1);
}

void AddNewNode(Node current, int direction)
{
	Node* tmp;
	Point2D* pt;
	vector<Point2D>::iterator gray_it;
	vector<Point2D>::iterator black_it;
	double space_weight = 0.1, wall_weight = 5, weight;
	int dx = 0, dy = 0;

	switch (direction)
	{
	case UP:
		dx = 0;
		dy = -1;
		break;
	case DOWN:
		dx = 0;
		dy = 1;
		break;
	case LEFT:
		dx = -1;
		dy = 0;
		break;
	case RIGHT:
		dx = 1;
		dy = 0;
		break;
	}// switch

	if ((direction == UP && current.GetPoint().GetY() > 0) ||
		(direction == DOWN && current.GetPoint().GetY() < MSIZE - 1) ||
		(direction == LEFT && current.GetPoint().GetX() > 0) ||
		(direction == RIGHT && current.GetPoint().GetX() < MSIZE - 1))
	{
		pt = new Point2D(current.GetPoint().GetX() + dx, current.GetPoint().GetY() + dy);
		gray_it = find(gray.begin(), gray.end(), *pt);
		black_it = find(black.begin(), black.end(), *pt);
		if (gray_it == gray.end() && black_it == black.end()) // this is a new point
		{
			// very important to tunnels
			if (maze[current.GetPoint().GetY() + dy][current.GetPoint().GetX() + dx] == WALL)
				weight = wall_weight;
			else weight = space_weight;
			// weight depends on previous weight and wheater we had to dig
			// to this point or not
			tmp = new Node(*pt, target, current.GetG() + weight);
			pq.emplace(*tmp); // insert first node to priority queue
			gray.push_back(*pt); // paint it gray
			// add Parent
			parents.push_back(Parent(tmp->GetPoint(), current.GetPoint(), true));
		}
	}

}

void RunAStar4Tunnels()
{
	Node current;

	vector<Point2D>::iterator gray_it;
	vector<Point2D>::iterator black_it;
	bool finished = false;
	double space_weight = 0.5, wall_weight = 0.5, weight;

	while (!pq.empty() && !finished)
	{
		current = pq.top();
		pq.pop(); // remove it from pq

		if (current.GetH() == 0) // the solution has been found
		{
			vector<Parent>::iterator itr;
			finished = true;
			// go back to start and change WALL to SPACE
			itr = find(parents.begin(), parents.end(),
				Parent(current.GetPoint(), current.GetPoint(), true));
			while (itr->HasParent())
			{
				Point2D tmp_prev = itr->GetPrev();
				Point2D tmp_cur = itr->GetCurrent();
				// set SPACE
				if (maze[tmp_cur.GetY()][tmp_cur.GetX()] == WALL)
					maze[tmp_cur.GetY()][tmp_cur.GetX()] = SPACE;
				itr = find(parents.begin(), parents.end(),
					Parent(tmp_prev, current.GetPoint(), true));
			}
		}
		else // check the neighbours
		{
			// remove current from gray
			gray_it = find(gray.begin(), gray.end(), current.GetPoint());
			if (gray_it != gray.end())
				gray.erase(gray_it);
			// and paint it black
			black.push_back(current.GetPoint());
			// try to go UP
			AddNewNode(current, UP);
			// try to go DOWN
			AddNewNode(current, DOWN);
			// try to go LEFT
			AddNewNode(current, LEFT);
			// try to go RIGHT
			AddNewNode(current, RIGHT);

		}

	} // while
}


void DigTunnels()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			start = all_rooms[i].GetCenter();
			target = all_rooms[j].GetCenter();

			printf("Start: %d      Target: %d\n", i, j);

			Node* tmp = new Node(start, target, 0);
			while (!pq.empty())
				pq.pop();

			pq.emplace(*tmp); // insert first node to priority queue
			gray.clear();
			gray.push_back(start); // paint it gray
			black.clear();
			parents.clear();
			parents.push_back(Parent(tmp->GetPoint(), tmp->GetPoint(), false));
			RunAStar4Tunnels();
			delete tmp;
		}
}


void DrawRoom(int right, int left, int bottom, int top)
{
	for (int i = top; i <= bottom; i++)
	{
		for (int j = left; j <= right; j++)
		{
			maze[i][j] = SPACE;
		}
	}
}


/*******************************************************************************************
********************************************************************************************
********************************************************************************************
********************************************************************************************
********************************************************************************************/

Point2D& findPositionForObjectInRoom(Room& room)
{
	int left = room.getLeft()/*GetCenter().GetX() - room.GetWidth() / 2*/;
	int right = room.getRight()/*GetCenter().GetX() + room.GetWidth() / 2*/;
	int top = room.getTop()/*GetCenter().GetY() - room.GetHeight() / 2*/;
	int bottom = room.getBottom()/*GetCenter().GetY() + room.GetHeight() / 2*/;
	bool noCollision = false;

	int obj_x, obj_y;
	do
	{
		obj_x = (int)(rand() % ((right - 1) - (left + 1)) + left + 1);
		obj_y = (int)(rand() % ((bottom - 1) - (top + 1)) + top + 1);
		if (obj_x < MSIZE && obj_y < MSIZE)
			if (maze[obj_y - 1][obj_x] == SPACE && maze[obj_y + 1][obj_x] == SPACE)
				noCollision = true;

	} while (!noCollision);

	return Point2D(obj_x, obj_y);
}


void DrawAmmo(Room& room)
{
	if (room.GetHasAmmo())
	{
		Point2D tmpCenter = findPositionForObjectInRoom(room);
		int currentY = tmpCenter.GetY();
		int currentX = tmpCenter.GetX();

		maze[currentY][currentX] = AMMO;
		maze[currentY - 1][currentX] = AMMO;
		maze[currentY + 1][currentX] = AMMO;
		ammo.push_back(Point2D(currentX, currentY));
	}
}

void DrawMedicine(Room& room)
{
	if (room.GetHasMedicine())
	{
		Point2D tmpCenter = findPositionForObjectInRoom(room);
		int currentY = tmpCenter.GetY();
		int currentX = tmpCenter.GetX();
		maze[currentY][currentX] = MEDICINE;
		maze[currentY - 1][currentX] = MEDICINE;
		maze[currentY + 1][currentX] = MEDICINE;
		medicine.push_back(Point2D(currentX, currentY));
	}
}

void initWarriorInitialRoom(int warrior, Warrior*& warriorObj, Room& room)
{
	Point2D tmpCenter = findPositionForObjectInRoom(room);
	int currentY = tmpCenter.GetY();
	int currentX = tmpCenter.GetX();
	maze[currentY][currentX] = warrior;
	cout << "Warrior - " << warrior << " location: (X,Y)-->(" << currentX << ", " << currentY << ") " << endl;
	warriorObj = new Warrior(Point2D(currentX, currentY), 0, maze,
		warrior == WARRIOR_1 ? warriors_color[0] : warriors_color[1], room);
}

void SetupMaze()
{
	int i, counter;
	int left, right, top, bottom;
	bool isValidRoom;
	int warriorRandRoom1 = 0, warriorRandRoom2 = 1;

	do	//different rooms - to make it more interesting
	{
		warriorRandRoom1 = (int)(rand() % NUM_ROOMS);
		warriorRandRoom2 = (int)(rand() % NUM_ROOMS);
	} while (warriorRandRoom1 == warriorRandRoom2);

	int room_xLimit = (MIN_ROOM_WIDTH_AND_HEIGHT + MAX_ROOM_WIDTH);
	int romm_yLimit = (MIN_ROOM_WIDTH_AND_HEIGHT + MAX_ROOM_HEIGHT);
	Room* pr = NULL;

	for (counter = 0; counter < NUM_ROOMS; counter++)
	{
		// create room
		do
		{
			free(pr);
			pr = new Room(Point2D((rand() % (MSIZE - (room_xLimit))) + room_xLimit / 2,
				(rand() % (MSIZE - (romm_yLimit))) + romm_yLimit / 2)
				, MIN_ROOM_WIDTH_AND_HEIGHT + rand() % MAX_ROOM_WIDTH
				, MIN_ROOM_WIDTH_AND_HEIGHT + rand() % MAX_ROOM_HEIGHT);

			top = pr->GetCenter().GetY() - pr->GetHeight() / 2;
			//if (top < 0) top = 0;
			bottom = pr->GetCenter().GetY() + pr->GetHeight() / 2;
			//if (bottom >= MSIZE) bottom = MSIZE - 1;
			left = pr->GetCenter().GetX() - pr->GetWidth() / 2;
			//if (left < 0) left = 0;
			right = pr->GetCenter().GetX() + pr->GetWidth() / 2;
			//if (right >= MSIZE) right = MSIZE - 1;

			isValidRoom = true;
			for (i = 0; i < counter && isValidRoom; i++)
				if (all_rooms[i].IsOverlap(*pr))
					isValidRoom = false;

		} while (!isValidRoom);

		// Draw room
		DrawRoom(right, left, bottom, top);
		all_rooms[counter] = *pr;
		all_rooms[counter].setRoomBounds(top, bottom, left, right);

		// setup medicine in the room
		if (counterMedicine < MAX_NUM_OF_MEDICINE)
		{
			int medicine = rand() % MAX_RAND;
			if (medicine == 1)
			{
				all_rooms[counter].SetHasMedicine(true);
				DrawMedicine(all_rooms[counter]);
				counterMedicine++;
			}
		}

		// setup ammo in the room
		if (counterAmmo < MAX_NUM_OF_AMMO)
		{
			int ammo = rand() % MAX_RAND;
			if (ammo == 1)
			{
				all_rooms[counter].SetHasAmmo(true);
				DrawAmmo(all_rooms[counter]);
				counterAmmo++;
			}
		}
	}

	DigTunnels();
	initWarriorInitialRoom(WARRIOR_1, warrior1, all_rooms[warriorRandRoom1]);
	initWarriorInitialRoom(WARRIOR_2, warrior2, all_rooms[warriorRandRoom2]);
	warriors.push_back(warrior1);
	warriors.push_back(warrior2);
}

void DrawMaze(vector<vector<int>> tmpWarriorMaze)
{
	int i, j;

	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
		{
			switch (tmpWarriorMaze[i][j])
			{
			case WALL:
				glColor3d(0.596, 0.596, 0.596); // DARK GREY
				break;
			case SPACE:
				glColor3d(1, 1, 1); // WHITE;
				break;
			case VISITED_W1:
				glColor3d(0, 0.9, 0); // GREEN;
				break;
			case VISITED_W2:
				glColor3d(0.5, 0.9, 0); // GREEN;
				break;
			case GRAY:
				glColor3d(1, .8, 0); // ORANGE;
				break;
			case MEDICINE:
				glColor3d(0.3, 0.3, 1); // LIGHT BLUE;
				break;
			case AMMO:
				glColor3d(0, 0.5, 0); // GREEN;
				break;
			case WARRIOR_1:
				glColor3d(0, 0, 0); // BLACK
				break;
			case WARRIOR_2:
				glColor3d(1, 0, 0); // RED;
				break;
			case WARRIOR1_PATH:
				glColor3d(1, 1, 1); // GREEN;
				break;
			case WARRIOR2_PATH:
				glColor3d(0.2, 0.9, 0.7); // GREEN;
				break;
			}

			// draw square
			glBegin(GL_POLYGON);
			glVertex2d(j*SQSIZE - 1 - SQSIZE / 2, i*SQSIZE /*- 1*/ + SQSIZE / 2);
			glVertex2d(j*SQSIZE /*- 1*/ + SQSIZE / 2, i*SQSIZE - 1 + SQSIZE / 2);
			glVertex2d(j*SQSIZE - 1 + SQSIZE / 2, i*SQSIZE - 1 - SQSIZE / 2);
			glVertex2d(j*SQSIZE - 1 - SQSIZE / 2, i*SQSIZE - 1 - SQSIZE / 2);
			glEnd();
		}

	

}

Point2D* findNearestTargetObjectForWarrior(Warrior& warrior)
{
	Point2D warriorLocation = warrior.getWarriorLocation();
	vector<Point2D>* objVector = nullptr;
	Warrior::Status warriorStatus = warrior.getPreviousTargetPointType();

	if (warriorStatus == Warrior::SEARCHING_FOR_MEDICINE)
	{
		if (medicine.size() > 0)
			objVector = &medicine;
		else
		{
			/*isGameOver = true;*/
			cout << "\nNo more medicine" << endl;
			return nullptr;
		}
	}
	else if (warriorStatus == Warrior::SEARCHING_FOR_AMMO)
	{
		if (ammo.size() > 0)
			objVector = &ammo;
		else
		{
			/*isGameOver = true;*/
			cout << "\nNo more ammo" << endl;
			return nullptr;
		}
	}
	else if (warriorStatus == Warrior::SEARCHING_FOR_ENEMY)
		for (int i = 0; i < warriors.size(); i++)
			if (&warrior != warriors[i])
			{	//check if the enemy in static position, else keep chasing it's previous static position
				if (warriors[i]->getWarriorStatus() == Warrior::IN_MOVEMENT || warriors[i]->getWarriorStatus() == Warrior::IN_BATTLE)
					if(&warrior.getPreviousTargetPoint())
						return &warrior.getPreviousTargetPoint();

				return &warriors[i]->getWarriorLocation();
			}



	Point2D* nearestObj = &(*objVector)[0];

	double minDist = warriorLocation.calcDistanceFromTarget(nearestObj);
	double tmpDist;

	for (int i = 1; i < (*objVector).size(); i++)
	{
		tmpDist = warriorLocation.calcDistanceFromTarget(&(*objVector)[i]);
		if (tmpDist < minDist)
		{
			nearestObj = &(*objVector)[i];
			minDist = tmpDist;
		}
	}

	return nearestObj;
}

void drawWarriorsInfo()
{
	glViewport(W, 0, W, H);
	glColor3d(0.2, 0.9, 0.7);
	glBegin(GL_POLYGON);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glVertex2d(-1, -1);
	glEnd();

	for (int i = 0; i < warriors.size(); i++)
	{
		string msg = warriors[i]->getMessage();
		if(i == 0)
			glColor3d(0, 0, 0);
		else
			glColor3d(1, 0, 0);
		
		glRasterPos2d(-0.9, 2 - i - 1 - 0.1);

		int len = msg.length();
		int lineCounter = 1;
		char* str = new char[len + 1];
		strcpy(str, msg.c_str());
		char* line = strtok(str, "\n");
		while (line != NULL)
		{
			for (int j = 0; j < (int)strlen(line); j++)
			{
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (line[j]));
			}
			glRasterPos2d(-0.9, 2 - i - 1 - (0.1 * ++lineCounter));
			line = strtok(NULL, "\n");
		}
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, W, H);
	if (drawWarrior1Maze)
		DrawMaze(warrior1->getWarriorMaze());
	else if (drawWarrior2Maze)
		DrawMaze(warrior2->getWarriorMaze());
	else
		DrawMaze(maze);

	drawWarriorsInfo();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

bool isWarriorChangedRoom(Warrior& warrior)
{
	Room* previousRoom = &warrior.getCurrentRoom();
	Point2D* currentWarriorLocation = &warrior.getWarriorLocation();

	for each (Room room in all_rooms)
	{
		if (room.isObjInRoom(*currentWarriorLocation))
			if (room != *previousRoom)
			{
				warrior.setCurrentRoom(room);
				return true;
			}
	}

	return false;

}

void deleteObjectFromMaze(Point2D& obj)
{
	int currentY = obj.GetY();
	int currentX = obj.GetX();

	//maze[currentY][currentX] = SPACE;
	maze[currentY - 1][currentX] = SPACE;
	maze[currentY + 1][currentX] = SPACE;
}

//checks if warrior reached its target, if it did then remove it from maze and vector
bool checkWarriorReachedObject(Warrior& warrior)
{
	Point2D* target = &warrior.getPreviousTargetPoint();
	Warrior::Status warriorStatus = warrior.getPreviousTargetPointType();
	vector<Point2D>::iterator it;

	if (warrior.getWarriorLocation() == *target)
	{
		deleteObjectFromMaze(*target);

		if (warriorStatus == Warrior::SEARCHING_FOR_MEDICINE)
		{
			it = find(medicine.begin(), medicine.end(), *target);
			medicine.erase(it);

			int medicineAmount = (rand() % (MAX_WARRIOR_HP / 2)) + MAX_WARRIOR_HP / 2;
			warrior.addMedicine(medicineAmount);
			return true;
		}
		else if (warriorStatus == Warrior::SEARCHING_FOR_AMMO)
		{
			it = find(ammo.begin(), ammo.end(), *target);
			ammo.erase(it);

			int ammoAmount = (rand() % (MAX_WARRIOR_BULLETS / 2)) + MAX_WARRIOR_BULLETS / 2;
			warrior.addAmmo(ammoAmount);
			return true;
		}


	}

	return false;

}

//in case that one of the warriors reached its target, check if that target is the other's warrior's target also
//if it is, change it
void changeWarriorTargetPoint(Warrior& otherWarrior, Point2D& irelevantPoint)
{
	if (otherWarrior.getPreviousTargetPoint() == irelevantPoint)	//this means that the other warrior found the same target and took it
		otherWarrior.setWarriorStatus(otherWarrior.getPreviousTargetPointType());

}

void handleWarriorInMovement(int warriorIndex)
{
	maze[warriors[warriorIndex]->getWarriorLocation().GetY()][warriors[warriorIndex]->getWarriorLocation().GetX()] = SPACE;
	warriors[warriorIndex]->moveWarriorByOne();
	maze[warriors[warriorIndex]->getWarriorLocation().GetY()][warriors[warriorIndex]->getWarriorLocation().GetX()] = warriors_color[warriorIndex][0];

	if (isWarriorChangedRoom(*warriors[warriorIndex]))
	{
		cout << "warrior: " << warriorIndex << " ,changed room" << endl;
		Point2D* target = findNearestTargetObjectForWarrior(*warriors[warriorIndex]);
		if (*target != warriors[warriorIndex]->getPreviousTargetPoint())
		{
			warriors[warriorIndex]->setWarriorStatus(warriors[warriorIndex]->getPreviousTargetPointType());
		}
	}

	if (checkWarriorReachedObject(*warriors[warriorIndex]))
	{
	//	warriors[warriorIndex]->setTragetPoint(Point2D());	//just for debug!!!DELETE!!!
		warriors[warriorIndex]->makeDecision();
		changeWarriorTargetPoint(*warriors[(warriorIndex + 1) % warriors.size()], warriors[warriorIndex]->getWarriorLocation());
	}


}

void idle()
{
	if (!isGameOver)
	{
		if (startGame)
		{
			for (int i = 0; i < warriors.size(); i++)
			{
				if (&warriors[i]->getPreviousTargetPoint())
					cout << "\n\nWarrior - " << (i == 0 ? "Black: \n" : "Red: \n") << *warriors[i];
				/*<< "position: (" << (warriors[i]->getWarriorLocation().GetX()) << ", " << (warriors[i]->getWarriorLocation().GetY())
				<< "\ntarget: (" << warriors[i]->getPreviousTargetPoint().GetX() << ", " << warriors[i]->getPreviousTargetPoint().GetY()
				<< "\n\n" << endl;*/

				Point2D* target = findNearestTargetObjectForWarrior(*warriors[i]);

				if (warriors[i]->getWarriorStatus() == Warrior::SEARCHING_FOR_MEDICINE)
				{
					if (target)
						warriors[i]->/*searchMedicine*/searchForTarget(*target, MEDICINE);
					else
					{
						warriors[i]->setNoMoreMedicineInGame(true);
						warriors[i]->makeDecision();
					}
				}
				else if (warriors[i]->getWarriorStatus() == Warrior::SEARCHING_FOR_AMMO)
				{
					if(target)
						warriors[i]->/*searchAmmo*/searchForTarget(*target, AMMO);
					else
					{
						warriors[i]->setNoMoreAmmoInGame(true);
						warriors[i]->makeDecision();
					}

				}
				else if (warriors[i]->getWarriorStatus() == Warrior::SEARCHING_FOR_ENEMY)
				{
					if (warriors[i]->checkWarriorsInTheSameRoom(*warriors[(i + 1) % warriors.size()]))
					{
					//	warriors[i]->setWarriorStatus(Warrior::IN_BATTLE);
						//shoot enemy should be invoked from makeDesicion method in Warrior
						if (warriors[i]->shootEnemy(*warriors[(i + 1) % warriors.size()]))
						{	
							if (warriors[(i + 1) % warriors.size()]->getLife() <= 0)
							{
								isGameOver = true;
								cout << "\n\nI Won!!!!!" << endl;
							}
						}
					}
					else
						warriors[i]->/*searchEnemy*/searchForTarget(*target, warriors[(i + 1) % warriors.size()]->getWarriorMazeColor());
				}
				else if (warriors[i]->getWarriorStatus() == Warrior::IN_MOVEMENT)
				{
					idleCounter++;
					if (idleCounter % 5 == 0)
					{
						handleWarriorInMovement(i);
					}
				}

				if (isGameOver)
					break;
			}
		}
	}
	else
		cout << "GAME OVER!!!" << endl;


	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	switch (choice)
	{
	case 1:
		drawWarrior2Maze = false;
		drawWarrior1Maze = true;
		break;
	case 2:
		drawWarrior1Maze = false;
		drawWarrior2Maze = true;
		break;
	case 3:
		drawWarrior1Maze = false;
		drawWarrior2Maze = false;
		break;
	case 4:
		startGame = true;
	default:
		break;
	}

}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W*2, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeoun - BattleField");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	init();

	glutCreateMenu(Menu);
	glutAddMenuEntry("Draw warrior black maze", 1);
	glutAddMenuEntry("Draw warrior red maze", 2);
	glutAddMenuEntry("Draw battlefield maze", 3);
	glutAddMenuEntry("Start", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}

