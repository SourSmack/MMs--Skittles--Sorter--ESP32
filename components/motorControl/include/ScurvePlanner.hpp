

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>
#include <etl/pair.h>
#include <etl/bitset.h>
#include "FreeRTOSWrapper.hpp"
#include "scurve.h"
template< class InMove , class OutMove >
class ScurvePlanner 
{
private:

    enum {
        stopBit  = 0 ,
        pauseBit = 1 << 1 ,
        resumeBit = 1 << 2 
    };
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< InMove,  MAXBUFFOR > movesQ{} ; 
    
    void (*destinationEnqueue)( const InMove &move ) { nullptr } ;

    FREETask  taskHandle { nullptr } ;
    
    static void scurveTask(void * arg){
        auto& [ instance ]   = *static_cast< ScurvePlanner* >( arg ) ;
        auto& [ MAXBUFFOR , movesQ , destinationEnqueue ,   taskHandle ] = instance ;
        

        while ( !FREETask::stopRequested()){
            if ( FREETask::waitForNotify( ScurvePlanner<T>::pauseBit , 0 )){
                while( FREETask::waitForNotify( ScurvePlanner<T>::resumeBit , 0 ){
                    FREETask::yield();
                } 
            }
            auto motion = calculateFrequency< InMove >( movesQ.front() ; ) ;
            movesQ.pop_front() ;
            destinationEnqueue( motion );


        }
        
        
    }

public:
   
    ScurvePlanner():  taskHandle( FREETask::create( scurveTask , this  , 2048 , 4 )) {}
    static etl::optional< ScurvePlanner >  create(  T&& enqueueFunc   );

    template < class T > 
    T calculateFrequency(const moveBlock_t &move , void ( desQue*)( const OutMove &move) = nullptr   ) {
        static double time { 0 } ;

        auto result = motion(  std::max( move.endSpeed , move.startSpeed ) , move.startAcc , move.startAcc  , move.steps , endSpeed ,  endAcc , time ) ; 
        time += 0.1 ;
        if ( time >= result.ct ){
            time = 0 ;
            OutMove finished{} ;
            if ( desQue)
                desQue( finished ) ;

            return finished ;

        }
        if ( desQue){
            
        }

        return 

    }

    bool stop();
    
    bool start(  void  ( enqueueAdapter*)( const OutMove &move) ) /* we should get recipe that converts from some randomEnqueue to enqueue that functions with our OutMove eand InMove*/ ){
        if ( taskHandle.isRunning() ) return false ;

        destinationEnqueue = enqueueAdapter ; 

        if ( !tastHandle.start()){
            destinationEnqueue = nullptr ;
            return false ;
        } 
        return true ; 
    }

    bool pause() ;
    bool resume() ;
    void enqueue( const moveBlock_t motion);
    
};