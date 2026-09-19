#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Nema.hpp"
#include "mocks.hpp"

class NemaTesting : public ::testing::Test{
protected:

    PlannerMOCK planner ;
    StepperMOCK stepper ;
    TaskMOCK task ;

    Nema< PlannerMOCK , StepperMOCK , TaskMOCK > nemaEngine{ 0 , 0 , planner , stepper , task } ;


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
using start  = NemaTesting ;

using ::testing::_;
using ::testing::Return;

TEST_F( start , HappyPath){

    testing::InSequence seq ;
    
    EXPECT_CALL( stepper , start() ).Times(1).WillOnce( testing::Return( true ) ) ;
/*    bool start()  {
        stepper.start();
        scurve.start() ; 
        task.start() ;

        running  = true ;

        return true; 
     }
*/ 

    EXPECT_CALL( planner , start() ).Times(1).WillOnce( testing::Return( true ) ) ;

    EXPECT_CALL( task , start() ).Times(1).WillOnce( testing::Return( true ) ) ;

    auto result = nemaEngine.start() ;

    EXPECT_EQ( result , true ) ;


}