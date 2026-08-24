#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ITask.hpp"


class FreeRtosWrapper : public ITask<FreeRtosWrapper>{
private:
    TaskHandle_t task;
public:

        
        

    static etl::optional< FreeRtosWrapper > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;
    static bool notifyWait( uint8_t message , uint32_t delay );
    void notify( uint8_t message )   ;


    bool requestStop()  ;
    bool join();
    static bool stopRequested();
};
