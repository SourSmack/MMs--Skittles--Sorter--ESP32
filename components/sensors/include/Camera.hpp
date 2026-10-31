#pragma once

#include "driver/uart.h"
#include "freertos/idf_additions.h"
#include <cstdint>
#include <etl/string.h>



#define COLORSENSOR_WORD_SIZE 20 

class Uart{
public:

}

class Camera  
{
private:
    
     /*{.baud_rate  = 115200 , 
        .data_bits  = UART_DATA_8_PINS  , 
        .parity =  UART_PARITY_DISABLE ,
        .stop_bits = UART_STOP_BITS_1, 
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE , 
        .source_clk = UART_SCLK_APB   */ 
     
    pc_uart_config_t cameraUart ; 
    uart_port_t  uartNum ;  
    int buffsize ; 
    int rxPin  ;
    int txPin ;
    int queSize ;
    QueueHandle_t queHandle { nullptr }; 

    EventGroup eventGroup ;
    EventBits_t  colorSensorBit ; 

    FREETask task ;

    etl::string< COLORSENSOR_WORD_SIZE > last_word ; 


    static void uartTask(void *arg)
    {   
        auto instance = *static_cast< Camera* > arg ;

        int8_t dtmp[64];
        uart_event_t  event;  
        
        

        while ( !FREETask::waitForNotify( FREETask::stopBit , 0 )) 
        {
            while ( FREETask::waitForNotify( FREETask::pauseBit , FREETask::MAX_DELAY )){}

            if ( ! xQueueReceive( queHandle,   &event, portMAX_DELAY ))
                continue;

            if ( event.type != UART_PATTERN_DET){
                uart_flush_input(uartNum);
                continue;
            }

            int pos = uart_pattern_pop_pos(uartNum);
            if (pos == -1)
                continue ;
            
            if (pos >= sizeof(dtmp)){
                uart_flush(uartNum);
                continue;
            }
            
            int readLen = uart_read_bytes(uartNum,  dtmp,  pos + 1 ,  pdMS_TO_TICKS(100));
            dtmp[readLen] = '\0';

            etl::string<32> tmpString{(char*)dtmp};
            last_word = tmpString ;

            eventGroup.setBits( colorSensorBit  );
        }

         
        
    }
    
    
    Camera(int buffsize, int queSize);
    //int init(pc_uart_config_t &uartCfg ,uart_port_t  uartNum,  uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb  );
public:




    bool uartConfiguration( pc_uart_config_t uartConfig , uart_port_t  uartNum ,   int buffsize ,  int rxPin  , int txPin , int queSize , QueueHandle_t queHandle , EventGroup &p_eventGroup , FREETask &task){

        if ( uartCfg.baud_rate == 0  ) 
        {
            
            uartCfg.parity = UART_PARITY_DISABLE;
            uartCfg.data_bits = UART_DATA_8_BITS ;
            uartCfg.baud_rate = 115200 ;
            uartCfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            uartCfg.source_clk = UART_SCLK_DEFAULT ; 
            uartCfg.stop_bits = UART_STOP_BITS_1 ; 
        }
        uartNum = uartNm ;
        cameraUart = uartCfg ;

        eventGroup = eg ; 
        colorSensorBit = eb ;

        if ( !queHandle )
            queHandle = xQueueCreate( queSize, sizeof(uart_event_t) );
        if (queHandle == nullptr) return false ; 




        uart_param_config( uartNum,   &cameraUart); 

        uart_set_pin(uartNum , txPin , rxPin ,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);


        uart_driver_install(uartNum,  rxBuffSize, 0,  queSize ,  &queHandle,    0 );

            
        //xTaskCreate( uart_task_wrapper ,  "uartTask" ,   4096,   this,  5, &task)) return ESP_FAIL ; 


        
        uart_driver_install( uartNum , buffsize, , 10, &queHandle, 0 );

        uart_param_config( uartNum , &uartConfig);

        uart_set_pin( uartNum , txPin, rxPin, UART_PIN_NO_CHANGE , UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        uart_enable_pattern_det_baud_intr(uartNum, '\n', 1, 9, 0, 0);

        uart_pattern_queue_reset((uartNum ),  queSize)
    }

    Camera( pc_uart_config_t uartConfig , uart_port_t  uartNum ,   int buffsize ,  int rxPin  , int txPin , int queSize , QueueHandle_t queHandle , EventGroup &p_eventGroup , FREETask &task) : eventGroup( eg ) , colorSensorBit( eb ) , queSize( queSize ) ,  

    static etl::EventFlags< Camera > create( int buffsize, int queSize , pc_uart_config_t &uartCfg ,uart_port_t  uartNum,  uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb  );
    

    bool stopListeningIT();
    bool listenIT();
    bool turnOn();
    bool turnOff();
    int getSample()const ;



};