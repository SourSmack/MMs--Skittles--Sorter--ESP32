#pragma once

#include <concepts>
#include "moveStructures.hpp"
#include "etl/optional.h"

template< typename T > 
concept     StaticFactoryPlanner = requires( ){
    { T::create() } -> std::same_as< etl::optional< T >> ; 
};

class IPlanner 
{
private:
public:
    virtual ~IPlanner(void) = default ;
    // whole function should be atomic to prevent inconsistency in moves turn
    virtual void calculateFrequency(const moveBlock_t &move ) = 0 ;


};