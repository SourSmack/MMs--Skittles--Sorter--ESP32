#pragma once
#include <cstdint>
#include "ScurvePlanner.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "IEngine.hpp"
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


class Nema : public IEngine< Nema >
{
private:

    message_t message{} ;

    static constexpr uint32_t ALL { 0u };
    static constexpr uint32_t maxBuffor { 32u } ; 

    int8_t stepPin{ 0 } ;
    int8_t dirPin{ 0 } ;


    IPlanner<PlannerType>  *scurve { nullptr }  ; 
    IStepper<StepperType> *stepper { nullptr } ;

    ITaskWrapper<TaskType> *dataRelayTaskHandle { nullptr };  

    int init(IPlanner<PlannerType> &planner ,IStepper<StepperType> &engine   );

    Nema(int8_t stepPin , int8_t dirPin);

    static void dataRelayTask(void * arg);
    Nema(){}
public:


    static etl::optional< Nema > create( int8_t stepPin , int8_t dirPin , IPlanner<PlannerType> &planner ,IStepper<StepperType> &engine    );

    bool isRunning(const uint8_t engineNum)const  ;
    long position(const uint8_t engineNum)   ;


    void move( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  )  ;
    void moveTo( const moveBlock_t &move , const moveInfo_t flags , const int wait   )  ;
    void moveToCup( const moveBlock_t &move , const moveInfo_t flags , const int wait   )  ;

    void update(const uint32_t blocksToUpdate = ALL)  ;

    void flush(const uint32_t motionsToFlush = ALL)  ;

    void stop(const bool flush  , const bool instantly   )  ;
    void start(void)  ;
 

};


