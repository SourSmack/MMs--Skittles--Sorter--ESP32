#pragma once
#include "etl/optional.h"


#define ITaskWrapperDerived static_cast<T&>( *this )
template <class T>
class ITask {
private: 
public:
    ~ITask(){ }  ;
    
    bool kill() { ITaskWrapperDerived.kill();} ;
    void notify( uint8_t message )  { ITaskWrapperDerived.notify( message ) ;}
    
    static etl::optional< T > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority){ return T::create( task , arg, stackSize, priority ); } 

    static bool notifyWait( uint8_t message , uint32_t delay ){ return T::notifyWait( message , delay) ; }
};

#undef ITaskWrapperDerived 

