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
    auto& [ instance ]   = *static_cast< ScurvePlanner* >( arg ) ;
    auto& [ MAXBUFFOR , movesQ , enqueueMotion  taskHandle ] = instance ;
    

    while ( !FREETask::stopRequested()){
        if ( FREETask::waitForNotify( ScurvePlanner<T>::pauseBit , 0 )){
            while( FREETask::waitForNotify( ScurvePlanner<T>::resumeBit , 0 ){
                FREETask::yield();
            } 
        }
        auto motion = calculateFrequency< motionBlock_t >( movesQ.front() ; ) ;
        movesQ.pop_front() ;
        enqueueMotion( motion );


    }
    
    
}



template < class T > 
T calculateFrequency(const moveBlock_t &move , void * desQue = nullptr   ) {
    static int iteration{ 0 } ;
    

} 

bool stop(){
    if ( taskHandle.stop() ) return true ;
    return false ;
}
bool start(){
}
bool pause(){
    if ( taskHandle.isRunning() ) return true ;
    
    

}

bool resume() {

}

void enqueue( const moveBlock_t motion){

}
    