#include "Mission.hpp"

unsigned long Mission::MAX_DELAY = portMAX_DELAY;

etl::optional< Mission > Mission::create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {
    Mission tmp{ task ,arg , stackSize , priority } ;
    
    return tmp;
}

 void Mission::_task( void * arg ){
        auto &instance=  *static_cast<Mission*>( arg ) ;
        instance.taskRunning = true ;
        instance.task( arg  ) ;

        vTaskDelete( instance.taskHandle ) ;
        instance.taskRunning = false ;

    }

bool Mission::notify( const uint32_t message )   {
    auto result = xTaskNotify( taskHandle , message , eSetBits ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}

bool Mission::requestStop()  {
    auto result = xTaskNotify( taskHandle , stopBit , eSetBits  ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}
bool Mission::join(){
    if ( !taskRunning ) return true ;
    if ( !requestStop() ) return false ;
    while ( taskRunning ){ Mission::yield(); }
    return true  ;
}

bool Mission::start() {
    if ( taskRunning) return true ;
    auto result  = xTaskCreate( _task , "task" , stackSize , arg  , priority , &taskHandle ) ;
    if ( result != pdPASS ) return false ; 
    return true; 
} 


bool Mission::stopRequested(   ){
    uint32_t result { 0 } ;
    if ( !xTaskNotifyWait( 0 , stopBit ,  &result , 0  )) return false ;
    if ( result & stopBit ) return true ;
    return false ;
}
bool Mission::waitForNotify( const uint32_t waitBitMask , const uint32_t p_delay ){
    uint32_t result { 0 } ;
    auto delay = ( p_delay == MAX_DELAY ) ? (  portMAX_DELAY ) : pdMS_TO_TICKS( p_delay ) ;
    if ( !xTaskNotifyWait( 0 ,  waitBitMask , &result , delay )) return false ;

    if ( result & waitBitMask )  return true ;

    return false  ;

}

void Mission::waitMS( const uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms ) ) ;
}


void Mission::yield(){
    taskYIELD() ;
}

bool Mission::isRunning(){ return taskRunning ; } 
