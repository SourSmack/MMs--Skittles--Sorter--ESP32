

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>

#include "FreeRTOSWrapper.hpp"

class ScurvePlanner 
{
private:
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< moveBlock_t,  MAXBUFFOR > movesQ{} ; 
    
    FREETask  taskHandle ;
    
    static void scurveTask(void * arg);
public:
   
    ScurvePlanner(): taskHandle( FREETask::create( scurveTask , this , 2048 , 4 )) {}
   // static etl::optional< ScurvePlanner >  create();
    motionBlock_t calculateFrequency(const moveBlock_t &move   , motionBlock_t &destQue)   ; 
    bool stop();
    bool start();
    void enqueue( const moveBlock_t motion);
    
};