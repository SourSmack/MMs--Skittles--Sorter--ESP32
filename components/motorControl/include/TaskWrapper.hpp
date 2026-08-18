#pragma once
#include "etl/optional.h"

class TaskWrapper {
private: 
public:
    virtual ~TaskWrapper() = 0 ;
    virtual etl::optional< TaskWrapper* > create() = 0 ;
    virtual bool kill() = 0  ;
    virtual void notify( uint8_t message ) = 0  ;
    
    
};

namespace thisTask { 
    bool notifyWait( uint8_t message , uint32_t delay );
}