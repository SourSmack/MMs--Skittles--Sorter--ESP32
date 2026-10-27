

#pragma once

#include "freertos/FreeRTOS.h"
#include  "freertos/queue.h"
#include <etl/circular_buffer.h>
#include "moveStructures.hpp"
#include <etl/optional.h>


#include "FreeRTOSWrapper.hpp"
#include "s-curve-motion-planning/scurve.h"

template< class InMove , class Task = FREETask  >
class ScurvePlanner 
{
private:
    using OutMove = Scurve::RESULT ;
    enum {
        stopBit  = 0 ,
        pauseBit = 1 << 1 ,
        resumeBit = 1 << 2 
    };
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer< InMove,  MAXBUFFOR > movesQ{} ; 
    
    void (*destinationEnqueue)( const OutMove &move ) { nullptr } ;

    Task  &taskHandle  ;
    
    static void scurveTask(void * arg){
        auto* instance = static_cast<ScurvePlanner*>(arg);
        auto& movesQ = instance->movesQ;
        auto& destinationEnqueue = instance->destinationEnqueue;
        
        
        while ( !Task::stopRequested()){
            if ( Task::waitForNotify( ScurvePlanner::pauseBit , 0 )){
                while( Task::waitForNotify( ScurvePlanner::resumeBit , 0 )){
                    Task::yield();
                } 
            }
            if ( !movesQ.empty() ){
                instance->calculateFrequency( movesQ.front() , destinationEnqueue  ) ;
                movesQ.pop() ;
            }
            else {
                Task::yield() ;
            }

        }
        
        
    }

public:
   friend class ScurveTesting ;
   
    //ScurvePlanner():  taskHandle( Task::create( scurveTask , this  , 2048 , 4 )) {}
    ScurvePlanner(  Task  &task   , void (*pDestinationEnqueue)( const OutMove &move) = nullptr  )
            :   destinationEnqueue( pDestinationEnqueue ) ,  
                taskHandle(  task   ) 
        {}

    static etl::optional< ScurvePlanner >  create(Task &task , void (*pDestinationEnqueue)( const OutMove &move) ){
        ScurvePlanner tmp{ task ,  pDestinationEnqueue };
        return tmp ;
    }


    void calculateFrequency(const InMove &move ,  void ( *pEnqueue)( const OutMove &move) =  nullptr     ) {
        static Scurve calc{} ; 
        const auto destinationEnqueue = ( pEnqueue ) ? pEnqueue : ScurvePlanner::destinationEnqueue ;  

        double time { 0 } ;
        OutMove result ;
        while ( time < result.ct ){
            auto result = calc.motion(
                move.traversalSpeed,
                move.maxAcc,       
                move.startSpeed,    
                move.startAcc,      
                move.steps,         
                move.endSpeed,      
                move.endAcc ,        
                time                
            );

            time += 0.1 ;
            destinationEnqueue( result ) ;
        }
    }

    bool stop(){
        if ( !taskHandle.isRunning() ) return true ;

        if ( !taskHandle.join() ) return false ;

        return true ;
    }
    /* we should get recipe that converts from some randomEnqueue to enqueue that functions with our OutMove eand InMove*/ 
    bool start(  void  ( *enqueueAdapter)( const OutMove &move) ) {
        if ( taskHandle.isRunning() ) return false ;

        destinationEnqueue = enqueueAdapter ; 

        if ( !taskHandle.start()){
            destinationEnqueue = nullptr ;
            return false ;
        } 
        return true ; 
    }

    // mechanism that DO NOT guarantee pause - it's task resposnible to implement proper while-loop with pause-statement 
    bool pause() {
        if ( !taskHandle.notify( pauseBit  ) ) return false ;

        return true ;
    }

    // mechanism that DO NOT guarantee resume - it's task resposnible to implement proper while-loop with resume-statement 
    bool resume() {
        if ( !taskHandle.notify( resumeBit )) return false ;
        return true ;
    }

    // Adds InMove to internal structure from which ScurveTask (taskHandle) takes motions to make 
    bool enqueue( const InMove &motion){
        if ( movesQ.full() ) return  false ;

        movesQ.push( motion );
        return true ;
    }
    
};