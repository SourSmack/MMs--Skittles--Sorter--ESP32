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
#include <format>
#include <random>
#include "mocks.hpp"

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

inline std::ostream& operator<<(std::ostream& os, const sorterErrFlags& flags) {
    // Wypisujemy rawMask jako liczbę szesnastkową dla łatwego czytania bitów
    constexpr auto names = magic_enum::enum_names< ErrCode >() ;

    if ( !static_cast<uint16_t>( flags.rawMask ) ){
        os << " ErrCode::OK " <<  std::endl ;
    }
    for ( auto i{0} ; i < 32 ; ++i){
        if ( static_cast<uint16_t>( flags.rawMask ) & ( 1 << i) ){
            os << "ErrCode::" << names[i] << std::endl  ;
        }
    }
    return os;
}

using SlideHomingTest = SorterTesting ;
using DisksHomingTest = SorterTesting ;
using SortingSingleCandyTest = SorterTesting ; 
using startSortingTest = SorterTesting ; 
using sortSingleCandyTEST = SorterTesting ;
using ::testing::_;
using ::testing::Return;

TEST_F( sortSingleCandyTEST , HappyPath){
    testing::InSequence seq ;

    auto testingValue{ 0 } ;
    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_, testing::_)).Times(1);

    EXPECT_CALL( disksSensor , getSample() ).Times(1).WillOnce( testing::Return( testingValue ) );

    EXPECT_CALL( slideEng , moveToCupImpl( cupsMoves[ testingValue ] , testing::_, testing::_)).Times(1) ;

    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_, testing::_)).Times(1) ;

    auto result = sorter->sortSingleCandy() ; 

    EXPECT_EQ( result , ErrCode::OK ) ;



}
TEST_F( startSortingTest , HappyPath){
    testing::InSequence sq ;
    EXPECT_CALL( sortingTask ,   start() ).Times(1);

    sorter->startSorting() ;
}


TEST_F( DisksHomingTest  , sensorDONTDetectsUnder5thAttempts ){
    testing::InSequence sq ;

    EXPECT_CALL( disksSensor , listenIT() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_DISKSSENSOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillRepeatedly( testing::Return( false )) ;
    EXPECT_CALL( disksEng , stop() ).Times(1).WillOnce( testing::Return( true )); 

    
    auto result = sorter->homingDisks() ;

    EXPECT_EQ( result , ErrCode::eventGroup ) ;  

}  


TEST_F( SlideHomingTest , sensorDONTDetectsUnder5thAttempts ){

    testing::InSequence sq ;
    EXPECT_CALL( slideSensor , listenIT() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( slideEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_TRANSOPTOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillRepeatedly( testing::Return( false )) ;
    
    EXPECT_CALL( slideEng , stop() ).Times(1).WillOnce( testing::Return( true )); 


    
    auto result = sorter->homingSlide() ;

    EXPECT_EQ( result , ErrCode::eventGroup );
}

TEST_F( DisksHomingTest  , sensorDetectsUnder5thAttempts ){
    testing::InSequence sq ;

    EXPECT_CALL( disksSensor , listenIT() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( disksEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_DISKSSENSOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( true ))
        .WillRepeatedly( testing::Return( false )) ;
    

    EXPECT_CALL( disksEng , stop() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( disksSensor , stopListeningIT() ).Times(1).WillOnce( testing::Return( true ));
    
    auto result = sorter->homingDisks() ;

    EXPECT_EQ( result , ErrCode::OK ) ;  

}  


TEST_F( SlideHomingTest , sensorDetectsUnder5thAttempts ){

    testing::InSequence sq ;
    EXPECT_CALL( slideSensor , listenIT() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( slideEng , moveImpl( testing::_ , testing::_ , testing::_ ) ).Times(1);

    EXPECT_CALL( eventFlags , bitsWait( BIT_TRANSOPTOR_INPUT , mockEventFlags::MAX_DELAY ) )
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( false ))
        .WillOnce( testing::Return( true ))
        .WillRepeatedly( testing::Return( false )) ;
    

    EXPECT_CALL( slideEng , stop() ).Times(1).WillOnce( testing::Return( true ));

    EXPECT_CALL( slideSensor , stopListeningIT() ).Times(1).WillOnce( testing::Return( true ));


    
    auto result = sorter->homingSlide() ;

    EXPECT_EQ( result , ErrCode::OK ) ;

}






