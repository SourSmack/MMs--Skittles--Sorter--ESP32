#pragma once
#include "ISensor.hpp"
#include "driver/uart.h"
#include "freertos/idf_additions.h"
#include <cstdint>
#include <etl/string.h>



#define COLORSENSOR_WORD_SIZE 20 

class Camera : public ISensor 
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

    int rxBuffSize ; 

    EventGroupHandle_t eventGroup ;
    EventBits_t  colorSensorBit ; 

    int queSize ;
    QueueHandle_t queHandle ; 

    etl::string< COLORSENSOR_WORD_SIZE > last_word ; 

    TaskHandle_t task ;

    static void uart_task_wrapper(void *arg);
    void uart_task_loop();
    
    Camera(int rxBuffSize, int queSize);
    int init(pc_uart_config_t &uartCfg ,uart_port_t  uartNum,  uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb  );
public:
    Camera() = delete ;
    static etl::optional< Camera > create( int rxBuffSize, int queSize , pc_uart_config_t &uartCfg ,uart_port_t  uartNum,  uint16_t txPin , uint16_t rxPin , EventGroupHandle_t &eg, EventBits_t eb  );
    

    bool stopListening_IT()override;
    bool listen_IT()override;
    bool turnOn()override;
    bool turnOff()override;
    const void* getSample()const override;



};