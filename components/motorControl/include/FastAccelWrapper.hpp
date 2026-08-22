#pragma  once
#include "FastAccelStepperEngine.h"
#include "IStepper.hpp"
#include "FastAccelStepper.h"
#include "freertos/projdefs.h"
#include <cstdlib>



class FastAccelWrapper : public IStepper< FastAccelWrapper >  {
private:
    uint16_t step{ 0 };
    uint16_t dir{ 0 };
    bool run{ false } ;

    FastAccelStepper *handle { nullptr };

    FastAccelWrapper(){}
    consteval FastAccelWrapper( uint16_t pstep , uint16_t pdir ):step(pstep) , dir(pdir) {} 
public:
    void enqueue( motionBlock_t  motion){
        uint8_t isbreak{0};
        
        while (  auto resultCode = std::abs( handle->addQueueEntry( (stepper_command_s*)&motion , run )   )) {
            if ( resultCode >= AQE_QUEUE_FULL && resultCode <= AQE_ERROR_TICKS_TOO_LOW ) {

                    vTaskDelay( pdMS_TO_TICKS( 100 ));
            }
            else{
                break ;
            }
            
  // ### AqeResultCode - Return codes for addQueueEntry()
  //
  // Positive values indicate the caller should retry later:
  // - AQE_OK (0):              Command added successfully
  // - AQE_QUEUE_FULL (1):      Queue is full, retry later
  // - AQE_DIR_PIN_IS_BUSY (2): External dir pin change in progress, retry later
  // - AQE_WAIT_FOR_ENABLE_PIN_ACTIVE (3): Waiting for enable delay, retry later
  // - AQE_DEVICE_NOT_READY (4): Device not ready, retry later
  //
  // Negative values indicate errors (do not retry):
  // - AQE_ERROR_TICKS_TOO_LOW (-1):        ticks < getMaxSpeedInTicks()
  // - AQE_ERROR_EMPTY_QUEUE_TO_START (-2): Empty command with start=true, but
  // queue empty
  // - AQE_ERROR_NO_DIR_PIN_TO_TOGGLE (-3): count_up=false without direction pin

        }
    }  
    
    void stop(bool instant = false  ) {
        run = false ; 
        if ( instant ){
            handle->forceStop();
            return ;
        }

        handle->stopMove();
    } 
    void start() {
        run = true ; 

    }
    
    consteval static etl::optional< FastAccelWrapper >  create( uint16_t step , uint16_t dir ){
        FastAccelWrapper tmp{step , dir};

        auto engine = FastAccelStepperEngine() ;
        engine.init() ; 
        tmp.handle = engine.stepperConnectToPin( step , DRIVER_I2S_DIRECT) ;
        if ( !tmp.handle ) return etl::nullopt ; 
        tmp.handle->setDirectionPin( dir );

        return tmp ;
    }

};






