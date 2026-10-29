#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mocks.hpp"
#include "ScurvePlanner.hpp"
#include "moveStructures.hpp"



using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Mock;

using RESULT = Scurve::RESULT ;


/*struct Move {     
    double steps{ 0 } ;
    // measureUnit / speedUnit^2 |   measureUnit^2 / speedUnit
    // describes accelaration and constant movement 
    double startAcc{ 0 }   , maxAcc { 0 },  endAcc{ 0 } ; 

    double startSpeed{ 0 } , traversalSpeed{ 0 } , endSpeed{ 0 } ; 

    // how many units to make with assigned speed in regards to part of the move (start,traversal,end) 
    double startSteps{0} , traversalSteps{0} , endSteps  {0}; 

};*/


class EnqueueCallbackMock {
public:
    MOCK_METHOD(void, enqueue, (const RESULT& move));
};
static EnqueueCallbackMock* g_callbackMock = nullptr;

void testAdapter(const RESULT& move) {
    if (g_callbackMock) g_callbackMock->enqueue(move);
}

// ------------------------------------------------------------------
// Fixture Główne
// ------------------------------------------------------------------

class ScurveTesting : public ::testing::Test {
protected:
    TaskMOCK taskMock ;
    ScurvePlanner< moveBlock_t ,  TaskMOCK > planner{ taskMock , testAdapter}; 

    void runTask(){
        ScurvePlanner< moveBlock_t, TaskMOCK >::scurveTask( &planner ) ;
    }
    void SetUp() override {
        g_callbackMock = new EnqueueCallbackMock();
    }

    void TearDown() override {
        delete g_callbackMock;
        g_callbackMock = nullptr;
        
        
        Mock::VerifyAndClearExpectations( &staticMethodsTaskMOCK );
    }
};

// ==============================================================================
// TESTY: start()
// ==============================================================================
using StartTest = ScurveTesting;


TEST_F(StartTest, failsWhenTaskIsAlreadyRunning) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(true));

    EXPECT_FALSE(planner.start(testAdapter));
}

// Krok 2: Wychodzi na `if ( !taskHandle.start() )`
TEST_F(StartTest, failsWhenTaskStartFails) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(false));
    
    EXPECT_CALL(taskMock, start())
        .WillOnce(Return(false));

    EXPECT_FALSE(planner.start(testAdapter));
}

// Krok 3: Przechodzi wszystkie warunki (sukces)
TEST_F(StartTest, succeedsWhenTaskStartsSuccessfully) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(false));
    
    EXPECT_CALL(taskMock, start())
        .WillOnce(Return(true));

    EXPECT_TRUE(planner.start(testAdapter));
}

// ==============================================================================
// TESTY: stop()
// ==============================================================================
using StopTest = ScurveTesting;

// Krok 1: Wychodzi na `if ( !taskHandle.isRunning() )`
TEST_F(StopTest, succeedsImmediatelyIfNotRunning) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(false));

    EXPECT_TRUE(planner.stop());
}

// Krok 2: Wychodzi na `if ( !taskHandle.join() )`
TEST_F(StopTest, failsIfTaskJoinFails) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(true));
        
    EXPECT_CALL(taskMock, join())
        .WillOnce(Return(false));

    EXPECT_FALSE(planner.stop());
}

// Krok 3: Przechodzi wszystkie warunki (sukces)
TEST_F(StopTest, succeedsIfTaskJoinSucceeds) {
    EXPECT_CALL(taskMock, isRunning())
        .WillOnce(Return(true));
        
    EXPECT_CALL(taskMock, join())
        .WillOnce(Return(true));

    EXPECT_TRUE(planner.stop());
}

// ==============================================================================
// TESTY: pause() / resume()
// ==============================================================================
using ControlBitsTest = ScurveTesting;

// pauseBit w ScurvePlanner ma wartość 1 << 1 (czyli 2)
TEST_F(ControlBitsTest, pauseFailsIfNotifyFails) {
    EXPECT_CALL(taskMock, notify(2)).WillOnce(Return(false));
    EXPECT_FALSE(planner.pause());
}

TEST_F(ControlBitsTest, pauseSucceedsIfNotifySucceeds) {
    EXPECT_CALL(taskMock, notify(2)).WillOnce(Return(true));
    EXPECT_TRUE(planner.pause());
}

// resumeBit ma wartość 1 << 2 (czyli 4)
TEST_F(ControlBitsTest, resumeFailsIfNotifyFails) {
    EXPECT_CALL(taskMock, notify(4)).WillOnce(Return(false));
    EXPECT_FALSE(planner.resume());
}

TEST_F(ControlBitsTest, resumeSucceedsIfNotifySucceeds) {
    EXPECT_CALL(taskMock, notify(4)).WillOnce(Return(true));
    EXPECT_TRUE(planner.resume());
}

// ==============================================================================
// TESTY: enqueue() oraz obsługa MAXBUFFOR
// ==============================================================================
using EnqueueTest = ScurveTesting;

// Test podstawowy: przyjęcie ruchu
TEST_F(EnqueueTest, successfullyQueuesMotion) {
    moveBlock_t motion{};
    EXPECT_TRUE(planner.enqueue(motion));
}

// Test integracyjny bufora: zapełnianie krok po kroku
TEST_F(EnqueueTest, fillsBufferAndRejectsExcess) {
    moveBlock_t motion{};
    
    // Zapełniamy do limitu (MAXBUFFOR = 64)
    for (int i = 0; i < 64; ++i) {
        EXPECT_TRUE(planner.enqueue(motion)) << "Powinno dodać element na indeksie " << i;
    }

    // Bufor jest pełny, test pierwszego warunku w enqueue(): if(movesQ.full())
    EXPECT_FALSE(planner.enqueue(motion)) << "Element 65 powinien zostać odrzucony!";
}

// ==============================================================================
// TESTY LOGIKI: Pętla Taska (Dzięki staticMethodsTaskMOCK wreszcie to możliwe!)
// ==============================================================================
using TaskLoopTest = ScurveTesting;

// Testujemy, czy funkcja Taska zatrzyma się, gdy `stopRequested()` zwróci true
TEST_F(TaskLoopTest, taskExitsOnStopRequest) {
    // Od razu przy pierwszym sprawdzeniu warunku pętli zwracamy "true"
testing::InSequence seq ; 
    EXPECT_CALL(staticMethodsTaskMOCK, stopRequested());
    
    EXPECT_CALL( staticMethodsTaskMOCK , waitForNotify( _ , _ )).WillOnce( Return( false )) ;



           
}



