

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <IPlanner.hpp>
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>


class ScurvePlanner : public IPlanner
{
private:
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< moveBlock_t,  MAXBUFFOR > movesQ{} ; 
    
    QueueHandle_t  motionsQ  { nullptr };
    static void scurveTask(void * arg);
    TaskHandle_t scurveTaskHandle {nullptr};

    ScurvePlanner();

    
public:
    
    static etl::optional< ScurvePlanner >  create();
    void calculateFrequency(const moveBlock_t &move) override  ; 
    void stop();
    void start();
    void enqueue();
};