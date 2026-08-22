#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ITaskWrapper.hpp"


class FreeRtosWrapper : public ITaskWrapper<FreeRtosWrapper>{
private:
    TaskHandle_t task{};
public:
    bool kill()   ;
    void notify( uint8_t message )   ;
        
        

    static etl::optional< FreeRtosWrapper > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;
    static bool notifyWait( uint8_t message , uint32_t delay );


};
