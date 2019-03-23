//
//  Point2D.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#include "Point2D.h"


Point2D::Point2D()
: x(0)
{
}


Point2D::~Point2D()
{
}


Point2D::Point2D(int x, int y)
{
    this->x = x;
    this->y = y;
}


int Point2D::GetX() const
{
    return x;
}


int Point2D::GetY() const
{
    return y;
}


bool Point2D::operator==(const Point2D& other)
{
	if(this != nullptr && &other != nullptr)
		return x==other.x && y== other.y;
	return false;
}

bool Point2D::operator!=(const Point2D & other)
{
	if (this != nullptr && &other != nullptr)
		return !(*this == other);
	return false;
}

double Point2D::calcDistanceFromTarget(Point2D * targetPoint)
{
	double xPow = pow(this->x - targetPoint->x, 2);
	double yPow = pow(this->y - targetPoint->y, 2);
	this->h = sqrtl(xPow + yPow);
	return this->h;
}



void Point2D::set_g(int levelOfPointInTree)
{
	this->g = levelOfPointInTree;
}

double Point2D::get_g()
{
	return this->g;
}

double Point2D::get_h()
{
	return this->h;
}

void Point2D::set_f(Point2D* targetPoint, int levelOfPointInTree)
{
	set_g(levelOfPointInTree);
	calcDistanceFromTarget(targetPoint);
	this->f = this->g + this->h;
}

double Point2D::get_f()
{
	return this->f;
}

