//#include "../main/Sorter.hpp"
/*
g++ -std=c++23 -g Sorter_test.cpp -o Sorter_test \
  -I$(pwd)/../components/libs/etl/include \
  -I/home/sourish/esp-idf-v6.0.2/components/log/include/ \
  -I/home/sourish/c++_libs/googletest/googlemock/include \
  -I/home/sourish/c++_libs/googletest/googletest/include

*/
#include "../main/ConceptsConfig.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

template < class GroupType  , class MemberType > 
class  EventFlagsMOCK {
public:
    static etl::optional< EventFlagsMOCK > create( GroupType & p_eventGroup , const uint32_t bits){ return etl::optional< EventFlagsMOCK >{etl::in_place} ;}


    
    
    MOCK_METHOD( ( etl::optional<  MemberType > ) , At , ( const uint32_t idx ) ) ;

    etl::optional< MemberType >operator[]( const uint32_t idx ){ 
        return At(idx);
    }

    MOCK_METHOD( ( bool )  ,  bitsWait   , ( const uint32_t message, const uint32_t delay));
 
};
struct DummyGroup{} ;
struct DummyMember{} ;
static_assert( EventGroupConcept < EventFlagsMOCK< DummyGroup , DummyMember >, DummyGroup , DummyMember > ) ;

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


template < class Planner , class Stepper , class  Task>
class EngMOCK {
private:
    EngMOCK() = default ;
    friend etl::optional< EngMOCK > ;
public:


    MOCK_METHOD( ( bool ) , isRunning ,());
    MOCK_METHOD( ( long ) , position ,());
    MOCK_METHOD( ( bool ) , start ,());
    MOCK_METHOD( ( bool ) , stop ,());
    MOCK_METHOD( ( void ) , move ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveTo ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveToCup ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
                            
};
//static_assert( EngineConcept< EngMOCK > , "EngMOCK does not meet concept requirments\n");

class SensorMOCK {

};

class PlannerMOCK{

};



using ::testing::_;
using ::testing::Return;


TEST( SorterHomingSlideTest , doesSensorDetect ){


}






