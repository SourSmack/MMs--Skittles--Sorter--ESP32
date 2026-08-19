
#include "Nema.hpp"




etl::optional< Nema > Nema::create( int8_t stepPin , int8_t dirPin , Planner &planner ,FastAccelStepperEngine &engine    )
{
    Nema tmp{ stepPin, dirPin} ; 

    if ( tmp.init( planner , engine) != ESP_OK )
        return etl::nullopt ; 
    // TODO: change this to my new taskWrapper api 
    if ( xTaskCreate( dataRelayTask ,"dataRelayTask" , 1024 , &tmp , 4 , tmp.dataRelayTaskHandle ) != pdPASS ) return etl::nullopt; 
    return tmp ; 
}

Nema:: Nema( int8_t stepPin , int8_t dirPin ): stepPin(stepPin) , dirPin( dirPin )    {} 

int Nema::init(IPlanner &planner ,FastAccelStepperEngine &engine   ){

    stepper = engine.stepperConnectToPin( stepPin , DRIVER_I2S_DIRECT);
    if ( !stepper ) return ESP_FAIL  ; 
    scurve = &planner ;


    return ESP_OK ;
} 

bool thisTask::notifyWait( uint8_t message , uint32_t delay ){
    uint32_t taskBits{0};
    xTaskNotifyWait(0x00 , message , &taskBits , delay) ; 
    return taskBits & ~message ; 
}  
// TODO on notify to end job it'll produce one more motion , and probably shouldnt 
void Nema::dataRelayTask(void * arg){
    auto instance { *static_cast< Nema*>( arg ) } ;
    auto& scurve = instance.scurve ; 
    auto& [dataRelayStart , dataRelayLoopON , plannerStart , plannerLoopON , plannersQueueFull] = instance.message ;  
    
    
    

    while ( !notifyWait( dataRelayStart , portMAX_DELAY)){}

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

bool Nema::isRunning(const uint8_t engineNum)const  
{

    return true ;
}
long Nema::position(const uint8_t engineNum)  
{

    return 1  ;
}

void Nema::move( const moveBlock_t &move  ,const  moveInfo_t flags = {}, const int wait = 0 ) 
{

}
void Nema::moveTo( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait = 0 )  
{
    
}



void Nema::moveToCup( const moveBlock_t &move , const moveInfo_t flags = {} , const int wait  = 0  )  
{
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

void Nema::update(const uint32_t blocksToUpdate )  
{

}

void Nema::flush(const uint32_t motionsToFlush  ) 
{

}

void Nema::stop(const bool flush  = false  , const bool instantly    = false)  
{

}
void Nema::start(void) 
{

}






