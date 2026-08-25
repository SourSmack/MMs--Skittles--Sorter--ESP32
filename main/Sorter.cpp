/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "Sorter.hpp"
#include "FreeRtosWrapper.hpp"

class Task;

struct HardwareContext 
{
    etl::optional< EventGroupIsrTransoptor >  slideSensor ;
    etl::optional< Nema< ScurvePlanner , Task >>  slideEngine ; 
    FastAccelStepperEngine fastSlideEngine; 

    etl::optional < Camera > disksSensor ;
    FastAccelStepperEngine fastDisksEngine; 
    etl::optional< Nema< ScurvePlanner , FreeRtosWrapperL>>  disksEngine ; 
 
    etl::optional< EventFlags_t<EventGroupHandle_t , EventBits_t ,16 >>   eventGroup;
    pc_uart_config_t uart_cfg ;  
    ScurvePlanner planner ;
};


using genericSorter = Sorter< 
                        Nema< ScurvePlanner , Task > ,
                        Nema< ScurvePlanner , Task > , 
                        Camera , 
                        EventGroupIsrTransoptor  , 
                        EventGroupHandle_t , 
                        EventBits_t  ,
                        16 > ;

                        
bool config( HardwareContext &peripherals , etl::optional< genericSorter < > &sorter );


etl::optional< MemberType > EventFlags_t::operator[](uint32_t idx){
    if ( idx  > commandsNum ) return etl::nullopt ;

    return pEventGropu.*(memberMap[idx]);
}
;

extern "C" void app_main(void)
{
    HardwareContext peripherals ;
    etl::optional< genericSorter > opt_sorter ;

    if ( !config( peripherals , opt_sorter ) ){
        ESP_LOGE("CONFIG", "Peripherals configuration accured error\n", ESP_FAIL);
        return; 
    }
    auto &sorter  = *opt_sorter ;

    TaskHandle_t sortingTcb = nullptr; 
    xTaskCreate(  sorter.startSorting , "sorting()" , 2048  , &sorter  , 4, &sortingTcb) ;

    auto status{ sorterStatus::OK } ;
    while (true){ 

        status = sorter.getStatus() ;
        if ( status == sorterStatus::OK  || status == sorterStatus::busy ){
            taskYIELD() ;
            continue ; 
        }

        ESP_LOGE("ERROR", "Peripherals error accured: %d\n", status );
        sorter.stopSorting() ;

    }



    while (true){
        ESP_LOGE("APP_MAIN", "Program somehow run away ", ESP_FAIL);
        vTaskDelay( pdMS_TO_TICKS(1000));
    }

}

bool config( HardwareContext &peripherals , etl::optional< genericSorter > &sorter )
{
    auto& [ slideSensor , slideEngine , fastSlideEngine ,disksSensor , fastDisksEngine , disksEngine , eventGroup ,uartCfg , planner] = peripherals ; 

    if (  eventGroup  = xEventGroupCreate() ; eventGroup == nullptr )  
        return false ;
    
    if ( slideSensor = EventGroupIsrTransoptor::create( PHOTO_PIN, EMITTER_PIN , eventGroup , BIT_TRANSOPTOR_INPUT   ); slideSensor == etl::nullopt )
        return false ;
    if ( slideEngine = Nema::create( BOT_STEP_PIN , BOT_DIR_PIN , planner , fastSlideEngine ) ; slideEngine == etl::nullopt )
        return false ;




    uartCfg.baud_rate = 115200 ;
    uartCfg.data_bits = UART_DATA_8_BITS ;
    uartCfg.parity = UART_PARITY_DISABLE ;
    uartCfg.source_clk = UART_SCLK_DEFAULT ;
    uartCfg.stop_bits = UART_STOP_BITS_1 ;
    uartCfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE ;
    uartCfg.rx_flow_ctrl_thresh = 0 ;

    disksSensor = Camera::create(COLORSENSOR_RX_BUFF_SIZE , COLORSENSOR_TX_BUFF_SIZE ,  uartCfg , UART_NUM_0 , UART0_TX , UART0_RX ,  eventGroup, BIT_COLORSENSOR_INPUT); 
    if ( disksSensor == etl::nullopt ) return false ;
    disksEngine = Nema::create( TOP_STEP_PIN , TOP_DIR_PIN , planner, fastDisksEngine ) ;
    if ( disksEngine == etl::nullopt ) return false ; 
 
    xEventGroupSetBits ( eventGroup ,   BIT_COLORSENSOR_INPUT   |         
                                        BIT_TRANSOPTOR_INPUT  |
                                        BIT_SLIDE_ENGINE_FINISHED  |
                                        BIT_DISK_ENGINE_FINISHED ); 

    sorter.emplace( eventGroup , *slideEngine , *disksEngine , *disksSensor , *slideSensor );
    if ( sorter == etl::nullopt ) return false ;

    return true ;
}

void Sorter::startSorting(void *pvParameter) 
{
    static bool isRunning { false } ; 
    if (isRunning) return ;

    isRunning = true;

    auto* sorter = static_cast< Sorter* >( pvParameter ) ; 

    sorter->_startSorting() ; 

    isRunning = false ;

    vTaskDelete(nullptr);
}

void Sorter::_startSorting() {
    
    if ( status != sorterStatus::OK ) return ;

    if ( homingSlide() != sorterStatus::OK){
        status = sorterStatus::ERRORslideEngine ; 
        return ;
    } 
    if ( homingDisks() != sorterStatus::OK ) {
        status = sorterStatus::ERRORdisksEngine; 
        return ; 
    }


    while (shouldSort){
        disksEngine.move( fetchCandy ) ;

        auto sample = *static_cast< const etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ; 
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

void Sorter::stopSorting(){
    if ( getStatus() == sorterStatus::OK ) return ;
    
    shouldSort = false ;
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



Sorter(EventGroupHandle_t &eg,  IEngine &se , IEngine &de , ISensor &cs  , ISensor &sp) : eventGroup(eg) , slideEngine(se) , disksEngine(de) , colorSensor(cs) , slidePositionSensor(sp) {}


sorterStatus homingSlide(){
    auto sample = * static_cast < etl::string< COLORSENSOR_WORD_SIZE > * >( colorSensor.getSample() ) ;
    auto chamberColor = colorToNum( sample ) ;

    colorSensor.listen_IT() ; 

    disksEngine.move( spinForever   ); 

    auto bits = xEventGroupWaitBits( &eventGroup , BIT_COLORSENSOR_INPUT , portMAX_DELAY);   

    disksEngine.stop( flush = true , instantly = true ) ;

    colorSensor.stopListening_IT() ;

    if ( !( bits & ~BIT_COLORSENSOR_INPUT )) return sorterStatus::ERRORhomingSlide ; 

    return sorterStatus::OK ;

}

sorterStatus homingDisks(){

    slidePositionSensor.listen_IT();
    slideEngine.move( spinForever ) ;
    auto bits = xEventGroupWaitBits( eventGroup, BIT_TRANSOPTOR_INPUT,   portMAX_DELAY);
    slideEngine.stop( flush = true , instantly = true ) ;
    slidePositionSensor.stopListening_IT() ;

    if ( !( bits & !BIT_TRANSOPTOR_INPUT)) return sorterStatus::ERRORhomingSlide;

    return sorterStatus::OK ; 

} 