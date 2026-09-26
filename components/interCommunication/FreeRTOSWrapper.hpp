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
    friend class FREERTOSTest ;

    TaskHandle_t taskHandle { nullptr };

     bool  taskRunning { false } ;



    void (*task)(void*) ;
    void * arg ;
    uint32_t stackSize ; 
    uint32_t priority ;



    static void _task( void * arg ){
        auto &instance=  *static_cast<FREETask*>( arg ) ;
        instance.taskRunning = true ;
        instance.task( arg  ) ;

        vTaskDelete( instance.taskHandle ) ;
        instance.taskRunning = false ;

    }
public:
    static constexpr uint32_t stopBit  { 0b1 } ;
    static  unsigned long MAX_DELAY ;

    ~FREETask(){ 
        auto i{ 5 } ; 
        while( i--){ 
            if ( join()) break ; 
        }  
    }  
    
    FREETask(  void (*p_task)(void*) , void * p_arg ,  uint32_t p_stackSize , uint32_t p_priority )
        : task( p_task ) , arg( p_arg ) , stackSize( p_stackSize ) , priority( p_priority ) {}
        

    static etl::optional< FREETask > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;

    bool notify( const uint32_t message ) ;

    bool requestStop()  ;
    bool join();

    bool start() ;


    static bool stopRequested(   );

    static bool waitForNotify( const uint32_t waitBitMask , const uint32_t p_delay );

    static void waitMS( const uint32_t ms ) ;
};

static_assert( TaskConcept< FREETask > );