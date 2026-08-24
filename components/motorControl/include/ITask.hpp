#pragma once
#include "etl/optional.h"
#include "etl/atomic.h"


#define ITaskWrapperDerived static_cast<T&>( *this )
template <class T>
class ITask {
private: 
    etl::atomic< bool > taskRunning { false } ;

public:
    static etl::optional< T > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority){ return T::createTask( task , arg, stackSize, priority ); } 

    void notify( uint8_t message )  { ITaskWrapperDerived.notify( message ) ;}
    static bool notifyWait( uint8_t message , uint32_t delay ){ return T::notifyWait( message , delay) ; }

    bool requestStop() { return ITaskWrapperDerived.requestStop();} ;
    bool join(){ return ITaskWrapperDerived.join() ; }
    static bool stopRequested(){ return T::stopRequested() ; }
    ~ITask(){  if ( taskRunning) { requestStop() ;  join() ;  }}  ;
};

#undef ITaskWrapperDerived 

