#pragma once

#include <cstdint>
#include "../../../main/ConceptsConfig.hpp" 




#include "etl/optional.h"
#include "cstdint"
//#include "FreeRTOSWrapper.hpp"

struct message_t{
    uint8_t dataRelayStart : 1 { 0 } ;
    uint8_t dataRelayLoopON : 1 { 0 }   ; 
    uint8_t plannerStart : 1 {0 } ;
    uint8_t plannerLoopON : 1 { 0 } ;
    uint8_t plannersQueueFull : 1  { 0 }  ; 
    
};
auto message { message_t{} };

typedef enum {
    ERR = - 1 , 
    OK 
}err_code_t;


template < PlannerConcept T , StepperConcept N , TaskConcept M > 
class Nema  
{
private:

    static constexpr uint32_t ALL { 0u };
    static constexpr uint32_t maxBuffor { 32u } ; 

    int8_t stepPin{ 0 } ;
    int8_t dirPin{ 0 } ;

    Planner  *scurve { nullptr }  ; 
    Stepper *stepper { nullptr } ;

    Task *task { nullptr };  

    bool running { false } ; 
public:
    using Planner =  T ;
    using Stepper = N;
    using Task =  M ;
    template< class T = Nema <  Planner , Stepper , Task>> 
    static etl::optional< T > create( int8_t stepPin , int8_t dirPin , Planner &planner ,Stepper &engine  , etl::optional<Task> &taskSpace ){
        T tmp{ stepPin, dirPin} ; 

        if ( tmp.init( planner , engine, *taskSpace) != err_code_t::OK )
            return etl::nullopt ; 
        
        return tmp ; 
    }
private:

    Nema() = default ;

    Nema(int8_t stepPin , int8_t dirPin): stepPin(stepPin) , dirPin( dirPin ){} 

    int init(Planner &planner ,Stepper &engine   , Task &taskSpace){
        stepper = &engine ;
        scurve = &planner ;
        
        task = &taskSpace ;


        return err_code_t::OK ;
    }


    static void dataRelayTask(void * arg){
        auto instance { *static_cast< Nema*>( arg ) } ;
        auto& scurve = instance.scurve ; 
        auto& stepper = instance.stepper ; 
        auto& [dataRelayStart , dataRelayLoopON , plannerStart , plannerLoopON , plannersQueueFull] = message ;  


        
        while ( ! Task::stopRequested() ){
            
            auto queueFull =  Task::notifyWait( plannersQueueFull , 0 ) ;

            if ( queueFull ){
                motionBlock_t motion{} ;
                for (auto i{0} ; i < Nema::maxBuffor ; ++i){
                    motion = scurve->recieve() ; 
                    stepper->enqueue( motion ) ;
                }
                queueFull = false ;
            }
        }
    }


    //static_assert( EngineConcept<Nema, Planner , Stepper , Task > , "Nema does not meet Concept: \"EngineConcept\" requirments!\n");
public:


    bool isRunning(const uint8_t engineNum)const  { return running ;}
    long position(const uint8_t engineNum)   { return 0 ;}


    void move( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  )  { }
    void moveTo( const moveBlock_t &move , const moveInfo_t flags , const int wait   )  { }
    void moveToCup( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait  = 0   ){
        // seperate task that moves from plannner to stepper and seperate task for planner that makes moves 
        // moveToCup  just manages whether instantly make moveo or enqu in normal manner
        // manages que position motionBlock_t
        if ( !running) return ;

        if  ( flags.enqueue ){

            scurve->enqueue( move ) ;
        }
        else{

            scurve->stop() ;  
            while ( auto motion  = scurve->calculateFrequency( move ) ){
                stepper->enqueue( motion ) ; 
            }
            scurve->start();
        }
    }


    void update(const uint32_t blocksToUpdate = ALL)  {}

    void flush(const uint32_t motionsToFlush = ALL)  {}

    bool stop()  { return true; }
    bool start()  {
        stepper->start();
        scurve->start() ; 

        *task = *Task::createTask( dataRelayTask, this , 2048 , 4);
        if ( task == etl::nullopt) return false ; 

        running  = true ;

        return true; 
     }
 

};



