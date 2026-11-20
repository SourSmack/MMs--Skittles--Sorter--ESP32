#include "driver/gpio.h"

esp_err_t gpio_config(const gpio_config_t *) { return ESP_OK; }
esp_err_t gpio_reset_pin(gpio_num_t) { return ESP_OK; }
esp_err_t gpio_set_intr_type(gpio_num_t, gpio_int_type_t) { return ESP_OK; }
esp_err_t gpio_intr_enable(gpio_num_t) { return ESP_OK; }
esp_err_t gpio_intr_disable(gpio_num_t) { return ESP_OK; }
esp_err_t gpio_set_level(gpio_num_t, uint32_t) { return ESP_OK; }
int       gpio_get_level(gpio_num_t) { return 0; }
esp_err_t gpio_set_direction(gpio_num_t, gpio_mode_t) { return ESP_OK; }
esp_err_t gpio_set_pull_mode(gpio_num_t, gpio_pull_mode_t) { return ESP_OK; }
esp_err_t gpio_install_isr_service(int) { return ESP_OK; }
void      gpio_uninstall_isr_service(void) {}
esp_err_t gpio_isr_handler_add(gpio_num_t, gpio_isr_t, void *) { return ESP_OK; }
esp_err_t gpio_isr_handler_remove(gpio_num_t) { return ESP_OK; }
