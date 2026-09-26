

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>
#include <etl/pair.h>
#include <etl/bitset.h>

#include "FreeRTOSWrapper.hpp"
template< class T>
class ScurvePlanner 
{
private:
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< moveBlock_t,  MAXBUFFOR > movesQ{} ; 
    std::delegat
    FREETask  taskHandle { nullptr } ;
    
    static void scurveTask(void * arg);
public:
    enum {
        stopBit  = 0 ,
        pauseBit = 1 << 1 ,
        resumeBit = 1 << 2 
    };
   
    ScurvePlanner( T&& enqueueFunc   ): args{ enqueueFunc, this } , taskHandle( FREETask::create( scurveTask , &args  , 2048 , 4 )) {}
   static etl::optional< ScurvePlanner >  create(  T&& enqueueFunc   );
    consteval void calculateFrequency(const moveBlock_t &move )   ; 
    bool stop();
    bool start();
    bool pause() ;
    bool resume() ;
    void enqueue( const moveBlock_t motion);
    
};