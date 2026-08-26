#include "../include/Nema.hpp"
#include "etl/optional.h"
#include <thread>
#include <chrono>
#include <iostream>

class MOCKplanner{
#define LEFT true 
public:
    
    static etl::optional< MOCKplanner >  create(){  MOCKplanner tmp{} ; return tmp ; }
    motionBlock_t calculateFrequency(const moveBlock_t &move)   ; 
    void stop(){}
    void start(){}
    void enqueue( const moveBlock_t &move ){ }
    motionBlock_t recieve(){ static uint32_t a { 0 } ; static uint32_t b { 0 } ; return motionBlock_t{ ++a , ++b , LEFT } ;}

};

class MOCKstepper {
public:
    static  etl::optional< MOCKstepper >  create( uint16_t step , uint16_t dir ){  MOCKstepper tmp{}; return tmp;}

    void enqueue( motionBlock_t  motion){  std::cout << "STEPPER: Enqueueing motion \n Engine is running in background ... \n"; }  
    
    void stop(bool instant = false  ) { } 
    void start(){ } 
    


};

class MOCKtask{
private: 
    static bool stop  ;
    static bool running  ;
    static uint8_t mess;
    void (*taskFunc)( void*arg ) { nullptr };
    void * arg { nullptr } ;
public:
    static etl::optional< MOCKtask > createTask( void (*task)(void*arg) , void * arg ,  uint32_t stackSize , uint32_t priority ){  
        MOCKtask tmp{} ;

        tmp.taskFunc = task ;
        tmp.arg = arg ;
        return tmp;
    }

    void notify( uint8_t message ){  mess = message ; }
    bool requestStop()  { stop = false ;}
    bool join(){  while ( running){} ; }

    static bool stopRequested(){ return stop ; }
    static bool notifyWait( uint8_t message , uint32_t delay ){  
        auto result{ false } ;
        auto start{ std::chrono::high_resolution_clock::now() }; 
        auto delayChrono{ std::chrono::milliseconds( delay ) } ;
        auto now = start ; 
        while ( ( now  - start ) < delayChrono  ){
            if ( mess == message)  result = true  ; 
            now  = std::chrono::high_resolution_clock::now() ;
        }
        return result ; 
    } 
    static void waitMs( uint32_t ms ){  std::this_thread::sleep_for( std::chrono::milliseconds( ms ) ); } ;

    void _playTask(){  taskFunc( arg ) ;}
};
bool MOCKtask::stop { false } ;
bool MOCKtask::running { false } ;
uint8_t MOCKtask::mess { 0 };


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
    NemaMOCK.moveToCup( testMove , notQueInf ) ;



    // making move via queue ( spoofing dataRelayTask) 

}