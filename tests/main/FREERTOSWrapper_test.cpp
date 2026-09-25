#include "FreeRTOSWrapper.hpp"
#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"


class MockFreeRTOS {
public:
    MOCK_METHOD(BaseType_t, xTaskNotify, (TaskHandle_t, uint32_t, eNotifyAction));
    MOCK_METHOD(BaseType_t, xTaskCreate, (TaskFunction_t, const char*, configSTACK_DEPTH_TYPE, void*, UBaseType_t, TaskHandle_t*));
    MOCK_METHOD(void, vTaskDelay, (const TickType_t));
};

MockFreeRTOS* g_freeRTOS = nullptr;
extern "C" {

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction) {
    if(g_freeRTOS) return g_freeRTOS->xTaskNotify(xTaskToNotify, ulValue, eAction);
    return 0; // fallback
}

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask) {
    if(g_freeRTOS) return g_freeRTOS->xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
    return 0;
}
}

class FREERTOSTest : public ::testing::Test {
protected:
    MockFreeRTOS rtosMock;
    FREETask dummyTask{ dummyFunction, nullptr, 2048, 5 };

    void SetUp() override {
        g_freeRTOS = &rtosMock;
    }

    void TearDown() override {
        g_freeRTOS = nullptr;
    }
};
using  FREERTOS_notify_test = FREERTOSTest ; 
using  FREERTOS_requestStop_test = FREERTOSTest ; 
using  FREERTOS_join_test = FREERTOSTest ; 
using  FREERTOS_start_test = FREERTOSTest ; 

TEST_F(FREERTOS_notify_test, ReturnsTrueWhenNotifySucceeds) {
    // Oczekujemy wywołania xTaskNotify z odpowiednimi parametrami
    // Zauważ znak zapytania `_` dla argumentów, które nas w tej chwili nie obchodzą
    EXPECT_CALL(rtosMock, xTaskNotify(testing::_, testing::_, eSetsBits))
        .Times(1)
        .WillOnce(testing::Return(pdPASS)); // Symulujemy, że RTOS zwraca OK

    bool result = dummyTask.notify(0x01);

    EXPECT_TRUE(result);
}

// Test 2: Sprawdzamy ścieżkę błędu (RTOS zwraca błąd)
TEST_F(FREERTOS_notify_test, ReturnsFalseWhenNotifyFails) {
    EXPECT_CALL(rtosMock, xTaskNotify(testing::_, testing::_, eSetsBits))
        .Times(1)
        .WillOnce(testing::Return(0)); // Symulujemy błąd RTOSa (coś innego niż pdPASS)

    bool result = dummyTask.notify(0x01);

    EXPECT_FALSE(result);
}
/*
bool FREETask::notify( const uint32_t message )   {
    auto result = xTaskNotify( taskHandle , message , eSetsBits ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}

bool FREETask::requestStop()  {
    auto result = xTaskNotify( taskHandle , stopBit , eSetsBits  ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}
bool FREETask::join(){
    if ( !taskRunning ) return true ;
    if ( !requestStop() ) return false ;
    while ( taskRunning ){ taskYIELD(); }
    return true  ;
}

bool FREETask::start() {
    auto result  = xTaskCreate( task , "task" , stackSize , arg  , priority , &taskHandle ) ;
    if ( !pdPASS ) return false ; 
    return true; 
} 


bool FREETask::stopRequested(   ){
    uint32_t result { 0 } ;
    if ( !xTaskNotifyWait( 0 , stopBit ,  &result , 0  )) return false ;
    if ( result & stopBitMask ) return true ;
    return false ;
}
bool FREETask::waitForNotify( const uint32_t waitBitMask , const uint32_t p_delay ){
    uint32_t result { 0 } ;
    auto delay = ( p_delay == MAX_DELAY ) ? (  portMAX_DELAY ) : pdMS_TO_TICKS( p_delay ) ;
    if ( !xTaskNotifyWait( 0 ,  waitBitMask , &result , delay )) return false ;

    if ( result & waitBitsMask )  return true ;

    return false  ;

}

void FREETask::waitMs( const uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms ) ) ;
}

*/
TEST_F( )