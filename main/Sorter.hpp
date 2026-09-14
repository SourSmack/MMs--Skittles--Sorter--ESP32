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
enum class  ErrCode : uint16_t {
    
    bool OK  = 1 << 0 ;
    bool busy = 1 << 1 ;
    bool slideEngine = 1 << 2 ;
    bool disksEngine = 1 << 3 ;
    bool slideSensor = 1 << 4 ;
    bool disksSensor = 1 << 5 ;
    bool homingDisks = 1 << 6 ;
    bool homingSlide = 1 << 7 ;
    bool eventGroup  = 1 << 8 ;

    friend sorterErrFlags operator|( const  sorterErrFlags a  , const sorterErrFlags b  ){
        return  static_cast< sorterErrFlags >( 
            static_cast< uint16_t>( a ) | static_cast< uint16_t( b ) ) ;
    }
};

union sorterErrFlags{
    struct{
        bool OK  : 1 = false ;
        bool busy : 1 = false ;
        bool ERRORslideEngine : 1 = false ;
        bool ERRORdisksEngine : 1 = false ;
        bool ERRORslideSensor : 1 = false ;
        bool ERRORdisksSensor : 1 = false ;
        bool ERRORhomingDisks : 1 = false ;
        bool ERRORhomingSlide : 1 = false ;
    };

    ErrCode rawMask ;

    constexpr sorterErrFlags(): rawMask( ErrCode::OK ){}
    // implicit conversion 
    constexpr sorterErrFlags( ErrCode err ): rawMask( err ){} 

    constexpr operator bool(){
        return rawMask != ErrCode::OK ;
    }
}

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

    sorterErrFlags getStatus()const{ return status ; }

    sorterErrFlags stopSorting(){ 
        sortingTask.stop() ; 
        return ErrCode::OK ;
    }

    sorterErrFlags sortSingleCandy( /*moveBlock_t cupAddress*/ ){

        disksEngine.move( fetchCandy  );
        const auto candyColorIdx = disksSensor.getSample()  ; 

        slideEngine.moveToCup( cupsMoves[ candyColorIdx ] ) ; 
        disksEngine.move( flushCandy  );

        return ErrCode::OK ;
    }

    sorterErrFlags homingDisks(){


        if ( !disksSensor.listenIT() ) return ErrCode::disksSensor ;  

        disksEngine.move( spinForever   ); 
        

        auto i{4} ;
        while ( --i && !eventGroup.bitsWait( BIT_DISKSSENSOR_INPUT , EventFlagsType::MAX_DELAY )){}
        if ( !i ){
            if ( !disksEngine.stop() ) 
                return ErrCode::eventGroup | ErrCode::disksEngine ;
            return  ErrCode::eventGroup  ;
        }  
        

       

        if ( !disksEngine.stop( ) ) return ErrCode::disksEngine;

        if ( !disksSensor.stopListeningIT() ) return ErrCode::disksSensor;

        return Errcode::OK ;

    }

    sorterErrFlags homingSlide(){


        if ( !slidePositionSensor.listenIT()) return ErrCode::slideSensor ;
        slideEngine.move( spinForever ) ;

        auto i{4} ;
        while ( --i && !eventGroup.bitsWait( BIT_TRANSOPTOR_INPUT , EventFlagsType::MAX_DELAY )){}
        if ( !i ){
            if ( !slideEngine.stop() ) return ErrCode::eventGroup | ErrCode::slideEngine ;

        }  return ErrCode::eventGroup ;
        

        
        if ( !slideEngine.stop( ) ) return  ErrCode::slideEngine ;
        if ( !slidePositionSensor.stopListeningIT() ) return  ErrCode::slideSensor ;



        return ErrCode::OK ; 

    }


private:

    EventFlagsType  &eventGroup ; 

    TaskType &sortingTask  ;

    SlideEngineType &slideEngine;
    SlideSensorType &slidePositionSensor;

    DisksEngineType &disksEngine;
    DisksSensorType &disksSensor ; 
    


    sorterErrFlags status  ; 
    

    static void _sortingFunction(void *pvParameter){
        auto& pair = *static_cast< etl::pair<Sorter* , uint16_t > * >( pvParameter ) ;
        auto& [ instance , token ] = pair ;
        auto& [ eventGroup , sortingTask , slideEngine , slidePositionSensor , disksEngine , disksSensor , status  ] = instance ;

        static bool isHoomed{ false } ;

        if ( !isHoomed){
            if ( instance.homingSlide() != sorterErrFlags.OK){
                status = sorterErrFlags.slideEngine ; 
                return ;
                } 
            if ( instance.homingDisks() != sorterErrFlags.OK ) {
                status = sorterErrFlags.disksEngine; 
                return ; 
                }
            isHoomed = true; 
        }

        if ( status != sorterErrFlags.OK ) return ;


        while ( ! FREETask::stopRequested( token ) ){
            instance.sortSingleCandy() ;
        }


    }
    template< class T > 
    friend  bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< T > &sorter );
};







