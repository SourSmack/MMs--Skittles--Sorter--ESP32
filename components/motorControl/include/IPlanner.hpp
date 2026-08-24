#pragma once
#include "moveStructures.hpp"
#include "etl/optional.h"

#define IPlannerDeriv  static_cast< T& >( * this) 

template< class T>
class IPlanner 
{
protected:
    IPlanner(){}
public:
    ~IPlanner() = default   ;
    // whole function should be atomic to prevent inconsistency in moves turn
    void calculateFrequency(const moveBlock_t &move )  {
        IPlannerDeriv.calculateFrequency( move ) ;
    }
    consteval static etl::optional<T> create( ){  return T::create( ) ; }
    

};

#undef IPlannerDeriv
