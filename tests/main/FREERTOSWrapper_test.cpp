#include "FreeRTOSWrapper.hpp"
#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class MockFreeRTOS {
public:
    MOCK_METHOD(BaseType_t, xTaskGenericNotify, (TaskHandle_t, UBaseType_t, uint32_t, eNotifyAction, uint32_t*));
    
    MOCK_METHOD(BaseType_t, xTaskCreatePinnedToCore, (TaskFunction_t, const char*, const configSTACK_DEPTH_TYPE, void*, UBaseType_t, TaskHandle_t*, const BaseType_t));
    
    MOCK_METHOD(void, vTaskDelay, (const TickType_t));
    
    MOCK_METHOD(BaseType_t, xTaskGenericNotifyWait, (UBaseType_t, uint32_t, uint32_t, uint32_t*, TickType_t));

    MOCK_METHOD(void , myTaskYield , () );
};

MockFreeRTOS* g_freeRTOS = nullptr;
TaskHandle_t g_testTaskHandle = nullptr; 

extern "C" {

BaseType_t __real_xTaskGenericNotify(TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue);
BaseType_t __real_xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask, const BaseType_t xCoreID);
void __real_vTaskDelay(const TickType_t xTicksToDelay);
BaseType_t __real_xTaskGenericNotifyWait(UBaseType_t uxIndexToWaitOn, uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait);
void __real_vPortYield(void);

BaseType_t __wrap_xTaskGenericNotify(TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue) {
    if(g_freeRTOS && xTaskGetCurrentTaskHandle() == g_testTaskHandle) {
        return g_freeRTOS->xTaskGenericNotify(xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue);
    }
    return __real_xTaskGenericNotify(xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue);
}

BaseType_t __wrap_xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask, const BaseType_t xCoreID) {
    if(g_freeRTOS && xTaskGetCurrentTaskHandle() == g_testTaskHandle) {
        return g_freeRTOS->xTaskCreatePinnedToCore(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask, xCoreID);
    }
    return __real_xTaskCreatePinnedToCore(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask, xCoreID);
}

void __wrap_vTaskDelay(const TickType_t xTicksToDelay) {
    if(g_freeRTOS && xTaskGetCurrentTaskHandle() == g_testTaskHandle) {
        g_freeRTOS->vTaskDelay(xTicksToDelay);
    } else {
        __real_vTaskDelay(xTicksToDelay);
    }
}

BaseType_t __wrap_xTaskGenericNotifyWait(UBaseType_t uxIndexToWaitOn, uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait) {
    if(g_freeRTOS && xTaskGetCurrentTaskHandle() == g_testTaskHandle) {
        return g_freeRTOS->xTaskGenericNotifyWait(uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait);
    }
    return __real_xTaskGenericNotifyWait(uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait);
}

void __wrap_vPortYield(void) {
    if (g_freeRTOS && xTaskGetCurrentTaskHandle() == g_testTaskHandle) {
        g_freeRTOS->myTaskYield();
        return; 
    }
    __real_vPortYield();
}
}

void dummyFunction( void * arg){
    while ( !FREETask::stopRequested() ){
        std::cout << "doing something ..." << std::endl ;
    }
    std::cout << "stopRequested() called , exiting dummyFunction" << std::endl ;

}

class FREERTOSTest : public ::testing::Test {
protected:
    MockFreeRTOS rtosMock;
    FREETask dummyTask{ dummyFunction, nullptr, 2048, 5 };

    void SetUp() override {
        g_freeRTOS = &rtosMock;
        
        g_testTaskHandle = xTaskGetCurrentTaskHandle(); 
        
        dummyTask.taskHandle = (TaskHandle_t)1;
    }

    void TearDown() override {
        g_freeRTOS = nullptr;
    }
    void setTaskRunning(bool state) {
        dummyTask.taskRunning = state;
    }
};
using  FREERTOS_notify_test = FREERTOSTest ; 
using  FREERTOS_requestStop_test = FREERTOSTest ; 
using  FREERTOS_join_test = FREERTOSTest ; 
using  FREERTOS_start_test = FREERTOSTest ; 
using  FREERTOS_stopRequested_test = FREERTOSTest ; 
using  FREERTOS_waitForNotify_test = FREERTOSTest ;
using  FREERTOS_waitMs_test = FREERTOSTest ;

using testing::_; 
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgPointee;

TEST_F(FREERTOS_notify_test, ReturnsTrueWhenNotifySucceeds) {
    EXPECT_CALL(rtosMock, xTaskGenericNotify(testing::_, testing::_, testing::_, eSetBits, testing::_))
        .Times(1)
        .WillOnce(testing::Return(pdPASS)); 

    bool result = dummyTask.notify(0x01);

    EXPECT_TRUE(result);
}


TEST_F(FREERTOS_notify_test, ReturnsFalseWhenNotifyFails) {
    EXPECT_CALL(rtosMock, xTaskGenericNotify(testing::_, testing::_, testing::_, eSetBits, testing::_))
        .Times(1)
        .WillOnce(testing::Return(0)); 

    bool result = dummyTask.notify(0x01);

    EXPECT_FALSE(result);
}
/*
bool FREETask::notify( const uint32_t message )   {
    auto result = xTaskNotify( taskHandle , message , eSetBits ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}*/
