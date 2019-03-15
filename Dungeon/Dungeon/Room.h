//
//  Room.hpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#pragma once
#include "Point2D.h"
class Room
{
public:
    Room();
    ~Room();
private:
    Point2D center;
    int width, height;
    bool hasAmmo, hasMedicine;
public:
    Room(const Point2D& center_point, int w, int h);
    Point2D GetCenter() const;
    int GetWidth();
    int GetHeight();
    void SetHasAmmo(bool value);
    void SetHasMedicine(bool value);
    bool GetHasAmmo();
    bool GetHasMedicine();
    bool IsOverlap(const Room& other);
};


