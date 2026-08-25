#pragma once
#include <cstdint>
#include "CRTPconfig.hpp"
#include "ScurvePlanner.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "IPlanner.hpp"
#include "FastAccelStepperEngine.h"
#include "etl/optional.h"
#include "cstdint"
#include "FreeRtosWrapper.hpp"

struct message_t{
    uint8_t dataRelayStart : 1 { 0 } ;
    uint8_t dataRelayLoopON : 1 { 0 }   ; 
    uint8_t plannerStart : 1 {0 } ;
    uint8_t plannerLoopON : 1 { 0 } ;
    uint8_t plannersQueueFull : 1  { 0 }  ; 
    
};


template < PlannerConcept Planner , StepperConcept Stepper , TaskConcept Task > 
class Nema  
{
private:

    message_t message{} ;

    static constexpr uint32_t ALL { 0u };
    static constexpr uint32_t maxBuffor { 32u } ; 

    int8_t stepPin{ 0 } ;
    int8_t dirPin{ 0 } ;


    Planner  *scurve { nullptr }  ; 
    Stepper *stepper { nullptr } ;

    Task *dataRelayTaskHandle { nullptr };  

    int init(Planner &planner ,Stepper &engine   , Task &taskSpace){
        stepper = &engine ;
        scurve = &planner ;
        
        dataRelayTaskHandle = &taskSpace ;

        return ESP_OK ;
    }

    Nema(int8_t stepPin , int8_t dirPin): stepPin(stepPin) , dirPin( dirPin ){} 

    static void dataRelayTask(void * arg){
        auto instance { *static_cast< Nema*>( arg ) } ;
        auto& scurve = instance.scurve ; 
        auto& [dataRelayStart , dataRelayLoopON , plannerStart , plannerLoopON , plannersQueueFull] = instance.message ;  
        
        
        

        while ( !dataRelayTaskHandle->notifyWait( dataRelayStart , portMAX_DELAY)){}

        bool relay { true }; 
        
        while ( relay ){
            relay = notifyWait( dataRelayLoopON , 0) ;
            


            auto queueFull =  notifyWait( plannersQueueFull , 0 ) ;

            if ( queueFull ){
                motionBlock_t motion{} ;
                for (auto i{0} ; i < Nema::maxBuffor ; ++i){
                    xQueueReceive( scurve->motionsQ , &motion,  pdMS_TO_TICKS( 10 ) ) ;
                    stepper->addQueueEntity( motion ) ;
                }

            }



        }
        vTaskDelete(nullptr);
    
    }

    Nema() = default ;

    static_assert( EngineConcept<Nema, Planner , Stepper , Task > , "Nema does not meet Concept: \"EngineConcept\" requirments!\n")
public:


    static etl::optional< Nema > create( int8_t stepPin , int8_t dirPin , Planner &planner ,Stepper &engine  , etl::optional<Task> &taskSpace ){
        Nema tmp{ stepPin, dirPin} ; 

        taskSpace = ITask<TaskType>::createTask( dataRelayTask, &tmp , 2048 , 4);
        if ( taskSpace == etl::nullopt) return etl::nullopt ; 



        if ( tmp.init( planner , engine, *taskSpace) != ESP_OK )
            return etl::nullopt ; 
        
        return tmp ; 
    }

    bool isRunning(const uint8_t engineNum)const  ;
    long position(const uint8_t engineNum)   ;


    void move( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  )  ;
    void moveTo( const moveBlock_t &move , const moveInfo_t flags , const int wait   )  ;
    void moveToCup( const moveBlock_t &move , const moveInfo_t flags , const int wait   ){
        // seperate task that moves from plannner to stepper and seperate task for planner that makes moves 
        // moveToCup  just manages whether instantly make moveo or enqu in normal manner
        // manages que position motionBlock_t
        if  ( !flags.enqueue ){
            scurve.stop() ; // should not use vTaskSuspend , 
            while ( auto motion  = scurve->calculateFrequency( move ) ){
                stepper->addQueueEntry( motion ) ; 
            }
            scurve.start();
        }
        else{

            scurve.enqueue( move ) ;
        }
    }


    void update(const uint32_t blocksToUpdate = ALL)  ;

    void flush(const uint32_t motionsToFlush = ALL)  ;

    void stop(const bool flush  , const bool instantly   )  ;
    void start(void)  ;
 

};



