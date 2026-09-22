#pragma once
#include "etl/optional.h"
#include "etl/vector.h"
#include "etl/atomic.h"
#include "FreeRTOSWrapper.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


class FREETask {
private:

    static etl::vector<  FREETask& , 16 > tokensLookup ; 
private:
    TaskHandle_t task;
    etl::atomic< bool > taskRunning { false } ;

    bool _stopRequested { false } ;
    uint32_t notifyMessage { 0 } ;


    void * arg { nullptr } ;

    static uint32_t MAX_DELAY ;
public:

    ~FREETask(){  if ( taskRunning) { requestStop() ;  join() ;  }}  ;
    
    FREETask(): tokensLookup.push_back( this )  {}
        

    static etl::optional< FREETask > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {
        FREETask tmp ;
        tmp.set( task , arg ,stackSize , priority) ;
        return etl::nullopt;
    }
    bool set( void (*task)(void*arg) , void * p_arg ,  const uint32_t stackSize , const uint32_t priority ) {
        arg = p_arg  ;
    } 
    void notify( uint8_t message )   ;
    bool requestStop()  ;
    bool join();

    static bool stopRequested( uint32_t token  ){
        if ( tokensLookup[ token ]._stopRequested ) return true ;
        return false ;
    }
    static bool notifyWait( uint32_t token ,   uint8_t message , uint32_t delay ){
        
        auto& instance = tokensLookup[ token ];
        instance.task

    }

    /*   looks up some global std::pair structure to see which FREEtask instance is resposible for that TOKEN 
        
    */
    static void waitMs( uint32_t token  ,  uint32_t ms ) ;
    
    bool start() ; 
    bool stop() ; 
};
