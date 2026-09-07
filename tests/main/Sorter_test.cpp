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
    static uint32_t MAX_DELAY ; 
};

template< class T , class N >
uint32_t EventGroupMOCK< T , N >::MAX_DELAY{ 1000 } ;
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
    MOCK_METHOD( ( int ) , getSample , () , ( const ) );
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
    void move( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait = 0  ){  moveImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ));
    void moveTo( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait  = 0  ){  moveToImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveToImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait ));
    void moveToCup( const moveBlock_t &move  ,const  moveInfo_t flags = {} , const int wait = 0  ){  moveToCupImpl( move , flags , wait ) ;}
    MOCK_METHOD( ( void ) , moveToCupImpl ,( const moveBlock_t &move  ,const  moveInfo_t flags , const int wait  ));
                            
};
static_assert( EngineConcept< EngMOCK< PlannerMOCK , StepperMOCK,  TaskMOCK > > , "EngMOCK does not meet concept requirments\n");


class SorterTesting : public ::testing::Test {
protected:
    using mockEng = EngMOCK<PlannerMOCK, StepperMOCK, TaskMOCK>;
    using mockEventFlags = EventGroupMOCK<EventGroupHandle_t, EventBits_t>;
    using GenericSorter = Sorter<mockEventFlags, TaskMOCK, mockEng, SensorMOCK, mockEng, SensorMOCK>;

    // =====================================================================
    // UWAGA: Zmienne poniżej inicjalizują się kaskadowo z góry na dół.
    // Nie zmieniaj ich kolejności, bo wywoła to błąd kompilacji.
    // =====================================================================

    EventGroupHandle_t eg{};

    // --- EVENT FLAGS ---
    // Inicjalizujemy opcjonal dokładnie tak samo jak przez 'auto' w Twoim kodzie, 
    // a pod nim od razu tworzymy wygodną referencję.
    etl::optional<mockEventFlags> tmp_eventFlags = mockEventFlags::create(eg, (BIT_DISKSSENSOR_INPUT | BIT_TRANSOPTOR_INPUT | BIT_SLIDE_ENGINE_FINISHED | BIT_DISK_ENGINE_FINISHED));
    mockEventFlags& eventFlags = *tmp_eventFlags;

    // --- SORTING TASK ---
    etl::optional<TaskMOCK> tmp_sortingTask = TaskMOCK::create(nullptr, nullptr, 2048, 5);
    TaskMOCK& sortingTask = *tmp_sortingTask;

    // --- SLIDE ---
    etl::optional<TaskMOCK> tmp_slideTask = TaskMOCK::create(nullptr, nullptr, 2048, 5);
    TaskMOCK& slideTask = *tmp_slideTask;

    etl::optional<StepperMOCK> tmp_slideStepper = StepperMOCK::create(0, 0);
    StepperMOCK& slideStepper = *tmp_slideStepper;

    etl::optional<PlannerMOCK> tmp_slidePlanner = PlannerMOCK::create();
    PlannerMOCK& slidePlanner = *tmp_slidePlanner;

    // EngMOCK używa wygenerowanych referencji, które są już bezpiecznie zainicjalizowane wyżej
    etl::optional<mockEng> tmp_slideEng = mockEng::create(0, 0, slidePlanner, slideStepper, slideTask);
    mockEng& slideEng = *tmp_slideEng;

    SensorMOCK slideSensor{}; // O ile ten Mock ma konstruktor bezargumentowy

    // --- DISKS ---
    etl::optional<TaskMOCK> tmp_disksTask = TaskMOCK::create(nullptr, nullptr, 2048, 5);
    TaskMOCK& disksTask = *tmp_disksTask;

    etl::optional<PlannerMOCK> tmp_disksPlanner = PlannerMOCK::create();
    PlannerMOCK& disksPlanner = *tmp_disksPlanner;

    etl::optional<StepperMOCK> tmp_disksStepper = StepperMOCK::create(0, 0);
    StepperMOCK& disksStepper = *tmp_disksStepper;

    etl::optional<mockEng> tmp_disksEng = mockEng::create(0, 0, disksPlanner, disksStepper, disksTask);
    mockEng& disksEng = *tmp_disksEng;

    SensorMOCK disksSensor{};

    // --- GŁÓWNY OBIEKT (Sorter) ---
    // Możemy przechować go w etl::optional i zbudować dopiero wewnątrz konstruktora,
    // gdzie wszystkie referencje (np. slideEng, disksEng) na pewno są już ustalone.
    etl::optional< GenericSorter > sorter ;

    SorterTesting() {
        // Konstrukcja głównego obiektu Sortera w miejscu
        sorter.emplace( eventFlags, sortingTask, slideEng, slideSensor, disksEng, disksSensor );
    }
};

