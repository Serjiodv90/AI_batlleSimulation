//
//  Room.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#include "Room.h"
#include <math.h>

Room::Room()
{
}


Room::~Room()
{
}


Room::Room(const Point2D& center_point, int w, int h)
{
    center = center_point;
    width = w;
    height = h;
    hasAmmo = false;
    hasMedicine = false;
}


Point2D Room::GetCenter() const
{
    return center;
}


int Room::GetWidth()
{
    return width;
}


int Room::GetHeight()
{
    return height;
}

void Room::SetHasAmmo(bool value)
{
    hasAmmo = value;
}

void Room::SetHasMedicine(bool value)
{
    hasMedicine = value;
}

bool Room::GetHasAmmo()
{
    return hasAmmo;
}

bool Room::GetHasMedicine()
{
    return hasMedicine;
}

bool Room::IsOverlap(const Room& other)
{
    return abs(center.GetX()-other.GetCenter().GetX()) < (width+other.width)/2+5 && abs(center.GetY() - other.GetCenter().GetY()) < (height+other.height)/2+5;    
}

void Room::setRoomBounds(int top, int bottom, int left, int right)
{
	this->top = top;
	this->bottom = bottom;
	this->left = left;
	this->right = right;
}

int Room::getTop()
{
	return this->top;
}

int Room::getBottom()
{
	return this->bottom;
}

int Room::getLeft()
{
	return this->left;
}

int Room::getRight()
{
	return this->right;
}

bool Room::isObjInRoom(Point2D & obj)
{
	int obj_x = obj.GetX();
	int obj_y = obj.GetY();

	if (obj_x >= this->left && obj_x <= this->right
		&& obj_y >= this->top && obj_y <= this->bottom)
		return true;

	return false;
}

bool Room::operator==(const Room & other)
{
	if (this && &other)
		return this->center == other.center;
	else
		return false;
}

bool Room::operator!=(const Room & other)
{
	return !(*this == other);
}

double Room::getMaxDistInRoom()
{
	return sqrtf(powf(MIN_ROOM_WIDTH_AND_HEIGHT + MAX_ROOM_WIDTH, 2) +
		powf(MIN_ROOM_WIDTH_AND_HEIGHT + MAX_ROOM_HEIGHT, 2));
}
