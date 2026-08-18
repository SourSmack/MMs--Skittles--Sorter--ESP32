#pragma once


#include "moveStructures.hpp"

class IPlanner 
{
private:
public:
    virtual ~IPlanner(void) = default ;
    // whole function should be atomic to prevent inconsistency in moves turn
    virtual void calculateFrequency(const moveBlock_t &move ) = 0 ;


};