
#pragma once
#include "moveStructures.hpp"




class IEngine 
{
private: 
public:
    virtual ~IEngine() = default ; 
    
   // virtual void enqueue(const motionBlock_t &motion) = 0 ;
    //virtual void execute(void) = 0 ;
    
   // virtual void clear(void) = 0 ;
    virtual bool isRunning(const uint8_t engineNum)const = 0 ;
    virtual long position(const uint8_t engineNum)  = 0 ;


    /* move(...) -  Moves engine in relation to current position e.g. curr_pos = 400 , move = -100 (simplified), position after move = 300 
        moveTo(...) - Moves engine to absolute position in shortes possible way e.g. 
    */
    virtual void move( const moveBlock_t &move  ,const  moveInfo_t flags = {}, const int wait = 0 ) = 0 ;
    virtual void moveTo( const moveBlock_t &move , const moveInfo_t flags = {}, const int wait = 0  ) = 0 ;
    virtual void moveToCup( const moveBlock_t &move , const moveInfo_t flags = {} , const int wait = 0  ) = 0 ;

    /* motionBlock = Data Transfer Object from - {engineNum, steps,direction,speed(start,traversal,end) } to {engineNum ticks,frequency } in regards to motion producer.
       Loads ready motion_block to reciever  if producer queue has motionBlocks.
       Number specifies how many motionBlocks should be loaded, loads as many as possible
       if not enough in producer queue (in respect to demand).
       It assures atomicity by turning off interrupts , so demand will always has highest priority */
    virtual void update(const uint32_t blocksToUpdate  ) = 0 ;

    /* Clears/flushes motionBloc queue 
    motionsToFlush specifes how many motionBlocks to flush */
    virtual void flush(const uint32_t motionsToFlush  ) = 0 ;

    /* Halts execution of stepEngine 
     bool flush if true ,clears Queue 
     bool instantly if true , stops gently deacceleration ( makes it instant )*/
    virtual void stop(const bool flush = false , const bool instantly = false ) = 0 ;

    /* Starts execution of stepEngine */
    virtual void start(void) = 0 ;
   

};