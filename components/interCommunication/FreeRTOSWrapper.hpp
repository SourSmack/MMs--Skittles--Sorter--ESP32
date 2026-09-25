#pragma once
#include "etl/optional.h"
#include "etl/vector.h"
#include "etl/atomic.h"
#include "FreeRTOSWrapper.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ConceptsConfig.hpp"


class FREETask {
private:


    TaskHandle_t taskHandle { nullptr };

    etl::atomic< bool > taskRunning { false } ;



    void (*task)(void*) ;
    void * arg ;
    uint32_t stackSize ; 
    uint32_t priority ;

    static uint32_t MAX_DELAY ;

    uint32_t stopBit : 1 { 0b1 } ;

    static void _task(){

        taskRunnning = true ;
        task( arg  ) ;

        vTaskDelete() ;
        taskRunning = false ;

    }
public:

    ~FREETask(){ 
        auto i{ 5 } ; 
        while( i--){ 
            if ( join()) break ; 
        }  
    }  
    
    FREETask(  void (*p_task)(void*) , void * p_arg ,  uint32_t p_stackSize , uint32_t p_priority )
        : task( p_task ) , arg( p_arg ) , stackSize( p_stackSize ) , priority( p_priority ) {}
        

    static etl::optional< FREETask > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  {

    bool notify( const uint32_t message ) ;

    bool requestStop()  ;
    bool join();

    bool start() ;


    static bool stopRequested(   );

    static bool waitForNotify( const uint32_t waitBitMask , const uint32_t p_delay );

    static void waitMs( const uint32_t ms ) ;
};

static_assert( TaskConcept< FREETask > );