

#include "Nema.hpp"
#include "CRTPconfig.hpp"





etl::optional< Nema > Nema::create( int8_t stepPin , int8_t dirPin , Planner &planner ,Planner &engine , etl::optional<Task> &taskSpace  )
{
    Nema tmp{ stepPin, dirPin} ; 

    taskSpace = ITask<TaskType>::createTask( dataRelayTask, &tmp , 2048 , 4);
    if ( taskSpace == etl::nullopt) return etl::nullopt ; 



    if ( tmp.init( planner , engine, *taskSpace) != ESP_OK )
        return etl::nullopt ; 
        
        
    
    return tmp ; 
}


Nema:: Nema( int8_t stepPin , int8_t dirPin ): stepPin(stepPin) , dirPin( dirPin )    {} 


int Nema::init( Planner &planner ,Stepper &engine   , Task &taskSpace){

    stepper = &engine ;
    scurve = &planner ;
    
    dataRelayTaskHandle = &taskSpace ;

    return ESP_OK ;
} 

// TODO on notify to end job it'll produce one more motion , and probably shouldnt 

void Nema::dataRelayTask(void * arg){
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


bool Nema::isRunning()const  
{

    return true ;
}

long Nema::position()  
{

    return 1  ;
}


void Nema::move( const moveBlock_t &move  ,const  moveInfo_t flags = {}, const int wait = 0 ) 
{

}

void Nema::moveTo( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait = 0 )  
{
    
}
