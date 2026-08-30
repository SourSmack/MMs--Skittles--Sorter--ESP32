/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "Sorter.hpp"
#include "FreeRTOSWrapper.hpp"


struct UserHardwareConfiguration 
{

    etl::optional< EventGroup<EventGroupHandle_t , EventBits_t >>   eventFlags;
    
    etl::optional< FREETask > sortingTask ; 

    etl::optional< SensorConcept EventGroupIsrTransoptor >  slideSensor ;
    etl::optional< Nema< PlannerConcept Planner , StepperConcept Stepper , TaskConcept Task >>  slideEngine ; 
    etl::optional< FastAccelWrapper > slideStepper; 
    etl::optional< ScurvePlanner > slidePlanner ; 

    etl::optional < Camera > disksSensor ;
    etl::optional< Nema< PlannerConcept Planner , StepperConcept Stepper , TaskConcept Task >>  disksEngine ; 
    etl::optional< FastAccelWrapper > disksStepper; 
    etl::optional< ScurvePlanner > disksPlanner ;
 
};

bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< genericSorter > &sorter )
{
    auto& [ eventFlags ,
            sortingTask , 
            slideSensor , slideEngine , slideStepper , slidePlanner , 
            disksSensor ,  disksEngine , disksStepper  , disksPlanner ,  ] = peripherals ; 
            
    using EventFlagsType   = decltype( eventFlags ) ;
    using TaskType         = decltype( sortingTask ) ;

    using SlideSensorType  = decltype( slideSensor ) ;
    using SlideEngineType  = decltype( slideEngine ) ; 
    using SlideStepperType = decltype( slideStepper ) ;
    using SlidePlannerType = decltype( slidePlanner ) ;

    using DisksSensorType  = decltype( disksSensor ) ;
    using DisksEngineType  = decltype( disksEngine ) ;
    using DisksStepperType = decltype( disksStepper ) ;
    using DisksPlannerType = decltype( disksPlanner ) ;



    if (  eventFlags  = EventFlagsType::create( BIT_COLORSENSOR_INPUT   , BIT_TRANSOPTOR_INPUT , BIT_SLIDE_ENGINE_FINISHED  , BIT_DISK_ENGINE_FINISHED) ; 
        eventFlags == etl::nullopt )  
        return false ;

    if ( sortingTask = TaskType::create( _sortingFunction  , &sorter , 2024 , 4  ) ; 
        sortingTask == etl::nullopt) 
        return false ; 

    
    if ( slideSensor = SlideSensorType::create( PHOTO_PIN, EMITTER_PIN , eventFlags , BIT_TRANSOPTOR_INPUT   ); 
        slideSensor == etl::nullopt )
        return false ;
    if ( slideEngine = SlideEngineType::create( BOT_STEP_PIN , BOT_DIR_PIN , slidePlanner , slideStepper ) ; 
        slideEngine == etl::nullopt )
        return false ;





    disksSensor = DisksSensorType::create( UART0_TX , UART0_RX  , eventFlags , BIT_COLORSENSOR_INPUT); 
    if ( disksSensor == etl::nullopt ) return false ;
    disksEngine = DisksEngineType::create( TOP_STEP_PIN , TOP_DIR_PIN , disksPlanner, disksStepper ) ;
    if ( disksEngine == etl::nullopt ) return false ; 
 
    

    sorter.emplace( *eventFlags , *sortingTask ,  *slideEngine , *slideSensor , *disksEngine , *disksSensor   );
    if ( sorter == etl::nullopt ) return false ;

    return true ;
}






Sorter( EventFlagsType < eventGroupType , memberType , commandsNum > &p_eventGroup , 
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





void Sorter::_sortingFunction(void *pvParameter) 
{
    auto& pair = *static_cast< 
                                etl::pair<Sorter* , uint16t > * 
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

void Sorter::startSorting() {
    
   sortingTask.start() ; 
   /* 
    static bool isRunning { false } ; 
    if (isRunning) return ;

    isRunning = true;

    auto* sorter = static_cast< Sorter* >( pvParameter ) ; 

    sorter->startSorting() ; 

    isRunning = false ;

    vTaskDelete(nullptr);
*/





}

void Sorter::stopSorting(){
    sortingTask.stop() ; 
    
    
}


int colorToNum( etl::string< COLORSENSOR_WORD_SIZE > &color ){
    if ( color == "RED") return 0; 
    else if ( color ==  "ORANGE" ) return 1 ;
    else if ( color ==  "YELLOW") return 2 ;
    else if ( color == "PURPLE") return 3 ;
    else if ( color == "GREEN") return 4 ; 
    else 
        return -1 ;
} 

 



sorterStatus Sorter::getStatus()const{ return status ; }





sorterStatus homingSlide(){
    auto sample = * static_cast < etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ;
    auto chamberColor = colorToNum( sample ) ;

    colorSensor.listenIT() ; 

    disksEngine.move( spinForever   ); 

    while ( !eventGroup.bitsWait(BIT_COLORSENSOR_INPUT , portMAX_DELAY )) {}
    

    disksEngine.stop( flush = true , instantly = true ) ;

    colorSensor.stopListeningIT() ;

    return sorterStatus::OK ;

}

sorterStatus homingDisks(){

    slidePositionSensor.listenIT();
    slideEngine.move( spinForever ) ;

    while ( !eventGroup.bitsWait( BIT_TRANSOPTOR_INPUT,   portMAX_DELAY )) {} 
    
    slideEngine.stop( flush = true , instantly = true ) ;
    slidePositionSensor.stopListeningIT() ;



    return sorterStatus::OK ; 

} 