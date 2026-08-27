#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



class Task {
private:
    TaskHandle_t task;
    etl::atomic< bool > taskRunning { false } ;

    ~Task(){  if ( taskRunning) { requestStop() ;  join() ;  }}  ;
public:

        
        

    static etl::optional< Task > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;

    void notify( uint8_t message )   ;
    bool requestStop()  ;
    bool join();

    static bool stopRequested();
    static bool notifyWait( uint8_t message , uint32_t delay );
    static void waitMs( uint32_t ms ) ;

};
