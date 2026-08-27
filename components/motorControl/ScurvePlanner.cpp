#include "include/ScurvePlanner.hpp"
#include "scurve.h"


ScurvePlanner::ScurvePlanner():movesQ{} , motionsQ{} {}

etl::optional< ScurvePlanner >  ScurvePlanner::create( ){
    ScurvePlanner tmp{};

    tmp.motionsQ = xQueueCreate( MAXBUFFOR , sizeof(motionBlock_t) );
    if (! tmp.motionsQ) return etl::nullopt ;

    if ( xTaskCreate( scurveTask , "scurveTask" , 1024 , &tmp , 4  , &tmp.scurveTaskHandle ) != pdPASS ) return etl::nullopt ;
    return tmp ;
}

void scurveTask(void * arg){
    xTaskNotifyWait( );
    
}
void ScurvePlanner::calculateFrequency(const moveBlock_t &move)   
{


} 


motionBlock_t recieve(){
    motionBlock_t motion ;
    xQueueReceive( motionsQ , &motion,  pdMS_TO_TICKS( 10 ) ) ;
    return motion ;
};