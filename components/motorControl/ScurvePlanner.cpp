#include "include/ScurvePlanner.hpp"
#include "scurve.h"
/* ScurvePlanner(): taskHandle( FREETask::create( scurveTask , this , 2048 , 4 )) {}
   // static etl::optional< ScurvePlanner >  create();
    motionBlock_t calculateFrequency(const moveBlock_t &move   , motionBlock_t &destQue)   ; 
    bool stop();
    bool start();
    void enqueue( const moveBlock_t motion);
  */  

template< class T>
etl::optional< ScurvePlanner<T>>  ScurvePlanner<T>::create(   T&& enqueueFunc   ){
    ScurvePlanner tmp{ enqueueFunc };
    
    return tmp ;
}

template< class T>
void scurveTask(void * arg){
    auto&  [ instance , enqueueFunc  ]   = *static_cast< ScurvePlanner* >( arg ) ;
    auto& [ MAXBUFFOR , movesQ , args  taskHandle ] = instance ;
    

    while ( !FREETask::stopRequested()){
        if ( FREETask::waitForNotify( ScurvePlanner<T>::pauseBit , 0 )){
            while( FREETask::waitForNotify( ScurvePlanner<T>::resumeBit , 0 ){
                taskYIELD();
            } 
        }
    }
    
    
}



motionBlock_t calculateFrequency(const moveBlock_t &move   , motionBlock_t &destQue)   ; 
bool stop(){
    if ( taskHandle.stop() ) return true ;
    return false ;
}
bool start(){
    if ( tastHandle.start()) return true ;
    return false; 
}
bool pause(){
    if ( taskHandle.isRunning() ) return true ;
    
    

}

bool resume() {

}

void enqueue( const moveBlock_t motion){

}
    