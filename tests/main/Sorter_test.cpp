//#include "../main/Sorter.hpp"
/*
g++ -std=c++23 -g Sorter_test.cpp -o Sorter_test 
  -I$(pwd)/../components/libs/etl/include 
  -I/home/sourish/esp-idf-v6.0.2/components/log/include/ 
  -I/home/sourish/esp-idf-v6.0.2/components/freertos/config/include/freertos/ 
  -I/home/sourish/esp-idf-v6.0.2/components/freertos/FreeRTOS-Kernel/include/freertos/  
  -I/home/sourish/c++_libs/googletest/googlemock/include 
  -I/home/sourish/c++_libs/googletest/googletest/include 
 g++ -std=c++23 -g Sorter_test.cpp -o Sorter_test -I$(pwd)/../components/libs/etl/include -I/home/sourish/esp-idf-v6.0.2/components/log/include/ -I/home/sourish/esp-idf-v6.0.2/components/freertos/config/include/freertos/ -I/home/sourish/esp-idf-v6.0.2/components/freertos/FreeRTOS-Kernel/include/freertos/ -I/home/sourish/c++_libs/googletest/googlemock/include -I/home/sourish/c++_libs/googletest/googletest/include

  

*/
#include "../main/ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "FreeRTOS.h"
#include "event_groups.h"

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

class SensorMOCK {
public:


    MOCK_METHOD( ( bool ) , turnOn , () );
    MOCK_METHOD( ( bool ) , turnOff , () );
    MOCK_METHOD( ( const void* ) , getSample , () , ( const ) );
    MOCK_METHOD( ( bool ) , stopListeningIT , () );
    MOCK_METHOD( ( bool ) , listenIT , () );
    

};
static_assert( SensorConcept< SensorMOCK > , "SensorMOCK doesn't meet concept requirments \n");

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

template < class Planner , class Stepper , class  Task>
class EngMOCK {
private:
    struct ConstructorKey{};
public:
    EngMOCK( ConstructorKey ){}

    static etl::optional< EngMOCK > create( const int8_t stepPin , const int8_t dirPin , Planner &planner ,Stepper &engine  , etl::optional<Task> &taskSpace){
        return etl::optional< EngMOCK >{ etl::in_place , ConstructorKey{} } ;
    }     
    
    MOCK_METHOD( ( bool ) , isRunning ,() , ( const ) );
    MOCK_METHOD( ( long ) , position ,());
    MOCK_METHOD( ( bool ) , start ,());
    MOCK_METHOD( ( bool ) , stop ,());
    MOCK_METHOD( ( void ) , move ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveTo ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveToCup ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
                            
};
static_assert( EngineConcept< EngMOCK< PlannerMOCK , StepperMOCK,  TaskMOCK > , PlannerMOCK , StepperMOCK,  TaskMOCK> , "EngMOCK does not meet concept requirments\n");


sorterStatus homingSlide(){
    auto sample = * static_cast < etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ;
    auto chamberColor = colorToNum( sample ) ;

    colorSensor.listenIT() ; 

    disksEngine.move( spinForever   ); 

    while ( !eventGroup.bitsWait(BIT_COLORSENSOR_INPUT , portMAX_DELAY )) {}
    

    disksEngine.stop( flush = true , instantly = true ) ;

    colorSensor.stopListeningIT() ;

    return sorterStatus::OK ;

}


using ::testing::_;
using ::testing::Return;


TEST( SorterHomingSlideTest , doesSensorDetect ){
    using mockEng = EngMOCK< PlannerMOCK , StepperMOCK , TaskMOCK> ;
    using mockEventFlags = EventFlagsMOCK< EventGroupHandle_t, EventBits_t   > ; 
    using mockSorter  = Sorter< EventFlagsMOCK , TaskMOCK , EngMOCK , SensorMOCK , EngMOCK , SensorMOCK >;

                                        EventGroupType &p_eventGroup , etl::array< EventGroupMembersType EventGroupType::* , 32 > p_membersMap,
    EventFlags_t eg ;
    auto& eventFlags  = mockEventFlags::create( ) ;
    auto& task  = TaskMOCK::create( ... ) ;

    auto& slideStepper  = StepperMOCK::create( ... ) ;
    auto& slidePlanner = PlannerMOCK::create( ... ) ;
    auto& slideEng = mockEng::create( .. ) ;

    auto& slideSensor;



    auto& disksPlanner  = PlannerMOCK::create( ... );
    auto& disksStepper = StepperMOCK::create( ... ) ;
    auto& disksEng = mockEng::create( ...  );

    auto& disksSensor = SensorMOCK::create( ... ) ;


    auto& sorter  = mockSorter::create( ) ;

    EXPECT_CALL( mockSensor , getSample().WillOnce(testing::Return(true)));
    //  .... and so on testing 
}
extern "C" app_main(){

    printf("Uruchamianie testow jednostkowych na systemie Linux...\n");
    
    // Inicjalizacja GMock/GTest (tworzymy sztuczne argumenty argc/argv)
    int argc = 1;
    char* argv[] = {(char*)"test_runner", nullptr};
    ::testing::InitGoogleMock(&argc, argv);
    
    // Uruchomienie wszystkich testów
    int result = RUN_ALL_TESTS();
    
    // Zakończenie programu (ważne na targecie Linux, by zwrócić kod błędu do terminala)
    exit(result);

}




