#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Nema.hpp"

class TaskMOCK {
public:





    static etl::optional < TaskMOCK >  create (  void (*task)(void*arg) , void * arg ,  const uint32_t stackSize , const uint32_t priority ){ return etl::optional< TaskMOCK >{ etl::in_place } ;}
    MOCK_METHOD( ( void ) , notify , ( const uint32_t message )  ) ;
    MOCK_METHOD( ( bool ) , requestStop  , ()) ;
    MOCK_METHOD( ( bool ) , join , ()) ;
    MOCK_METHOD( ( bool ) , stop , ()) ;
    MOCK_METHOD( ( bool ) , start , ()) ;

    static bool notifyWait( const uint32_t token , const uint32_t message , const uint32_t delay){ return true ; }
    static bool stopRequested( const uint32_t token ){ return  true; }
    static void waitMS( const uint32_t token , const uint32_t ms ){  }

    static uint32_t MAX_DELAY ;

};
uint32_t TaskMOCK::MAX_DELAY {  10000 } ;
static_assert( TaskConcept< TaskMOCK >  );

class TaskPerspective{
public:
    bool notifyWait( uint32_t token , uint32_t message, uint32_t delay){ return true ;  }
    bool stopRequested( uint32_t token ){  return true; }
    void waitMS( uint32_t token , uint32_t ms ) { }
};

class TaskPerspectiveMOCK{

public:
    MOCK_METHOD( ( bool ) , notifyWait , (  uint32_t token , uint32_t message, uint32_t delay ));
    MOCK_METHOD( ( bool ) , stopRequested ,  ( uint32_t token ));
    MOCK_METHOD( ( bool ) , waitMS , ( uint32_t token , uint32_t ms ) );

};

class PlannerMOCK{
public:

    static  etl::optional< PlannerMOCK > create() {  return  etl::optional< PlannerMOCK>{ etl::in_place} ; }

    MOCK_METHOD( ( motionBlock_t ) , calculateFrequency , ( const moveBlock_t &move ));
    MOCK_METHOD( ( motionBlock_t ) , recieve ,  ());
    MOCK_METHOD( ( void ) , stop , ( ));
    MOCK_METHOD( ( void ) , start , ());
    MOCK_METHOD( ( void ) , enqueue , ( const moveBlock_t &move ));
 
};
static_assert( PlannerConcept< PlannerMOCK > , "PlannerMOCK doesn't meet concept requirments \n");

class StepperMOCK{
    struct ConstructorKey{};
public:
    explicit StepperMOCK( ConstructorKey) {} 
    
    static etl::optional<  StepperMOCK > create( const uint16_t step , const uint16_t dir ){
        return etl::optional< StepperMOCK >{ etl::in_place , ConstructorKey{} } ;
    }

    MOCK_METHOD( ( void ) , enqueue , ( const motionBlock_t  motion  ));
    MOCK_METHOD( ( void ) , stop , ());
    MOCK_METHOD( ( void ) , start , ());

};
static_assert( StepperConcept< StepperMOCK > , "StepperMOCK doesn't meet concept requirments \n");




class NemaTesting : public ::testing::Test{
protected:




};

/*
template< class T >
concept EngineConcept = requires( T Engine, const T ConstEngine , 
                            //const int8_t stepPin , const int8_t dirPin , typename T::Planner &planner ,typename T::Stepper &engine  , typename T::Task  &task, 
                            const moveBlock_t &move  ,const  moveInfo_t flags =  {}, const int wait  = 0 ) {

    //requires !std::default_initializable< T >;

    //{ T::create(  stepPin ,  dirPin ,  planner , engine  , task )} -> std::same_as< etl::optional< T >> ;
    { ConstEngine.isRunning() } -> std::same_as< bool > ; 
    { Engine.position() }       -> std::same_as< long > ;
    
    { Engine.start() } -> std::same_as< bool > ;
    { Engine.stop()  } -> std::same_as< bool > ;

    { Engine.move(      move , flags , wait ) } -> std::same_as< void > ; 
    { Engine.moveTo(    move , flags , wait ) } -> std::same_as< void > ;
    { Engine.moveToCup( move , flags , wait ) } -> std::same_as< void > ;


}; */

