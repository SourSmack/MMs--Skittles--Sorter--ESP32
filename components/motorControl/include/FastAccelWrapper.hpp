#pragma  once
#include "FastAccelStepperEngine.h"
#include "../../../main/ConceptsConfig.hpp" 
#include "FastAccelStepper.h"
#include <cstdlib>



class FastAccelWrapper   {
private:
    uint16_t step{ 0 };
    uint16_t dir{ 0 };
    bool run{ false } ;

    FastAccelStepper *handle { nullptr };

    FastAccelWrapper() = default ;
    FastAccelWrapper( uint16_t pstep , uint16_t pdir ):step(pstep) , dir(pdir) {} 


public:

    static  etl::optional< FastAccelWrapper >  create( uint16_t step , uint16_t dir );

    void enqueue( motionBlock_t  motion) ; 
    
    void stop(bool instant = false  ) ; 
    void start(); 
    

};


static_assert( StepperConcept< FastAccelWrapper> , "FastAccelWrapper doesn't meet StepperConcept requirments!\n");



