
#pragma once
#include "../components/motorControl/include/moveStructures.hpp"
#include <concepts>
#include "etl/optional.h"
#include "etl/array.h"



template < class T >
concept EventGroupConcept = requires(   T EventGroup , const T  ConstEventGroup , 

                                        const uint32_t flagsBits , 
                                        typename T::GroupType &p_eventGroup ,
                                        const uint32_t idx ,
                                        const uint32_t message , const uint32_t delay ){
    //requires  std::constructible_from< T , EventGroupType , etl::array<  EventGroupMembersType EventGroupType::* , 32 >> ;
    { T::create( p_eventGroup,  flagsBits )} -> std::same_as< etl::optional< T >> ;
    { EventGroup.operator[]( idx ) } -> std::same_as< etl::optional< typename T:: MembersType >> ;
    { EventGroup.bitsWait( message , delay ) } -> std::same_as< bool > ;
 
    

};     


template< class T >
concept EngineConcept = requires( T Engine, const T ConstEngine , 
                            const int8_t stepPin , const int8_t dirPin , typename T::Planner &planner ,typename T::Stepper &engine  , typename T::Task  &task, 
                            const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ) {

    requires !std::default_initializable< T >;

    { T::create(  stepPin ,  dirPin ,  planner , engine  , task )} -> std::same_as< etl::optional< T >> ;
    { ConstEngine.isRunning() } -> std::same_as< bool > ; 
    { Engine.position() }       -> std::same_as< long > ;
    
    { Engine.start() } -> std::same_as< bool > ;
    { Engine.stop()  } -> std::same_as< bool > ;

    { Engine.move(      move , flags , wait ) } -> std::same_as< void > ; 
    { Engine.moveTo(    move , flags , wait ) } -> std::same_as< void > ;
    { Engine.moveToCup( move , flags , wait ) } -> std::same_as< void > ;


};

template<class T>
concept PlannerConcept = requires( T Planner , T ConstPlanner , 
                                    const moveBlock_t &move ){

    { Planner.calculateFrequency( move )} -> std::same_as< motionBlock_t > ; 
    { T::create() } -> std::same_as< etl::optional< T>> ; 

    { Planner.recieve() }       -> std::same_as< motionBlock_t > ;
    { Planner.stop() }          -> std::same_as< void > ;
    { Planner.start() }         -> std::same_as< void > ;
    { Planner.enqueue( move ) } -> std::same_as< void > ;
    
};

template< class T > 
concept StepperConcept  = requires( T Stepper , const T ConstStepper , 
                                    const motionBlock_t  motion ,
                                    const uint16_t step , const uint16_t dir ) { 

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
                                void (*task)(void*arg) , void * arg ,  const uint32_t stackSize , const uint32_t priority,  //  etl::optional< T> T::createTask( ... ) 
                                const uint32_t token , 
                                const uint32_t ms , 
                                const uint32_t message , const uint32_t delay   
                                                ){

    { T::create(  task ,  arg ,   stackSize ,  priority) }  -> std::same_as< etl::optional< T >> ;
    //requires !std::destructible<T> ; // private/protected : ~T(){ .... } ;

    { Task.notify(  message )  }            -> std::same_as< void > ;
    { Task.requestStop() }   -> std::same_as< bool > ;
    { Task.join() }          -> std::same_as< bool > ; 
    { Task.stop() } -> std::same_as< bool > ;
    { Task.start() } -> std::same_as< bool > ;


    { T::notifyWait( token ,  message ,  delay ) }  -> std::same_as< bool > ;
    { T::stopRequested( token ) }   -> std::same_as< bool > ;
    { T::waitMS(token ,  ms )} -> std::same_as<void> ;
    { T::MAX_DELAY } -> std::convertible_to< uint32_t > ;
};

template< class T > 
concept SensorConcept =     
                    requires( T Sensor , const T ConstSensor ) {

    { Sensor.turnOn()}              -> std::same_as< bool > ;
    { Sensor.turnOff()}             -> std::same_as< bool > ;
    { ConstSensor.getSample() }     -> std::same_as< const void* > ; 
    { Sensor.stopListeningIT()}     -> std::same_as< bool >   ;
    { Sensor.listenIT()}            -> std::same_as< bool >   ;
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