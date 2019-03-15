//
//  main.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

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
#include "Definitions.h"

using namespace std;



int maze[MSIZE][MSIZE];
bool bfs_started = false;
int counterAmmo = 0;
int counterMedicine = 0;

Room all_rooms[NUM_ROOMS];
Warrior warrior1;
Warrior warrior2;

// vector for tunnels
vector <Point2D> gray;
vector <Point2D> black;
vector <Parent> parents;

// medicine verctor
vector <Point2D> medicine;
// ammo verctor
vector <Point2D> ammo;

priority_queue<Node, vector<Node>, CompareNodes> pq;

Point2D start,target;

void SetupMaze();

void init()
{
    int i, j;
    
    srand(time(0));
    
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
    
    if ((direction==UP && current.GetPoint().GetY() > 0) ||
        (direction == DOWN && current.GetPoint().GetY() < MSIZE-1) ||
        (direction == LEFT && current.GetPoint().GetX() > 0) ||
        (direction == RIGHT && current.GetPoint().GetX() < MSIZE - 1))
    {
        pt = new Point2D(current.GetPoint().GetX() + dx, current.GetPoint().GetY() + dy);
        gray_it = find(gray.begin(), gray.end(), *pt);
        black_it = find(black.begin(), black.end(), *pt);
        if (gray_it == gray.end() && black_it == black.end()) // this is a new point
        {
            // very important to tunnels
            if (maze[current.GetPoint().GetY() +dy][current.GetPoint().GetX()+dx] == WALL)
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
    double space_weight = 0.5, wall_weight = 0.5,weight;
    
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
    
    for(i=0;i<NUM_ROOMS;i++)
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
	int left = room.GetCenter().GetX() - room.GetWidth() / 2;
	int right = room.GetCenter().GetX() + room.GetWidth() / 2;
	int top = room.GetCenter().GetY() - room.GetHeight() / 2;
	int bottom = room.GetCenter().GetY() + room.GetHeight() / 2;
	bool noCollision = false;

	int obj_x, obj_y;
	do
	{
		obj_x = (int)(rand() % ((right - 1) - (left + 1)) + left+1);
		obj_y = (int)(rand() % ((bottom - 1) - (top + 1)) + top+1);
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

void initWarriorInitialRoom(int warrior, Warrior& warriorObj, Room& room)
{
	Point2D tmpCenter = findPositionForObjectInRoom(room);
	int currentY = tmpCenter.GetY();
	int currentX = tmpCenter.GetX();
	maze[currentY][currentX] = warrior;
	warriorObj = Warrior(Point2D(currentX, currentY), 0, reinterpret_cast<int**>(maze));
}

void SetupMaze()
{
    int i, counter;
    int left, right, top, bottom;
    bool isValidRoom;
    
    int warriorRandRoom1 = (int) (rand() % NUM_ROOMS);
    int warriorRandRoom2 = (int) (rand() % NUM_ROOMS);
    
    Room* pr=NULL;
    
    for (counter = 0; counter < NUM_ROOMS; counter++)
    {
        // create room
        do
        {
            free(pr);
            pr = new Room(Point2D(rand()%MSIZE,rand() % MSIZE), 5 + rand() % 15, 5 + rand() % 25);
            top = pr->GetCenter().GetY() - pr->GetHeight() / 2;
            if (top < 0) top = 0;
            bottom = pr->GetCenter().GetY() + pr->GetHeight() / 2;
            if (bottom >= MSIZE) bottom = MSIZE - 1;
            left = pr->GetCenter().GetX() - pr->GetWidth() / 2;
            if (left < 0) left = 0;
            right = pr->GetCenter().GetX() + pr->GetWidth() / 2;
            if (right >= MSIZE) right = MSIZE - 1;
            
            isValidRoom = true;
            for (i = 0; i < counter && isValidRoom; i++)
                if (all_rooms[i].IsOverlap(*pr))
                    isValidRoom = false;
            
        } while (!isValidRoom);
        
		// Draw room
		DrawRoom(right, left, bottom, top);
		all_rooms[counter] = *pr;

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
        
        
        if (counter == warriorRandRoom1)
            initWarriorInitialRoom(WARRIOR_1, warrior1, all_rooms[counter]);

		if (counter == warriorRandRoom2)
			initWarriorInitialRoom(WARRIOR_2, warrior2, all_rooms[counter]);
    }
    
    DigTunnels();
}

void DrawMaze()
{
    int i, j;
    
    for(i = 0; i < MSIZE; i++)
        for (j = 0; j < MSIZE; j++)
		{
            switch (maze[i][j])
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
                    glColor3d(1, 0, 0); // DARK RED;
                    break;
				case WARRIOR1_PATH:
					glColor3d(0,0,0); // GREEN;
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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawMaze();
    
    glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	if(warrior1.getWarriorStatus() == Warrior::SEARCHING_FOR_MEDICINE)
		warrior1.searchMedicine(medicine[0], warrior1.getWarriorLocation(), maze, MEDICINE, VISITED_W1, WARRIOR_1, WARRIOR1_PATH);
		
    glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
    
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Digits Example ");
    
    glutDisplayFunc(display); // refresh function
    glutIdleFunc(idle); // idle: when nothing happens
    init();
    
    glutCreateMenu(Menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    glutMainLoop();
    return 0;
}

