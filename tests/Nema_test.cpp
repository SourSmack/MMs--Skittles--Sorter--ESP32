#include "../components/motorControl/include/Nema.hpp"
#include "etl/optional.h"
#include <thread>
#include <chrono>
#include <iostream>

class MOCKplanner{
#define LEFT true 
public:
    
    static etl::optional< MOCKplanner >  create(){  MOCKplanner tmp{} ; return tmp ; }
    motionBlock_t calculateFrequency(const moveBlock_t &move)   { 
        static uint32_t count { 0 } ;
        if ( count == 10 ) return  motionBlock_t{ 0 , 0, 0 } ;
        count++ ;
        

        std::cout << "Planner: calculateFrequency() \n" ; 
        
        return motionBlock_t{ ( 10u + count )   , ( 10u+ ( count * count))  ,1 } ;
    } 
    void stop(){ std::cout << " Planner: stop() \n "; }
    void start(){ std::cout << "Planner: start() \n"; }
    void enqueue( const moveBlock_t &move ){ std::cout << "Planner: enqueue() \n";}
    motionBlock_t recieve(){ 
        std::cout << "Planner : recieve() \n";
        static uint32_t a { 0 } ; 
        static uint32_t b { 0 } ; 
        auto tmp  { motionBlock_t{ ++a , ++b , LEFT }} ;
        return tmp ;
    }

};

class MOCKstepper {
    MOCKstepper() = default ;
public:
    static  etl::optional< MOCKstepper >  create( uint16_t step , uint16_t dir ){  MOCKstepper tmp{}; return tmp;}

    void enqueue( motionBlock_t  motion){  std::cout << "STEPPER: Enqueueing motion \n Engine is running in background ... \n"; }  
    
    void stop(bool instant = false  ) { std::cout << "Stepper: stop() \n" ;} 
    void start(){ std::cout << "Stepper : start() \n" ; } 


};


class MOCKtask{
public:
    inline static bool stop  { false };
    inline static bool running { false } ;
    inline static uint8_t mess { 0 } ;
    void (*taskFunc)( void*arg ) { nullptr };
    void * arg { nullptr } ;
    

    static  etl::optional< MOCKtask > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority ){  
        MOCKtask tmp{} ;

        tmp.taskFunc = task ;
        tmp.arg = arg ;
        return tmp;
    }

    void notify( uint8_t message ){  mess = message ; }
    bool requestStop()  { auto count { 0 }  ; count++ ;  if ( count >= 10 ) return true; stop = false ;}
    bool join(){  while ( running){} ; return true;  }

    static bool stopRequested(){ return stop ; }
    static bool notifyWait( uint8_t message , uint32_t delay ){  
        std::cout << "MOCKtask : notifyWait()  " << message << std::endl ;
        return true ;
    } 
    static void waitMS( uint32_t ms ){  std::this_thread::sleep_for( std::chrono::milliseconds( ms ) ); } ;

    void _playTask(){  taskFunc( arg ) ;}
};


int main(){
    #define stepPin 0 
    #define dirPin 1 
    auto  planner = *MOCKplanner::create() ; 
    auto stepper = *MOCKstepper::create( stepPin , dirPin  ) ;
    etl::optional<MOCKtask> taskspace;  

    using NemaMOCK = Nema< MOCKplanner , MOCKstepper , MOCKtask >;
    auto nemaEng = *NemaMOCK::create(stepPin , dirPin ,planner , stepper , taskspace  );
    
    moveBlock_t testMove1 { 50 } ;
    moveInfo_t notQueInf { false , false } ;
    moveInfo_t QueInf { false , true } ;

    // making move directly (without queue) 
    nemaEng.moveToCup( testMove1 ,  notQueInf ) ; // should do 10 times 



    // making move via queue ( spoofing dataRelayTask) 
    // scenario where Planners queue is full and it is ready to be transfered via dataRelayTask ( we'll be simulationg it ) 

    nemaEng.moveToCup( testMove1 , QueInf) ;
    
    auto task = *taskspace ;

    task.taskFunc( task.arg) ;  // dataRelayTask 

}