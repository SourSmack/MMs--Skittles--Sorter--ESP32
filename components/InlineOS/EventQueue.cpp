#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"





template< class T >
class EventQueue{
private:
    T *queuePtr { nullptr } ;
public:
    EventQueue( T *ptr){
        if ( ptr)  queuePtr = ptr ;
    }
    T* operator*(){ return queuePtr; } 
    bool recieve( void * event, uint32_t ticksToWait){
        if( xQueueReceive( queuePtr,   event, ticksToWait )  == pdPASS) return true ;
        return false ;
    }
};


