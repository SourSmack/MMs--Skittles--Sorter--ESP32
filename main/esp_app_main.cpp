
#include "Sorter.hpp"
#include "FreeRTOSWrapper.hpp"
#include "Nema.hpp"
#include "ScurvePlanner.hpp"
#include "FastAccelWrapper.hpp"
#include "EventGroupIsrTransoptor.hpp"
#include "EventFlags.hpp"

#define TOP_STEP_PIN int8_t(GPIO_NUM_32)
#define TOP_DIR_PIN GPIO_NUM_33 

#define BOT_STEP_PIN GPIO_NUM_25 
#define BOT_DIR_PIN GPIO_NUM_26

#define PHOTO_PIN GPIO_NUM_27 
#define EMITTER_PIN GPIO_NUM_14 

#define UART0_TX GPIO_NUM_1
#define UART0_RX GPIO_NUM_3


using genericSorter = Sorter< 
                        Nema< ScurvePlanner , FREETask > ,
                        Nema< ScurvePlanner , FREETask > , 
                        Camera , 
                        EventGroupIsrTransoptor  , 
                        EventGroup < EventGroupHandle_t , EventBits_t  >
                        >;

bool peripheralsCreation( UserHardwareConfiguration &peripherals , etl::optional< genericSorter > &sorter );
 
extern "C" void app_main(void)
{
    
    UserHardwareConfiguration peripherals ;
    etl::optional< genericSorter > opt_sorter ;

    if ( !peripheralsCreation( peripherals , opt_sorter ) ){
        ESP_LOGE("CONFIG", "Peripherals configuration accured error\n", ESP_FAIL);
        return; 
    }
    auto &sorter  = *opt_sorter ;

    sorter.startSorting() ;
    auto status{ sorterStatus::OK } ;
    while (true){ 

        status = sorter.getStatus() ;
        if ( status == sorterStatus::OK  || status == sorterStatus::busy ){
            
            continue ; 
        }

        ESP_LOGE("ERROR", "Peripherals error accured: %d\n", status );
        sorter.stopSorting() ;
        /*
         whole device reset routine 
         either hand reset or automatic routine for reset 
         */ 
        break ; 
    }



    while (true){
        //  MAIN LOOP SOMEHOW ESCAPED 

        ESP_LOGE("APP_MAIN", "Program somehow run away ", ESP_FAIL);
        vTaskDelay( pdMS_TO_TICKS(1000));
    }

}



                        
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



                    
