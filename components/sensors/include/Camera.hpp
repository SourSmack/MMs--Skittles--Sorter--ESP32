#pragma once

#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "driver/uart.h"
#include <etl/string.h>
#include <etl/event_flags.h>
#include "SynchEsp.hpp"
#include "EventGroup.hpp"

#define COLORSENSOR_WORD_SIZE 20

class Camera
{
private:
    uart_config_t cameraUart;
    uart_port_t uartNum;
    int buffsize;
    int rxPin;
    int txPin;
    int queSize;
    QueueHandle_t queHandle{nullptr};

    EventGroup<EventGroupHandle_t> eventGroup;
    EventBits_t colorSensorBit;

    FREETask task;

    etl::string<COLORSENSOR_WORD_SIZE> last_word;

    static void uartTask(void *arg)
    {
        auto instance = static_cast<Camera *>(arg);

        int8_t dtmp[64];
        uart_event_t event;

        while (!FREETask::waitForNotify(FREETask::stopBit, 0))
        {
            while (FREETask::waitForNotify(FREETask::pauseBit, FREETask::MAX_DELAY)) {}

            if (!xQueueReceive(instance->queHandle, &event, portMAX_DELAY))
                continue;

            if (event.type != UART_PATTERN_DET)
            {
                uart_flush_input(instance->uartNum);
                continue;
            }

            int pos = uart_pattern_pop_pos(instance->uartNum);
            if (pos == -1)
                continue;

            if (pos >= static_cast<int>(sizeof(dtmp)))
            {
                uart_flush(instance->uartNum);
                continue;
            }

            int readLen = uart_read_bytes(instance->uartNum, dtmp, pos + 1, pdMS_TO_TICKS(100));
            if (readLen >= 0)
            {
                dtmp[readLen] = '\0';
            }

            etl::string<32> tmpString{reinterpret_cast<char *>(dtmp)};
            instance->last_word = tmpString;

            instance->eventGroup.setBits(instance->colorSensorBit);
        }
    }

    Camera(int buffsize, int queSize);

public:
    bool uartConfiguration(pc_uart_config_t uartConfig,
                           uart_port_t uartNum,
                           int buffsize,
                           int rxPin,
                           int txPin,
                           int queSize,
                           QueueHandle_t queHandle,
                           EventGroup<EventGroupHandle_t> &p_eventGroup,
                           EventBits_t eb,
                           FREETask &task)
    {
        if (uartConfig.baud_rate == 0)
        {
            uartConfig.parity = UART_PARITY_DISABLE;
            uartConfig.data_bits = UART_DATA_8_BITS;
            uartConfig.baud_rate = 115200;
            uartConfig.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            uartConfig.source_clk = UART_SCLK_DEFAULT;
            uartConfig.stop_bits = UART_STOP_BITS_1;
        }

        this->uartNum = uartNum;
        this->cameraUart = uartConfig;
        this->buffsize = buffsize;
        this->rxPin = rxPin;
        this->txPin = txPin;
        this->queSize = queSize;
        this->eventGroup = p_eventGroup;
        this->colorSensorBit = eb;

        if (!queHandle)
        {
            queHandle = xQueueCreate(queSize, sizeof(uart_event_t));
        }
        if (queHandle == nullptr)
            return false;

        this->queHandle = queHandle;

        uart_param_config(this->uartNum, &this->cameraUart);
        uart_set_pin(this->uartNum, this->txPin, this->rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(this->uartNum, this->buffsize, 0, this->queSize, &this->queHandle, 0);

        // xTaskCreate(uart_task_wrapper, "uartTask", 4096, this, 5, &task);

        uart_enable_pattern_det_baud_intr(this->uartNum, '\n', 1, 9, 0, 0);
        uart_pattern_queue_reset(this->uartNum, this->queSize);

        return true;
    }

    Camera(pc_uart_config_t uartConfig,
           uart_port_t uartNum,
           int buffsize,
           int rxPin,
           int txPin,
           int queSize,
           QueueHandle_t queHandle,
           EventGroup<EventGroupHandle_t> &p_eventGroup,
           EventBits_t eb,
           FREETask &task)
        : cameraUart(uartConfig),
          uartNum(uartNum),
          buffsize(buffsize),
          rxPin(rxPin),
          txPin(txPin),
          queSize(queSize),
          queHandle(queHandle),
          eventGroup(p_eventGroup),
          colorSensorBit(eb),
          task(task)
    {
    }

    static etl::EventFlags<Camera> create(int buffsize,
                                          int queSize,
                                          pc_uart_config_t &uartCfg,
                                          uart_port_t uartNum,
                                          uint16_t txPin,
                                          uint16_t rxPin,
                                          EventGroupHandle_t &eg,
                                          EventBits_t eb);

    bool stopListeningIT();
    bool listenIT();
    bool turnOn();
    bool turnOff();
    int getSample() const;
};