TEST_F( FREERTOS_requestStop_test , notifiedSuccess){
    EXPECT_CALL( rtosMock , xTaskGenericNotify( testing::_, testing::_, testing::_, eSetBits, testing::_ )).Times(1).WillOnce( testing::Return(pdPASS));
    
    auto result = dummyTask.requestStop() ; 
    EXPECT_TRUE( result ) ;
}

TEST_F( FREERTOS_requestStop_test , notifiedNOTSuccess){
    EXPECT_CALL( rtosMock , xTaskGenericNotify( testing::_, testing::_, testing::_, eSetBits, testing::_ )).Times(1).WillOnce( testing::Return(pdFAIL));
    
    auto result = dummyTask.requestStop() ; 
    EXPECT_FALSE( result ) ;
}
/*
bool FREETask::requestStop()  {
    auto result = xTaskNotify( taskHandle , stopBit , eSetBits  ) ;
    if ( result != pdPASS) return false ; 
    return true ;
}*/

TEST_F( FREERTOS_join_test , happyPath){
    
    setTaskRunning( true ) ;
    

    EXPECT_CALL(rtosMock, xTaskGenericNotify(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return( pdPASS )); 


  EXPECT_CALL( rtosMock , myTaskYield() )
        .WillOnce(testing::Invoke([this]() { 
            this->setTaskRunning(false); 
        }));
    EXPECT_TRUE( dummyTask.join() );

    
}

TEST_F( FREERTOS_join_test , requestStopFAIL){
    
    setTaskRunning( true ) ;
    
EXPECT_CALL(rtosMock, xTaskGenericNotify(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return( pdFAIL )); 


    EXPECT_FALSE( dummyTask.join() );

    
    setTaskRunning( false ) ;
}
/*
bool FREETask::join(){
    if ( !taskRunning ) return true ;
    if ( !requestStop() ) return false ;
    while ( taskRunning ){ taskYIELD(); }
    return true  ;
}
*/
TEST_F( FREERTOS_start_test, happyPath ){
    EXPECT_CALL( rtosMock , xTaskCreatePinnedToCore( _  , _ , _ , _ , _ , _ , _)).Times(1).WillOnce( testing::Return( pdPASS));
    EXPECT_TRUE( dummyTask.start() ) ;
}

TEST_F( FREERTOS_start_test, xTaskCreateFAIL ){
    EXPECT_CALL( rtosMock , xTaskCreatePinnedToCore( _  , _ , _ , _ , _ , _ , _)).Times(1).WillOnce( testing::Return( pdFAIL ));
    EXPECT_FALSE( dummyTask.start() ) ;
}
/*
bool FREETask::start() {
    auto result  = xTaskCreate( task , "task" , stackSize , arg  , priority , &taskHandle ) ;
    if ( !pdPASS ) return false ; 
    return true; 
} 
*/
TEST_F( FREERTOS_stopRequested_test , noNotificationAvaiable ){
    
    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(Return(pdFAIL));

    EXPECT_FALSE( FREETask::stopRequested() );
}

TEST_F( FREERTOS_stopRequested_test , incorrectSignal ){
    
    uint32_t incorrectSignal = 0x00; 

    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(incorrectSignal), 
            Return(pdPASS)               
        ));

    EXPECT_FALSE( FREETask::stopRequested() );
}
TEST_F( FREERTOS_stopRequested_test , happyPath ){
    
    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>( FREETask::stopBit ), 
            Return(pdPASS)                           
        ));

    EXPECT_TRUE( FREETask::stopRequested() );
}
/*
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

    if ( result & waitBitMask )  return true ;

    return false  ;

}

void FREETask::waitMs( const uint32_t ms ) {
    vTaskDelay( pdMS_TO_TICKS( ms ) ) ;
}

*/

TEST_F(FREERTOS_waitForNotify_test, returnsFalseOnTimeout) {
    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(Return(pdFAIL));

    EXPECT_FALSE( dummyTask.waitForNotify(0x01, 1000) );
}

TEST_F(FREERTOS_waitForNotify_test, returnsFalseOnIncorrectBit) {
    uint32_t expectedBit = 0x01;
    uint32_t receivedBit = 0x02;

    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(receivedBit), 
            Return(pdPASS)                 
        ));

    EXPECT_FALSE( dummyTask.waitForNotify(expectedBit, 1000) );
}

TEST_F(FREERTOS_waitForNotify_test, returnsTrueOnCorrectBit) {
    uint32_t expectedBit = 0x08;
    
    EXPECT_CALL(rtosMock, xTaskGenericNotifyWait(_, _, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(expectedBit),
            Return(pdPASS)
        ));

    EXPECT_TRUE( dummyTask.waitForNotify(expectedBit, 1000) );
}

TEST_F(FREERTOS_waitMs_test, callsVTaskDelayWithCorrectTicks) {
    uint32_t msToWait = 500;
    TickType_t expectedTicks = pdMS_TO_TICKS(msToWait);

    EXPECT_CALL(rtosMock, vTaskDelay(expectedTicks))
        .Times(1);

    dummyTask.waitMS(msToWait);
}