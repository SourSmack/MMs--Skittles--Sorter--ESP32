#pragma once
#include <concepts> 
#include "etl/optional.h"

class ISensor 
{
private:

public:
    virtual ~ISensor() = default ;

    virtual bool turnOn() = 0 ;
    virtual bool turnOff() = 0 ;
    virtual const void* getSample()const  = 0 ;
    virtual bool stopListening_IT()  = 0   ;

    virtual bool listen_IT() = 0   ;


};



template < 
    class T ,
    class ... Args
> 
concept StaticFactorySensor  = 
    std::derived_from<T , ISensor>&& 
    requires(Args&& ... args){
    { T::create( std::forward< Args >( args) ... )} -> std::same_as< etl::optional< T >> ; 
};

