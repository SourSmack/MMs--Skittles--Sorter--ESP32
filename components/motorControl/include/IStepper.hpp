
#pragma once
#include "moveStructures.hpp"
#include "etl/optional.h"
#include "config.hpp"

#define derived static_cast<T>(*this) 
template < class T>
class IStepper {
private:

protected:
    consteval IStepper(){}
public:


    ~IStepper() =  default ;

    void enqueue( motionBlock_t  motion) { derived.enqueue(  motion ) ; }
    
    void stop() {  derived.stop() ; } 
    void start() {  derived.start() ; }
    static etl::optional< T > create(  uint16_t step , uint16_t dir ){ return T::create( step, dir ) ; }
};

