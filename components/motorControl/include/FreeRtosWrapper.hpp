#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TaskWrapper.hpp"


class FreeRtosWrapper : public ITaskWrapper{
private:
    TaskHandle_t task{};
public:
    etl::optional< ITaskWrapper* > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority ) override ;
    bool kill() override  ;
    void notify( uint8_t message ) override  ;
    

};
