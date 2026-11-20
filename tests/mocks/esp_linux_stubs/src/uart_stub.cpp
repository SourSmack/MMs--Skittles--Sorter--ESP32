#include "driver/uart.h"

esp_err_t uart_param_config(uart_port_t, const uart_config_t *) { return ESP_OK; }
esp_err_t uart_set_pin(uart_port_t, int, int, int, int) { return ESP_OK; }
esp_err_t uart_driver_install(uart_port_t, int, int, int, QueueHandle_t *, int) { return ESP_OK; }
esp_err_t uart_driver_delete(uart_port_t) { return ESP_OK; }
int       uart_write_bytes(uart_port_t, const void *, size_t size) { return static_cast<int>(size); }
int       uart_read_bytes(uart_port_t, void *, uint32_t, TickType_t) { return 0; }
esp_err_t uart_flush(uart_port_t) { return ESP_OK; }
esp_err_t uart_flush_input(uart_port_t) { return ESP_OK; }
esp_err_t uart_wait_tx_done(uart_port_t, TickType_t) { return ESP_OK; }
esp_err_t uart_get_buffered_data_len(uart_port_t, size_t *size) { if (size) *size = 0; return ESP_OK; }
