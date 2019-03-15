//
//  CompareNodes.cpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#include "CompareNodes.h"

CompareNodes::CompareNodes()
{
}


CompareNodes::~CompareNodes()
{
}


bool CompareNodes::operator ( )(const Node& n1, const Node& n2)
{
    return n1.GetF()>n2.GetF();
}
