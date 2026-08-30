#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <etl/optional.h>
#include "ISensor.hpp"
#include "driver/gpio.h"


class EventGroupIsrTransoptor : public ISensor 
{
private:
    uint32_t phototransistorPin;
    uint32_t emitterPin;
    
    gpio_config_t photoGpio;
    gpio_config_t emitGpio;
    
    bool isBlocked;


   EventGroupHandle_t eventGroup    ; 
   EventBits_t colorSensorBit     ; 

   EventGroupIsrTransoptor( uint32_t photoPin , uint32_t emitPin );


   int init(EventGroupHandle_t &evtGroup , EventBits_t evtBits);

   static void isrHandler( void* arg ) ; 
public: 
    EventGroupIsrTransoptor() = delete ;
    
    static etl::EventFlags< EventGroupIsrTransoptor > create(uint32_t photoPin , uint32_t emitPin , EventGroupHandle_t &evtGrp , EventBits_t evtBits ); 
    

    bool listenIT() override ;
    
    const void* getSample()const override  ;
    bool stopListeningIT() override  ;

    bool turnOn()override  ;
    bool turnOff()override  ;
    


   
};