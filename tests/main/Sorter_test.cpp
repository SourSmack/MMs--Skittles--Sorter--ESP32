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
#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Sorter.hpp"

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
 
};
struct DummyGroup{} ;
struct DummyMember{} ;
static_assert( EventGroupConcept < EventGroupMOCK< EventGroupHandle_t, EventBits_t   >> ) ;

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

template < class T , class N , class  M>
class EngMOCK {
private:
    struct ConstructorKey{};
public:
    using Planner = T;
    using Stepper = N ;
    using Task  = M ;
    EngMOCK( ConstructorKey ){}

    static etl::optional< EngMOCK > create( const int8_t stepPin , const int8_t dirPin , Planner &planner ,Stepper &engine  , Task &taskSpace){
        return etl::optional< EngMOCK >{ etl::in_place , ConstructorKey{} } ;
    }     
    
    static void dataRelayTask(void * arg){ }

    MOCK_METHOD( ( bool ) , isRunning ,() , ( const ) );
    MOCK_METHOD( ( long ) , position ,());
    MOCK_METHOD( ( bool ) , start ,());
    MOCK_METHOD( ( bool ) , stop ,());
    MOCK_METHOD( ( void ) , move ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveTo ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    MOCK_METHOD( ( void ) , moveToCup ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
                            
};
static_assert( EngineConcept< EngMOCK< PlannerMOCK , StepperMOCK,  TaskMOCK > > , "EngMOCK does not meet concept requirments\n");







using ::testing::_;
using ::testing::Return;


TEST( SorterHomingSlideTest , doesSensorDetect ){
    using mockEng = EngMOCK< PlannerMOCK , StepperMOCK , TaskMOCK> ;
    using mockEventFlags = EventGroupMOCK< EventGroupHandle_t, EventBits_t   > ; 
    Sorter  = Sorter< mockEventFlags , TaskMOCK , mockEng , SensorMOCK , mockEng , SensorMOCK >;

    EventGroupHandle_t eg{} ;
    auto& eventFlags  = mockEventFlags::create(  eg , ( BIT_COLORSENSOR_INPUT | BIT_TRANSOPTOR_INPUT  | BIT_SLIDE_ENGINE_FINISHED | BIT_DISK_ENGINE_FINISHED )  ) ;
    auto& sortingTask = TaskMOCK::create( nullptr , nullptr , 2048 , 5) ;

    auto& slideTask  = TaskMOCK::create( nullptr ,nullptr ,  2048 , 5  ) ;

    auto& slideStepper  = StepperMOCK::create( 0 , 0 ) ;
    auto& slidePlanner = PlannerMOCK::c argreate() ;
    auto& slide= mockEng::create(  0 , 0 ,  slidePlanner ,slideStepper  , slideTask ) ;
    auto slideSensor = SensorMOCK{};


    auto& disksTask = TaskMOCK::create( nullptr , nullptr , 2048 ,5) ;
    auto& disksPlanner  = PlannerMOCK::create() ; 
    auto& disksStepper = StepperMOCK::create( 0 , 0 ) ;
    auto& disksEng = mockEng::create(  0 , 0 ,  disksPlanner ,disksStepper  , disksTask   );

    auto disksSensor = SensorMOCK{} ;


    auto sorter  = mockSorter{ eventFlags , sortingTask , slideEng , slideSensor , disksEng , disksSensor }  ;

     EXPECT_CALL( mockSensor , getSample().WillOnce(testing::Return(true)));
     //  .... and so on testing 

void app_main(){

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




