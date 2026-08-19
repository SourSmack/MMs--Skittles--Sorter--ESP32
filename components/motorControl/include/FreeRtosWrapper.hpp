#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ITaskWrapper.hpp"


class FreeRtosWrapper : public ITaskWrapper{
private:
    TaskHandle_t task{};
public:
    static etl::optional< FreeRtosWrapper > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;
    bool kill() override  ;
    void notify( uint8_t message ) override  ;
    

};
