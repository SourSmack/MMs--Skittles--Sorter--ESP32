#include "FreeRTOSWrapper.hpp"




etl::optional< Task > Task::create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {

//    xTaskCreate(task , "task" , stackSize , arg , priority , &taskHandle );
    return etl::nullopt;
}
bool Task::kill()   {
    return true ;
}
void Task::notify( uint8_t message ){

}
    


bool Task::notifyWait( uint8_t message , uint32_t delay ){
    uint32_t taskBits{0};
    xTaskNotifyWait(0x00 , message , &taskBits , delay) ; 
    return taskBits & ~message ; 
}  

void waitMs( uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms )  );
}