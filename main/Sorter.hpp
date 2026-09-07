#pragma once

#include <cstdint>
#include <algorithm>





#include "etl/optional.h"
#include "etl/string.h"

#include "FreeRTOSWrapper.hpp"


#include "ConceptsConfig.hpp"

//#define DRIVER_I2S_DIRECT FasDriver::I2S_DIRECT

#define RX_BUFF_SIZE 1024

#define BIT_DISKSSENSOR_INPUT 1 << 0  
#define BIT_TRANSOPTOR_INPUT 1 << 1  
#define BIT_SLIDE_ENGINE_FINISHED 1 << 2  
#define BIT_DISK_ENGINE_FINISHED 1 << 3 


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

constexpr const moveBlock_t cupsMoves[] =  {
    moveBlock_t { RED_CUP_DEGREE_POS  } , 
    moveBlock_t { ORANGE_CUP_DEGREE_POS  } ,
    moveBlock_t {  YELLOW_CUP_DEGREE_POS } ,
    moveBlock_t { PURPLE_CUP_DEGREE_POS } ,
    moveBlock_t { GREEN_CUP_DEGREE_POS  } , 
    moveBlock_t { UNKNOWN_CUP_DEGREE_POS } 

};

constexpr moveBlock_t flushCandy{ 2 }; 
constexpr moveBlock_t fetchCandy{ 1 }; 
constexpr moveBlock_t spinForever{ 0 } ;

/*enum  
{
    RED,
    ORANGE,
    YELLOW,
    PURPLE,
    GREEN,
    UNKNOWN
};*/

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

struct UserHardwareConfiguration ; 

template < 
    EventGroupConcept EventFlagsType ,
    TaskConcept  TaskType  ,

    EngineConcept SlideEngineType , 
    SensorConcept SlideSensorType , 

    EngineConcept DisksEngineType, 
    SensorConcept DisksSensorType 

> 
class Sorter{

public:
    Sorter() = delete ;


    Sorter( EventFlagsType  &p_eventGroup , 
            TaskType &p_sortingTask  , 
            SlideEngineType &p_slideEngine , 
            SlideSensorType &p_slidePositionSensor , 
            DisksEngineType &p_disksEngine , 
            DisksSensorType &p_colorSensor  ) : eventGroup( p_eventGroup ) , 
                                                sortingTask( p_sortingTask )  , 
                                                slideEngine( p_slideEngine ) , 
                                                slidePositionSensor( p_slidePositionSensor) , 
                                                disksEngine( p_disksEngine ) ,
                                                disksSensor( p_colorSensor )
                                                {} 




    void startSorting() {
        sortingTask.start() ; 
    } 

    sorterStatus getStatus()const{ return status ; }

    sorterStatus stopSorting(){ 
        sortingTask.stop() ; 
        return sorterStatus::OK ;
    }

    sorterStatus sortSingleCandy( /*moveBlock_t cupAddress*/ ){

        disksEngine.move( fetchCandy  );
        const auto candyColorIdx = disksSensor.getSample()  ; 

        slideEngine.moveToCup( cupsMoves[ candyColorIdx ] ) ; 
        disksEngine.move( flushCandy  );

        return sorterStatus::OK ;
    }

    sorterStatus homingDisks(){

        disksSensor.listenIT() ; 

        disksEngine.move( spinForever   ); 
        

        auto i{4} ;
        while ( --i && !eventGroup.bitsWait( BIT_DISKSSENSOR_INPUT , EventFlagsType::MAX_DELAY )){}
        if ( !i )  return sorterStatus::ERRORhomingDisks ;
        

       

        disksEngine.stop( ) ;

        disksSensor.stopListeningIT() ;

        return sorterStatus::OK ;

    }

    sorterStatus homingSlide(){

        slidePositionSensor.listenIT();
        slideEngine.move( spinForever ) ;

        auto i{4} ;
        while ( --i && !eventGroup.bitsWait( BIT_TRANSOPTOR_INPUT , EventFlagsType::MAX_DELAY )){}
        if ( !i )  return sorterStatus::ERRORhomingSlide ;
        

        
        slideEngine.stop( ) ;
        slidePositionSensor.stopListeningIT() ;



        return sorterStatus::OK ; 

    }


private:

    EventFlagsType  &eventGroup ; 

    TaskType &sortingTask  ;

    SlideEngineType &slideEngine;
    SlideSensorType &slidePositionSensor;

    DisksEngineType &disksEngine;
    DisksSensorType &disksSensor ; 
    


    sorterStatus status{ sorterStatus::OK }    ; 
    

    static void _sortingFunction(void *pvParameter){
        auto& pair = *static_cast< etl::pair<Sorter* , uint16_t > * >( pvParameter ) ;
        auto& [ instance , token ] = pair ;
        auto& [ eventGroup , sortingTask , slideEngine , slidePositionSensor , disksEngine , disksSensor , status  ] = instance ;

        static bool isHoomed{ false } ;

        if ( !isHoomed){
            if ( instance.homingSlide() != sorterStatus::OK){
                status = sorterStatus::ERRORslideEngine ; 
                return ;
                } 
            if ( instance.homingDisks() != sorterStatus::OK ) {
                status = sorterStatus::ERRORdisksEngine; 
                return ; 
                }
            isHoomed = true; 
        }

        if ( status != sorterStatus::OK ) return ;


        while ( ! FREETask::stopRequested( token ) ){
            instance.sortSingleCandy() ;
        }


    }
    template< class T > 
    friend  bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< T > &sorter );
};







