
#include "Sorter.hpp"

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



                        
                        
