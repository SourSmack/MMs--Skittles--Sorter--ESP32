#pragma once
#include "etl/optional.h"
#include <concepts>

template <class T , class ... Args> 
concept StaticFactoryTaskWrapper  = requires( Args&& .. args ){
    { T::create( std::forward< Args >( args ) ... )} -> std::same_as<etl::optional< T >>;
};


class ITaskWrapper {
private: 
public:
    virtual ~ITaskWrapper() = 0 ;
    virtual etl::optional< ITaskWrapper* > create(void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority ) = 0 ;
    virtual bool kill() = 0  ;
    virtual void notify( uint8_t message ) = 0  ;
    
    
};

namespace thisTask { 
    bool notifyWait( uint8_t message , uint32_t delay );
}