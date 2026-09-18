#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Nema.hpp"
#include "mocks.hpp"

class NemaTesting : public ::testing::Test{
protected:




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
