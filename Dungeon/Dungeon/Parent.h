//
//  Parent.hpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#pragma once
#include "Point2D.h"
class Parent
{
public:
    Parent();
    ~Parent();
private:
    Point2D current;
    Point2D prev;
    bool has_prev;
public:
    Parent(Point2D c, Point2D p, bool has);
    bool operator==(Parent other);
    bool HasParent();
    Point2D GetPrev();
    Point2D GetCurrent();
};


