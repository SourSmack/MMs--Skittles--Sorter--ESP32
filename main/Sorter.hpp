#pragma once

#include <cstdint>
#include <algorithm>





#include "etl/optional.h"

#include "esp_log.h"

#include "ConceptsConfig.hpp"

#define DRIVER_I2S_DIRECT FasDriver::I2S_DIRECT

#define RX_BUFF_SIZE 1024

#define TOP_STEP_PIN int8_t(GPIO_NUM_32)
#define TOP_DIR_PIN GPIO_NUM_33 

#define BOT_STEP_PIN GPIO_NUM_25 
#define BOT_DIR_PIN GPIO_NUM_26

#define PHOTO_PIN GPIO_NUM_27 
#define EMITTER_PIN GPIO_NUM_14 

#define BIT_COLORSENSOR_INPUT BIT0
#define BIT_TRANSOPTOR_INPUT BIT1
#define BIT_SLIDE_ENGINE_FINISHED BIT2 
#define BIT_DISK_ENGINE_FINISHED BIT3 

#define UART0_TX GPIO_NUM_1
#define UART0_RX GPIO_NUM_3

#define COLORSENSOR_RX_BUFF_SIZE 64 
#define COLORSENSOR_TX_BUFF_SIZE 64 
#define COLORSENSOR_WORD_SIZE 20 

#define NEMA_FREQ 1000000 
#define u32 uint32_t 

#define RED_CUP_DEGREE_POS 30 
#define ORANGE_CUP_DEGREE_POS 30 
#define YELLOW_CUP_DEGREE_POS 30 
#define PURPLE_CUP_DEGREE_POS 30 
#define GREEN_CUP_DEGREE_POS 30 
#define UNKNOWN_CUP_DEGREE_POS 30 

#define makeIsr  true 
#define enqueMove true 

namespace cup {
    constexpr moveBlock_t RED{ RED_CUP_DEGREE_POS  } ; 
    constexpr moveBlock_t ORANGE{ ORANGE_CUP_DEGREE_POS  } ;
    constexpr moveBlock_t YELLOW{  YELLOW_CUP_DEGREE_POS } ;
    constexpr moveBlock_t PURPLE{ PURPLE_CUP_DEGREE_POS } ;
    constexpr moveBlock_t GREEN{ GREEN_CUP_DEGREE_POS  } ; 
    constexpr moveBlock_t UNKNOWN{ UNKNOWN_CUP_DEGREE_POS }; 

}

constexpr moveBlock_t flushCandy{ 2 }; 
constexpr moveBlock_t fetchCandy{ 1 }; 
constexpr moveBlock_t spinForever{ 0 } ;

enum  
{
    RED,
    ORANGE,
    YELLOW,
    PURPLE,
    GREEN,
    UNKNOWN
};

// create error codes that clears which&why peripherals malfuntion
enum class sorterStatus{
    OK , 
    busy , 
    ERRORslideEngine,
    ERRORdisksEngine,
    ERRORslideSensor,
    ERRORdisksSensor,
    ERRORhomingDisks,
    ERRORhomingSlide

};

template < 
    EventFlagsConcept EventFlagsType 
    TaskConcept  TaskType  ,

    EngineConcept SlideEngineType , 
    SensorConcept SlideSensorType , 

    EngineConcept DisksEngineType, 
    SensorConcept DisksSensorType, 

> 
class Sorter{

public:
    Sorter() = delete ;
    Sorter( EventFlagsType  &p_eventGroup , 
            TaskType &p_sortingTask  , 
            SlideEngineType &p_slideEngine , 
            SlideSensorType &p_slidePositionSensor , 
            DisksEngineType &p_disksEngine , 
            DisksSensorType &p_colorSensor  ); 


    void startSorting() ; 

    sorterStatus getStatus()const;

    sorterStatus stopSorting() ; 

private:

    EventFlagsType  &eventGroup ; 

    TaskType &sortingTask  ;

    SlideEngineType &slideEngine;
    SlideSensorType &slidePositionSensor;

    DisksEngineType &disksEngine;
    DisksSensorType &colorSensor ; 
    


    sorterStatus status{ sorterStatus::OK }    ; 
    

    static void _sortingFunction(void *pvParameter) ;

    sorterStatus homingSlide();
    sorterStatus homingDisks(); 

    template< class T > 
    friend  bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< T > &sorter );
};