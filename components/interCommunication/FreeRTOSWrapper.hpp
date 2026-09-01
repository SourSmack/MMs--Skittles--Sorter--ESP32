#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



class FREETask {
private:
    template < class T > 
    static etl::array< etl::pair< T& , uint16_t >, 16 > tokens ; 
private:
    TaskHandle_t task;
    etl::atomic< bool > taskRunning { false } ;

    bool _stopRequested { false } ;
    uint32_t notifyMessage { 0 } ;


    void * arg { nullptr } ;

    static uint32_t MAX_DELAY ;
public:

    ~FREETask(){  if ( taskRunning) { requestStop() ;  join() ;  }}  ;
        
        

    static etl::optional< FREETask > create( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority )  ;

    void notify( uint8_t message )   ;
    bool requestStop()  ;
    bool join();

    static bool stopRequested( uint32_t token  );
    static bool notifyWait( uint32_t token ,  , uint8_t message , uint32_t delay );
    /*   looks up some global std::pair structure to see which FREEtask instance is resposible for that TOKEN 
        
    */
    static void waitMs( uint32_t token  ,  uint32_t ms ) ;
    
    bool start() ; 
    bool stop() ; 
};
