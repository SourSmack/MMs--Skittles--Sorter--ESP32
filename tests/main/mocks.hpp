#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <format>
#include <random>





template < class T  , class N > 
class  EventGroupMOCK {
public:
    using  GroupType  = T ;
    using MembersType = N ;
    static etl::optional< EventGroupMOCK > create( T & p_eventGroup , const uint32_t flagsBits){ return etl::optional< EventGroupMOCK >{etl::in_place} ;}


    
    // helper method for operator[] 
    MOCK_METHOD( ( etl::optional<  T > ) , At , ( const uint32_t idx ) ) ;

    etl::optional< N >operator[]( const uint32_t idx ){ 
        return At(idx);
    }

    MOCK_METHOD( ( bool )  ,  bitsWait   , ( const uint32_t message, const uint32_t delay));
    inline static  uint32_t MAX_DELAY { 1000 } ; 
};


struct DummyGroup{} ;
struct DummyMember{} ;
static_assert( EventGroupConcept < EventGroupMOCK< EventGroupHandle_t, EventBits_t   >> ) ;

class TaskMOCK {
public:





    static etl::optional < TaskMOCK >  create (  void (*task)(void*arg) , void * arg ,  const uint32_t stackSize , const uint32_t priority ){ return etl::optional< TaskMOCK >{ etl::in_place } ;}
    MOCK_METHOD( (bool) , set , ( void (*task)(void*arg) , void * arg ,  const uint32_t stackSize , const uint32_t priority)) ;
    MOCK_METHOD( ( void ) , notify , ( const uint32_t message )  ) ;
    MOCK_METHOD( ( bool ) , requestStop  , ()) ;
    MOCK_METHOD( ( bool ) , join , ()) ;
    MOCK_METHOD( ( bool ) , stop , ()) ;
    MOCK_METHOD( ( bool ) , start , ()) ;

    static bool notifyWait( const uint32_t token , const uint32_t message , const uint32_t delay){ return true ; }
    static bool stopRequested( const uint32_t token ){ return  true; }
    static void waitMS( const uint32_t token , const uint32_t ms ){  }

    inline static uint32_t MAX_DELAY { 10000 };

};
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

class SensorMOCK {
public:


    MOCK_METHOD( ( bool ) , turnOn , () );
    MOCK_METHOD( ( bool ) , turnOff , () );
    MOCK_METHOD( ( uint32_t ) , getSample , () , ( const ) );
    MOCK_METHOD( ( bool ) , stopListeningIT , () );
    MOCK_METHOD( ( bool ) , listenIT , () );
    

};
static_assert( SensorConcept< SensorMOCK > , "SensorMOCK doesn't meet concept requirments \n");

class PlannerMOCK{
public:

    static  etl::optional< PlannerMOCK > create() {  return  etl::optional< PlannerMOCK>{ etl::in_place} ; }

    MOCK_METHOD( ( motionBlock_t ) , calculateFrequency , ( const moveBlock_t &move ));
    MOCK_METHOD( ( motionBlock_t ) , recieve ,  ());
    MOCK_METHOD( ( bool ) , stop , ( ));
    MOCK_METHOD( ( bool ) , start , ());
    MOCK_METHOD( ( void ) , enqueue , ( const moveBlock_t &move ));
 
};
static_assert( PlannerConcept< PlannerMOCK > , "PlannerMOCK doesn't meet concept requirments \n");

class StepperMOCK{
public:

    
    static etl::optional<  StepperMOCK > create( const uint16_t step , const uint16_t dir ){
        return etl::optional< StepperMOCK >{ etl::in_place  }  ;
    }

    MOCK_METHOD( ( void ) , enqueue , ( const motionBlock_t  motion  ));
    MOCK_METHOD( ( bool ) , stop , ());
    MOCK_METHOD( ( bool ) , start , ());

};
static_assert( StepperConcept< StepperMOCK > , "StepperMOCK doesn't meet concept requirments \n");

template < class T , class N , class  M>
class EngMOCK {
private:
public:
    using Planner = T;
    using Stepper = N ;
    using Task  = M ;

    static etl::optional< EngMOCK > create( const int8_t stepPin , const int8_t dirPin , Planner &planner ,Stepper &engine  , Task &taskSpace){
        return etl::optional< EngMOCK >{ etl::in_place   } ;
    }     
    
    static void dataRelayTask(void * arg){ }

    MOCK_METHOD( ( bool ) , isRunning ,() , ( const ) );
    MOCK_METHOD( ( long ) , position ,());
    MOCK_METHOD( ( bool ) , start ,());
    MOCK_METHOD( ( bool ) , stop ,());
    void move( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait = 0  ){  moveImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ));
    void moveTo( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait  = 0  ){  moveToImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveToImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    void moveToCup( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait = 0  ){  moveToCupImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveToCupImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ));
                            
};
static_assert( EngineConcept< EngMOCK< PlannerMOCK , StepperMOCK,  TaskMOCK > > , "EngMOCK does not meet concept requirments\n");

