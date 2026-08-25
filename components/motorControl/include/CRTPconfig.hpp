
#pragma once
#include "moveStructures.hpp"
#include <concepts>
#include "etl/optional.h"






template<class T, class Planner , class Stepper , class Task >
concept EngineConcept = requires( T Engine, const T ConstEngine , 
                            int8_t stepPin , int8_t dirPin , Planner &planner ,Stepper &engine  , etl::optional<Task> &taskSpace, 
                            const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ) {

    requires !std::default_initializable<T>;

    { T::create(  stepPin ,  dirPin ,  planner , engine  , taskSpace )} -> std::same_as< etl::optional<T>> ;
    { ConstEngine.isRunning() } -> std::same_as< bool > ; 
    { Engine.position() }       -> std::same_as< long > ;
    


    { Engine.move(      move , flags , wait ) } -> std::same_as< void > ; 
    { Engine.moveTo(    move , flags , wait ) } -> std::same_as< void > ;
    { Engine.moveToCup( move , flags , wait ) } -> std::same_as< void > ;


};

template<class T>
concept PlannerConcept = requires( T Planner , T ConstPlanner , 
                                    const moveBlock_t &move ){

    { Planner.calculateFrequency( move )} -> std::same_as< motionBlock_t > ; 
    { T::create() } -> std::same_as< etl::optional< T>> ; 

};

template< class T > 
concept StepperConcept  = requires( T Stepper , const T ConstStepper , 
                                    motionBlock_t  motion ,
                                    uint16_t step , uint16_t dir ) { 

    requires ! std::default_initializable<T> ; 
    { Stepper.enqueue(   motion) } -> std::same_as<void > ;
    
    { Stepper. stop() }   -> std::same_as< void > ;
    { Stepper.start() } -> std::same_as<void> ;
    { T::create( step , dir ) }  -> std::same_as< etl::optional< T >> ;
};

template< class T>
concept TaskConcept = 
                    requires( 
                                T Task , const T ConstTask ,
                                void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority,  //  etl::optional< T> T::createTask( ... ) 
                                uint32_t ms , 
                                uint8_t message ,           
                                uint32_t delay   
                                                ){

    { T::createTask(  task ,  arg ,   stackSize ,  priority) }  -> std::same_as< etl::optional< T >> ;
    requires !std::destructible<T> ; // private/protected : ~T(){ .... } ;

    { Task.notify(  message )  }            -> std::same_as< void > ;
    { Task.requestStop() }   -> std::same_as< bool > ;
    { Task.join() }          -> std::same_as< bool > ; 


    { T::notifyWait(  message ,  delay ) }  -> std::same_as< bool > ;
    { T::stopRequested() }   -> std::same_as< bool > ;
    { T::waitMS( ms )} -> std::same_as<void> ;

};

template< class T > 
concept SensorConcept =     
                    requires( T Sensor , const T ConstSensor ) {

    { Sensor.turnOn()}              -> std::same_as< bool > ;
    { Sensor.turnOff()}             -> std::same_as< bool > ;
    { ConstSensor.getSample() }     -> std::same_as< const void* > ; 
    { Sensor.stopListening_IT()}    -> std::same_as< bool >   ;
    { Sensor.listen_IT()}           -> std::same_as< bool >   ;
};


/*
template < PlannerConcept Planner , StepperConcept Stepper , TaskConcept Task > 
class Nema;

class ScurvePlanner;
class FastAccelWrapper;
class Task;


using EngineType  = Nema;
using PlannerType = ScurvePlanner;
using StepperType = FastAccelWrapper;
using TaskType    = Task;
*/