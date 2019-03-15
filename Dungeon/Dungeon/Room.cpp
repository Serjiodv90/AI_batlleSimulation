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
