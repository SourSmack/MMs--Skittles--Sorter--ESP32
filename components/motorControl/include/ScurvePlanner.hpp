

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <IPlanner.hpp>
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>


class ScurvePlanner 
{
private:
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< moveBlock_t,  MAXBUFFOR > movesQ{} ; 
    
    QueueHandle_t  motionsQ  { nullptr };
    static void scurveTask(void * arg);
    TaskHandle_t scurveTaskHandle {nullptr};

    ScurvePlanner() = default ;

    
public:
    
    static etl::optional< ScurvePlanner >  create();
    motionBlock_t calculateFrequency(const moveBlock_t &move)   ; 
    void stop();
    void start();
    void enqueue( const motionBlock_t motion);
    motionBlock_t recieve();
};