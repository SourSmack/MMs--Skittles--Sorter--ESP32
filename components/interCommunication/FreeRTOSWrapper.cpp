#include "FreeRTOSWrapper.hpp"

unsigned long FREETask::MAX_DELAY = portMAX_DELAY;

etl::optional< FREETask > FREETask::create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {
    FREETask tmp{ task ,arg , stackSize , priority } ;
    
    return tmp;
}

bool FREETask::notify( const uint32_t message )   {
    auto result = xTaskNotify( taskHandle , message , eSetBits ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}

bool FREETask::requestStop()  {
    auto result = xTaskNotify( taskHandle , stopBit , eSetBits  ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}
bool FREETask::join(){
    if ( !taskRunning ) return true ;
    if ( !requestStop() ) return false ;
    while ( taskRunning ){ taskYIELD(); }
    return true  ;
}

bool FREETask::start() {
    auto result  = xTaskCreate( _task , "task" , stackSize , arg  , priority , &taskHandle ) ;
    if ( result != pdPASS ) return false ; 
    return true; 
} 


bool FREETask::stopRequested(   ){
    uint32_t result { 0 } ;
    if ( !xTaskNotifyWait( 0 , stopBit ,  &result , 0  )) return false ;
    if ( result & stopBit ) return true ;
    return false ;
}
bool FREETask::waitForNotify( const uint32_t waitBitMask , const uint32_t p_delay ){
    uint32_t result { 0 } ;
    auto delay = ( p_delay == MAX_DELAY ) ? (  portMAX_DELAY ) : pdMS_TO_TICKS( p_delay ) ;
    if ( !xTaskNotifyWait( 0 ,  waitBitMask , &result , delay )) return false ;

    if ( result & waitBitMask )  return true ;

    return false  ;

}

void FREETask::waitMS( const uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms ) ) ;
}
