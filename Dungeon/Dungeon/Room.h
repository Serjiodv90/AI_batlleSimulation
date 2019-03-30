//
//  Room.hpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#pragma once
#include "Point2D.h"
#include "Definitions.h"

class Room
{
public:
    Room();
    ~Room();
private:
    Point2D center;
    int width, height;
    bool hasAmmo, hasMedicine;
	int top, bottom, left, right;

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

	void setRoomBounds(int top, int bottom, int left, int right);
	int getTop();
	int getBottom();
	int getLeft();
	int getRight();

	bool isObjInRoom(Point2D& obj);
	bool operator==(const Room& other);
	bool operator!=(const Room& other);

	double getMaxDistInRoom();
};


