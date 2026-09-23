#pragma once
#include "etl/optional.h"
#include "etl/vector.h"
#include "etl/atomic.h"
#include "FreeRTOSWrapper.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


class FREETask {
private:

    static etl::vector<  FREETask* , 16 > tokensLookup ; 

    TaskHandle_t taskHandle { nullptr };

    etl::atomic< bool > taskRunning { false } ;



    void (*task)(void*arg) ;
    void * arg ;
    uint32_t stackSize ; 
    uint32_t priority ;

    static uint32_t MAX_DELAY ;
public:

    ~FREETask(){  if ( taskRunning) { requestStop() ;  join() ;  }}  ;
    
    FREETask(  void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )
        : task( task ) , arg( arg ) , stackSize( stackSize ) , priority( priority ){
            tokensLookup.push_back( this ) ;
        }
        

    static etl::optional< FREETask > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {
        FREETask tmp{ task ,arg , stackSize , priority } ;
        
        return etl::nullopt;
    }
    /*bool set( void (*task)(void*arg) , void * p_arg ,  const uint32_t stackSize , const uint32_t priority ) {
        arg = p_arg  ;
    } */
    void notify( uint8_t message )   ;
    bool requestStop()  ;
    bool join();

    bool start() ; 
    bool stop() ; 




    static bool stopRequested(  uint32_t stopBitMask ){
        uint32_t result { 0 } ;
        if ( !xTaskNotifyWait( 0 , stopBitMask ,  &result , 0  )) return false ;
        if ( result & stopBitMask ) return true ;
        return false ;
    }
    static bool waitForNotify(    uint32_t waitBitMask , uint32_t p_delay ){
        
        
        uint32_t result { 0 } ;
        auto delay = ( p_delay == MAX_DELAY ) ? (  portMAX_DELAY ) : pdMS_TO_TICKS( p_delay ) ;
        if ( !xTaskNotifyWait( 0 ,  waitBitMask , &result , delay )) return false ;

        if ( result & waitBitsMask )  return true ;
 
        return false  ;

    }

    static void waitMs(  uint32_t ms ) {
        vTaskDelay( pdMS_TO_TICKS( ms ) ) ;
    }
    
};
