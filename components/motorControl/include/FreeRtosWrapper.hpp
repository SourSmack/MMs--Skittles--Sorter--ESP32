#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TaskWrapper.hpp"


class FreeRtosWrapper : public TaskWrapper{
private:
    TaskHandle_t task{};
public:
    etl::optional< TaskWrapper* > create() override ;
    bool kill() override  ;
    void notify( uint8_t message ) override  ;
    

};
