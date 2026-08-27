#pragma once
#include "cstdint"


struct motionBlock_t
{
    uint32_t ticks ;
    uint32_t steps ;
    bool dir : 1 ;

};


struct moveBlock_t
{
    // which engine has to make move
    
    
    

    // measureUnit / speedUnit^2 |   measureUnit^2 / speedUnit
    // describes accelaration and constant movement 
    float startAcc   , endAcc ; 

    float startSpeed , traversalSpeed , endSpeed ; 

    // how many units to make with assigned speed in regards to part of the move (start,traversal,end) 
    int startSteps{0} , traversalSteps{0} , endSteps  {0}; 

    constexpr moveBlock_t( int  steps , float ss =  0 , float sa = 5 , float ts = 100 , float es  = 0 , float ea = 5): startAcc(sa)  , endAcc(ea), startSpeed(ss) , traversalSpeed(ts) ,  endSpeed(es) {

        startSteps = (( ts * ts ) - ( ss * ss ))  / ( 2.0f*sa )  ; 
        endSteps =  (( es * es)  - ( ts * ts ))  / ( 2.0f * ea ); 
        traversalSteps = steps - ( startSteps + endSteps ) ;

        if ( startSteps > steps || endSteps > steps || ( traversalSteps < 0 ) || ( startSteps + endSteps  + traversalSteps) > steps ){
            startAcc = endAcc = 0 ;
            startSpeed = traversalSpeed = endSpeed  = 0 ; 
            startSteps =  traversalSteps =  endSteps = 0 ;
        }

    } 
};



struct moveInfo_t 
{
    bool IrqWhenFinished : 1  ; 
    bool enqueue : 1 ;

    moveInfo_t():IrqWhenFinished(false), enqueue(false){}
    moveInfo_t(bool irq , bool enq):IrqWhenFinished(irq), enqueue(enq){}
};