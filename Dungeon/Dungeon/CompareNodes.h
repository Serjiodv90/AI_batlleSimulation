//
//  CompareNodes.hpp
//  DungeonProject
//
//  Created by Hadar Pur on 05/02/2019.
//  Copyright © 2019 Hadar Pur. All rights reserved.
//

#pragma once
#include "Node.h"
class CompareNodes
{
public:
    CompareNodes();
    ~CompareNodes();
    bool operator ( ) (const Node& n1, const Node& n2);
};


