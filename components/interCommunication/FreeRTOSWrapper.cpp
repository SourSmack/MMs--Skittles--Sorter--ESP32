#include "FreeRTOSWrapper.hpp"



// allocate resources but do not start yet task 
etl::optional< FREETask > FREETask::create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {

//    xTaskCreate(task , "task" , stackSize , arg , priority , &taskHandle );
    return etl::nullopt;
}
void FREETask::notify( uint8_t message ){

}
    

bool FREETask::stopRequested( uint32_t token  ){
    return true;
}

bool FREETask::notifyWait(uint32_t token , uint8_t message , uint32_t delay ){
    uint32_t taskBits{0};
    xTaskNotifyWait(0x00 , message , &taskBits , delay) ; 
    return taskBits & ~message ; 
}  

void FREETask::waitMs(uint32_t token,  uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms )  );
}

// create thread 
bool FREETask::start(){ 

    return true;

}

bool FREETask::stop(){ 

    return true;

}