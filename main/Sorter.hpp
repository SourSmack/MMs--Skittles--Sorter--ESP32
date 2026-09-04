#pragma once

#include <cstdint>
#include <algorithm>





#include "etl/optional.h"
#include "etl/string.h"

#include "FreeRTOSWrapper.hpp"


#include "ConceptsConfig.hpp"

//#define DRIVER_I2S_DIRECT FasDriver::I2S_DIRECT

#define RX_BUFF_SIZE 1024

#define BIT_COLORSENSOR_INPUT 1 << 0  
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

struct UserHardwareConfiguration ; 
inline int colorToNum( const etl::string< COLORSENSOR_WORD_SIZE > &color );

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
                                                colorSensor( p_colorSensor )
                                                {} 




    void startSorting() {
        sortingTask.start() ; 
    } 

    sorterStatus getStatus()const{ return status ; }

    sorterStatus stopSorting(){ 
        sortingTask.stop() ; 
        return sorterStatus::OK ;
    }

private:

    EventFlagsType  &eventGroup ; 

    TaskType &sortingTask  ;

    SlideEngineType &slideEngine;
    SlideSensorType &slidePositionSensor;

    DisksEngineType &disksEngine;
    DisksSensorType &colorSensor ; 
    


    sorterStatus status{ sorterStatus::OK }    ; 
    

    static void _sortingFunction(void *pvParameter){
        auto& pair = *static_cast< 
                                    etl::pair<Sorter* , uint16_t > * 
                                                                    >
                                                                        ( pvParameter ) ;
        auto& [ instance , token ] = pair ;
        auto& [ eventGroup , sortingTask , slideEngine , slidePositionSensor , disksEngine , colorSensor , status  ] = instance ;

        if ( status != sorterStatus::OK ) return ;

        if ( instance.homingSlide() != sorterStatus::OK){
            status = sorterStatus::ERRORslideEngine ; 
            return ;
        } 
        if ( instance.homingDisks() != sorterStatus::OK ) {
            status = sorterStatus::ERRORdisksEngine; 
            return ; 
        }


        while ( ! FREETask::stopRequested( token ) ){
            disksEngine.move( fetchCandy ) ;

            const auto& sample = *static_cast< const etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ; 
            auto candyColor = colorToNum( sample ) ;

            switch ( candyColor ){
                case   RED  : 
                    slideEngine.moveToCup( cup::RED ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   ORANGE  :
                    slideEngine.moveToCup( cup::ORANGE ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   YELLOW  :
                    slideEngine.moveToCup( cup::YELLOW ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   PURPLE  :
                    slideEngine.moveToCup( cup::PURPLE ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                case   GREEN  : 
                    slideEngine.moveToCup( cup::GREEN ) ; 
                    disksEngine.move( flushCandy  );
                    break;

                default :
                    slideEngine.moveToCup( cup::UNKNOWN ) ; 
                    disksEngine.move( flushCandy  );
                    break ;
            }


        }


    }

    sorterStatus homingSlide(){
        auto sample = * static_cast < etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ;
        auto chamberColor = colorToNum( sample ) ;

        colorSensor.listenIT() ; 

        disksEngine.move( spinForever   ); 

        while ( !eventGroup.bitsWait(BIT_COLORSENSOR_INPUT , portMAX_DELAY )) {}
        

        disksEngine.stop( ) ;

        colorSensor.stopListeningIT() ;

        return sorterStatus::OK ;

    }

    sorterStatus homingDisks(){

        slidePositionSensor.listenIT();
        slideEngine.move( spinForever ) ;

        while ( !eventGroup.bitsWait( BIT_TRANSOPTOR_INPUT,   portMAX_DELAY )) {} 
        
        slideEngine.stop( ) ;
        slidePositionSensor.stopListeningIT() ;



        return sorterStatus::OK ; 

    }

    template< class T > 
    friend  bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< T > &sorter );
};




inline int colorToNum( const etl::string< COLORSENSOR_WORD_SIZE > &color ){
    if ( color == "RED") return 0; 
    else if ( color ==  "ORANGE" ) return 1 ;
    else if ( color ==  "YELLOW") return 2 ;
    else if ( color == "PURPLE") return 3 ;
    else if ( color == "GREEN") return 4 ; 
    else 
        return -1 ;
} 



