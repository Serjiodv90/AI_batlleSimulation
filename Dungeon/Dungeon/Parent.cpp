//
//  Parent.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#include "Parent.h"

Parent::Parent()
{
    has_prev = false;
}


Parent::~Parent()
{
}


Parent::Parent(Point2D c, Point2D p, bool has)
{
    current = c;
    prev = p;
    has_prev = has;
}


bool Parent::operator==(Parent other)
{
    return current == other.current;
}


bool Parent::HasParent()
{
    return has_prev;
}


Point2D Parent::GetPrev()
{
    return prev;
}


Point2D Parent::GetCurrent()
{
    return current;
}
