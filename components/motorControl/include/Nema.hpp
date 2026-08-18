#include <cstdint>
#include "IEngine.hpp"
#include "IPlanner.hpp"
#include "FastAccelStepperEngine.h"
#include "etl/circular_buffer.h"
#include "etl/optional.h"

class Nema : public IEngine 
{
private:

    struct tasksBits_t{
        uint8_t engineTaskBit : 1 { 0 }   ; 
        uint8_t plannerTaskBit : 1  { 0 }  ; 
    };
    tasksBits_t tasksBits{} ;

    static constexpr uint32_t ALL { 0u };
    static constexpr uint32_t maxBuffor { 32u } ; 

    int8_t stepPin{ 0 } ;
    int8_t dirPin{ 0 } ;


    IPlanner  *scurve { nullptr }  ; 
    FastAccelStepper *stepper { nullptr } ;

    TaskHandle_t dataRelayTaskHandle { nullptr };  

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