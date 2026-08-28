

#include "Camera.hpp"
#include "freertos/FreeRTOSConfig_arch.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "soc/clk_tree_defs.h"
#include <driver/uart.h>
#include <freertos/task.h>


etl::optional< Camera > Camera::create( int rxBuffSize, int queSize , pc_uart_config_t &uartCfg ,uart_port_t  uartNum,  uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb  )
{
    Camera temp { rxBuffSize , queSize };
    
    if ( temp.init( uartCfg , uartNum , txPin , rxPin , eg , eb ) != ESP_OK ) 
        return etl::nullopt ; 
    
    return temp;
}

Camera::Camera(int rxBuffSize, int queSize):cameraUart{} , uartNum{} ,rxBuffSize(rxBuffSize), queSize(queSize) {} 

void Camera::uart_task_wrapper(void *arg)
{    static_cast<Camera*>(arg)->uart_task_loop() ; } 

void Camera::uart_task_loop()
{
    int8_t dtmp[64];
    uart_event_t  event;  
    
    vTaskSuspend(NULL);

    while (true) 
    {
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

        xEventGroupSetBits( eventGroup,   colorSensorBit);
    }
}

int Camera::init(pc_uart_config_t &uartCfg, uart_port_t  uartNm , uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb )
{
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

    queHandle = xQueueCreate( queSize, sizeof(uart_event_t) );
    if (queHandle == nullptr) return -1 ; 


    esp_err_t err_code{};

    if ((  err_code = uart_param_config( uartNum,   &cameraUart)) 
        != ESP_OK) return err_code ;

    if (( err_code = uart_set_pin(uartNum , txPin , rxPin ,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE))
        != ESP_OK) return err_code; 


    if (( err_code = uart_driver_install(uartNum,  rxBuffSize, 0,  queSize ,  &queHandle,    0 ))
        != ESP_OK) return err_code;

    if (( err_code = uart_enable_pattern_det_baud_intr(uartNum, '\n', 1, 9, 0, 0))
        != ESP_OK) return err_code;

    if (( err_code = uart_pattern_queue_reset((uartNum ),  queSize))
        != ESP_OK) return err_code;
        
    if ( ! xTaskCreate( uart_task_wrapper ,  "uartTask" ,   4096,   this,  5, &task)) return ESP_FAIL ; 

    return ESP_OK ;
}


const void*  Camera::getSample()const  { return static_cast< const void* >( &last_word )   ; } 


bool Camera::stopListeningIT(){
    vTaskSuspend( task);
    return true ;
}

bool Camera::listenIT(){
    vTaskResume( task);
    return true;
}

bool Camera::turnOn(){
    if ( uart_enable_pattern_det_baud_intr(uartNum, '\n', 1, 9, 0, 0) != ESP_OK) return false;
    return true; 
}

bool Camera::turnOff(){
    vTaskSuspend(task);
    if ( uart_disable_pattern_det_intr(uartNum) != ESP_OK) return false ;

    return true; 
}