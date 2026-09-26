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
    testing::NiceMock< TaskMOCK> task ;

    Nema< PlannerMOCK , StepperMOCK , TaskMOCK > nemaEngine{ 0 , 0 , planner , stepper , task } ;
    const motionBlock_t VALID_BLOCK { 1, 2, true };
    const motionBlock_t INVALID_BLOCK { 0, 0, false };
    

};

using start  = NemaTesting ;
using moveToCup  = NemaTesting ;

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

TEST_F(moveToCup , HappyPathEnqueue){
    EXPECT_CALL(stepper, start()).WillOnce(testing::Return(true));
    EXPECT_CALL(planner, start()).WillOnce(testing::Return(true));
    EXPECT_CALL(task, start()).WillOnce(testing::Return(true));
    
    nemaEngine.start(); // <--- KLUCZ! Teraz running = true!
    testing::InSequence seq ; 

    EXPECT_CALL( planner , enqueue( testing::_ ) ).Times(1); 

    nemaEngine.moveToCup( moveBlock_t{ 0 } ,  moveInfo_t{ true , true  }, 0 ); 
/*void moveToCup( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait  = 0   ){
        // seperate task that moves from plannner to stepper and seperate task for planner that makes moves 
        // moveToCup  just manages whether instantly make moveo or enqu in normal manner
        // manages que position motionBlock_t
        if ( !running) return ;

        if  ( flags.enqueue ){

            scurve->enqueue( move ) ;
        }
        else{
            // TODO (MAYBE) stop engine -> do new steps 
            // rn it is sloppy bcs we only skip Planner queue which is unintuitive imo 
            // so I'd have to have direct engine steering api 49kkkkk
            scurve->stop() ;  
            while ( auto motion  = scurve->calculateFrequency( move ) ){
                stepper->enqueue( motion ) ; 
            }
            scurve->start();
        }
    }
*/

}


TEST_F( moveToCup , HappyPathNotEnqueue){
    EXPECT_CALL(stepper, start()).WillOnce(testing::Return(true));
    EXPECT_CALL(planner, start()).WillOnce(testing::Return(true));
    EXPECT_CALL(task, start()).WillOnce(testing::Return(true));
    
    nemaEngine.start();    
    

    EXPECT_CALL( planner , stop()).Times( 1 ) ;
    
    EXPECT_CALL( planner , calculateFrequency( testing::_ ))
        .WillOnce( testing::Return( VALID_BLOCK ))
        .WillOnce( testing::Return( VALID_BLOCK ))
        .WillOnce( testing::Return( VALID_BLOCK ))
        .WillOnce( testing::Return( VALID_BLOCK ))
        .WillOnce( testing::Return( INVALID_BLOCK )) ;

    EXPECT_CALL( stepper , enqueue( testing::_ )).Times(4 ) ;
    EXPECT_CALL( planner , start() ).Times( 1 ) ;

    nemaEngine.moveToCup( moveBlock_t{ 0 } ,  moveInfo_t{ false  , false  }, 0 ) ;
}


