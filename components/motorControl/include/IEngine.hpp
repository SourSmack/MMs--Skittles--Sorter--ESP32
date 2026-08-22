
#pragma once
#include "moveStructures.hpp"
#include "config.hpp"
#include "etl/optional.h"
template <class T> class IPlanner;
template <class T> class IStepper ;

#define IEngineDeriv static_cast<T&>( *this ) 
template< class T>
class IEngine 
{
private: 
public:
    ~IEngine() = default ; 

    static etl::optional<T> create( int8_t stepPin , int8_t dirPin , IPlanner<PlannerType> &planner ,IStepper<StepperType> &engine    ){ return T::create( stepPin , dirPin ,planner ,engine    ); }
    
   // virtual void enqueue(const motionBlock_t &motion) = 0 ;
    //virtual void execute(void) = 0 ;
   
   // virtual void clear(void) = 0 ;
    bool isRunning()const { return IEngineDeriv.isRunning() ; } 
        
    
    long position( )  { return IEngineDeriv.position() ; } 


    /* move(...) -  Moves engine in relation to current position e.g. curr_pos = 400 , move = -100 (simplified), position after move = 300 
        moveTo(...) - Moves engine to absolute position in shortes possible way e.g. 
    */
    void move( const moveBlock_t &move  ,const  moveInfo_t flags = {}, const int wait = 0 ){ IEngineDeriv.move( move , flags, wait ); }
    void moveTo( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait = 0  ){ IEngineDeriv.moveTo( move , flags, wait );         } 
    void moveToCup( const moveBlock_t &move , const moveInfo_t flags = {} , const int wait = 0  ){ IEngineDeriv.moveToCup( move , flags, wait ); }
/*
     motionBlock = Data Transfer Object from - {engineNum, steps,direction,speed(start,traversal,end) } to {engineNum ticks,frequency } in regards to motion producer.
       Loads ready motion_block to reciever  if producer queue has motionBlocks.
       Number specifies how many motionBlocks should be loaded, loads as many as possible
       if not enough in producer queue (in respect to demand).
       It assures atomicity by turning off interrupts , so demand will always has highest priority 
    void update(const uint32_t blocksToUpdate  ){

    }

     Clears/flushes motionBloc queue 
    motionsToFlush specifes how many motionBlocks to flush 
    void flush(const uint32_t motionsToFlush  ) = 0 

    Halts execution of stepEngine 
     bool flush if true ,clears Queue 
     bool instantly if true , stops gently deacceleration ( makes it instant 
    void stop(const bool flush = false , const bool instantly = false ) = 0 ;

     Starts execution of stepEngine 
    void start(void) = 0 ;
   
*/
};

#undef IEngineDeriv