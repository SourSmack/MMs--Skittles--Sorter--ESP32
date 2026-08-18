#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "IEngine.hpp"
#include "IPlanner.hpp"
#include "FastAccelStepperEngine.h"
#include "etl/circular_buffer.h"
#include "etl/optional.h"
#include "cstdint"
#include "TaskWrapper.hpp"

struct message_t{
    uint8_t dataRelayStart : 1 { 0 } ;
    uint8_t dataRelayLoopON : 1 { 0 }   ; 
    uint8_t plannerStart : 1 {0 } ;
    uint8_t plannerLoopON : 1 { 0 } ;
    uint8_t plannersQueueFull : 1  { 0 }  ; 
    
};


class Nema : public IEngine 
{
private:

    message_t message{} ;

    static constexpr uint32_t ALL { 0u };
    static constexpr uint32_t maxBuffor { 32u } ; 

    int8_t stepPin{ 0 } ;
    int8_t dirPin{ 0 } ;


    IPlanner  *scurve { nullptr }  ; 
    FastAccelStepper *stepper { nullptr } ;

    TaskWrapper *dataRelayTaskHandle { nullptr };  

    int init(IPlanner &planner ,FastAccelStepperEngine &engine   );

    Nema(int8_t stepPin , int8_t dirPin);

    static void dataRelayTask(void * arg);
public:
    Nema() = delete ;
    static etl::optional< Nema > create( int8_t stepPin , int8_t dirPin , IPlanner &planner ,FastAccelStepperEngine &engine    );
    
    

    bool isRunning(const uint8_t engineNum)const override ;
    long position(const uint8_t engineNum)  override ;


    void move( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ) override ;
    void moveTo( const moveBlock_t &move , const moveInfo_t flags , const int wait   ) override ;
    void moveToCup( const moveBlock_t &move , const moveInfo_t flags , const int wait   ) override ;

    void update(const uint32_t blocksToUpdate = ALL) override ;

    void flush(const uint32_t motionsToFlush = ALL) override ;

    void stop(const bool flush  , const bool instantly   ) override ;
    void start(void) override ;
 

};