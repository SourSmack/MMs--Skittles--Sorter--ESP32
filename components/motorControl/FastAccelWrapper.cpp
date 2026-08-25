#include "FastAccelWrapper.hpp"

etl::optional< FastAccelWrapper >  FastAccelWrapper::create( uint16_t step , uint16_t dir ){
    FastAccelWrapper tmp{step , dir};

    FastAccelStepperEngine engine {}  ;
    engine.init() ; 
    tmp.handle = engine.stepperConnectToPin( step , DRIVER_I2S_DIRECT) ;
    if ( !tmp.handle ) return etl::nullopt ; 
    tmp.handle->setDirectionPin( dir );
    tmp.handle->setAutoEnable( true ) ;

    return tmp ;
}
void FastAccelWrapper::enqueue( motionBlock_t  motion){
    uint8_t isbreak{0};
    
    while (  auto resultCode = std::abs( handle->( (stepper_command_s*)&motion , run )   )) {
        if ( resultCode >= AQE_QUEUE_FULL && resultCode <= AQE_ERROR_TICKS_TOO_LOW ) {

            Task::waitMS( 100 ) ;
        }
        else{
            break ;
        }
    }
}  
    
void FastAccelWrapper::stop(bool instant = false  ) {
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
 