#pragma once

#include <concepts>
#include "moveStructures.hpp"
#include "etl/optional.h"


class IPlanner 
{
private:
public:
    virtual ~IPlanner(void) = default ;
    // whole function should be atomic to prevent inconsistency in moves turn
    virtual void calculateFrequency(const moveBlock_t &move ) = 0 ;


};




template< 
    class T , 
    class ... Args 
> 
concept StaticFactoryPlanner = 
    std::derived_from<T , IPlanner> && 
    requires(Args&& ... args ){
    {  T::create( std::forward< Args >( args) ... ) } -> std::same_as< etl::optional< T >> ; 
};