using SlideHomingTest = SorterTesting ;
using DisksHomingTest = SorterTesting ;
using SortingSingleCandyTest = SorterTesting ; 

using ::testing::_;
using ::testing::Return;

TEST_F( DisksHomingTest  , sensorDONTDetectsUnder5thAttempts ){
    EXPECT_CALL( disksSensor , listenIT() ).Times(1);

    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_DISKSSENSOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillRepeatedly( testing::Return( false )) ;
    

    
    auto result = sorter->homingDisks() ;

    EXPECT_EQ( result , sorterStatus::ERRORhomingDisks ) ;  

}  


TEST_F( SlideHomingTest , sensorDONTDetectsUnder5thAttempts ){
    EXPECT_CALL( slideSensor , listenIT() ).Times(1);

    EXPECT_CALL( slideEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_TRANSOPTOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillRepeatedly( testing::Return( false )) ;
    


    
    auto result = sorter->homingSlide() ;

    EXPECT_EQ( result , sorterStatus::ERRORhomingSlide) ;
}

TEST_F( DisksHomingTest  , sensorDetectsUnder5thAttempts ){
    EXPECT_CALL( disksSensor , listenIT() ).Times(1);

    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_DISKSSENSOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( true ))
        .WillRepeatedly( testing::Return( false )) ;
    

    EXPECT_CALL( disksEng , stop() ).Times(1);

    EXPECT_CALL( disksSensor , stopListeningIT() ).Times(1);
    
    auto result = sorter->homingDisks() ;

    EXPECT_EQ( result , sorterStatus::OK ) ;  

}  


TEST_F( SlideHomingTest , sensorDetectsUnder5thAttempts ){
    EXPECT_CALL( slideSensor , listenIT() ).Times(1);

    EXPECT_CALL( slideEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_TRANSOPTOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( true ))
        .WillRepeatedly( testing::Return( false )) ;
    

    EXPECT_CALL( slideEng , stop() ).Times(1);

    EXPECT_CALL( slideSensor , stopListeningIT() ).Times(1);


    
    auto result = sorter->homingSlide() ;

    EXPECT_EQ( result , sorterStatus::OK ) ;
/*
  
    sorterStatus homingSlide(){
            auto sample = * static_cast < etl::string< COLORSENSOR_WORD_SIZE > * >( disksSensor.getSample() ) ;
            auto chamberColor = colorToNum( sample ) ;

            disksSensor.listenIT() ; 

            disksEngine.move( spinForever   ); 

            while ( !eventGroup.bitsWait(BIT_DISKSSENSOR_INPUT , portMAX_DELAY )) {}
            

            disksEngine.stop( ) ;

            disksSensor.stopListeningIT() ;

            return sorterStatus::OK ;

        }
    void startSorting() {
        sortingTask.start() ; 
    } 

    sorterStatus getStatus()const{ return status ; }

    sorterStatus stopSorting(){ 
        sortingTask.stop() ; 
        return sorterStatus::OK ;
    }
    static void _sortingFunction(void *pvParameter){
        auto& pair = *static_cast< 
                                    etl::pair<Sorter* , uint16_t > * 
                                                                    >
                                                                        ( pvParameter ) ;
        auto& [ instance , token ] = pair ;
        auto& [ eventGroup , sortingTask , slideEngine , slidePositionSensor , disksEngine , disksSensor , status  ] = instance ;

        if ( status != sorterStatus::OK ) return ;

        if ( instance.homingSlide() != sorterStatus::OK){
            status = sorterStatus::ERRORslideEngine ; 
            return ;
        } 
        if ( instance.homingDisks() != sorterStatus::OK ) {
            status = sorterStatus::ERRORdisksEngine; 
            return ; 
        }


        while ( ! FREETask::stopRequested( token ) ){
            disksEngine.move( fetchCandy ) ;

            const auto& sample = *static_cast< const etl::string< COLORSENSOR_WORD_SIZE > * >( disksSensor.getSample() ) ; 
            auto candyColor = colorToNum( sample ) ;

            switch ( candyColor ){
                case   RED  : 
                    slideEngine.moveToCup( cup::RED ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   ORANGE  :
                    slideEngine.moveToCup( cup::ORANGE ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   YELLOW  :
                    slideEngine.moveToCup( cup::YELLOW ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   PURPLE  :
                    slideEngine.moveToCup( cup::PURPLE ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   GREEN  : 
                    slideEngine.moveToCup( cup::GREEN ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                default :
                    slideEngine.moveToCup( cup::UNKNOWN ) ; 
                    disksEngine.move( flushCandy  );
                    break ;
            }


        }


    }


*/
}

extern "C" void app_main(){

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




