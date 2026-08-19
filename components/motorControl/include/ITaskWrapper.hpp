#pragma once
#include "etl/optional.h"
#include <concepts>


class ITaskWrapper {
private: 
public:
    virtual ~ITaskWrapper() = 0 ;
    
    virtual bool kill() = 0  ;
    virtual void notify( uint8_t message ) = 0  ;
    
    
};

namespace thisTask { 
    bool notifyWait( uint8_t message , uint32_t delay );
};



template <
    class T , 
    class ... Args
> 
concept StaticFactoryTaskWrapper  = 
    std::derived_from<T , ITaskWrapper> && 
    requires( Args&& ... args ){
    { T::create( std::forward< Args >( args ) ... )} -> std::same_as<etl::optional< T >>;
};
