#include "FreeRtosWrapper.hpp"




etl::optional< ITaskWrapper* > FreeRtosWrapper::create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {

    xTaskCreate(task , "task" , stackSize , arg , priority , &taskHandle );
}
bool FreeRtosWrapper::kill()   {

}
void FreeRtosWrapper::notify( uint8_t message ){

}
    


namespace thisTask { 
    bool notifyWait( uint8_t message , uint32_t delay ){

        return true;
    }
}