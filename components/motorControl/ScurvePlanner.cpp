#include "include/ScurvePlanner.hpp"
#include "scurve.h"
/* ScurvePlanner(): taskHandle( FREETask::create( scurveTask , this , 2048 , 4 )) {}
   // static etl::optional< ScurvePlanner >  create();
    motionBlock_t calculateFrequency(const moveBlock_t &move   , motionBlock_t &destQue)   ; 
    bool stop();
    bool start();
    void enqueue( const moveBlock_t motion);
  */  


etl::optional< ScurvePlanner >  ScurvePlanner::create( ){
    ScurvePlanner tmp{};

    tmp.motionsQ = xQueueCreate( MAXBUFFOR , sizeof(motionBlock_t) );
    if (! tmp.motionsQ) return etl::nullopt ;

    if ( xTaskCreate( scurveTask , "scurveTask" , 1024 , &tmp , 4  , &tmp.scurveTaskHandle ) != pdPASS ) return etl::nullopt ;
    return tmp ;
}

void scurveTask(void * arg){
    auto& [ instance, token  ] = *static_cast< etl::pair< ScurvePlanner* , uint16_t >*>( arg ) ;
    auto& [ MAXBUFFOR , movesQ , taskHandle ] = instance ;
    

    while ( !movesQ.empty()  && !FREETask::stopRequested( token )){

    }
    
    
}
void ScurvePlanner::calculateFrequency(const moveBlock_t &move)   
{


} 

