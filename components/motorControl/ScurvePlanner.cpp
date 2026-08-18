#include "include/ScurvePlanner.hpp"
#include "scurve.h"


ScurvePlanner::ScurvePlanner():movesQ{} , motionsQ{} {}

etl::optional< ScurvePlanner >  ScurvePlanner::create(){
    ScurvePlanner tmp{};

    tmp.motionsQ = xQueueCreate( MAXBUFFOR , sizeof(motionBlock_t) );
    if (! tmp.motionsQ) return etl::nullopt ;

    xTaskCreate( scurveTask , "scurveTask" , 1024 , &tmp , 4  , &tmp.scurveTaskHandle );
    return tmp ;
}

void scurveTask(void * arg){

}
void ScurvePlanner::calculateFrequency(const moveBlock_t &move)   
{


} 