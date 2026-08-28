

#include "EventGroupIsrTransoptor.hpp"
#include "esp_attr.h"


etl::optional< EventGroupIsrTransoptor > EventGroupIsrTransoptor::create(uint32_t photoPin , uint32_t emitPin , EventGroupHandle_t &evtGrp , EventBits_t evtBits )
{
    EventGroupIsrTransoptor temp{ photoPin , emitPin};

    if ( temp.init( evtGrp , evtBits) != ESP_OK ) 
        return etl::nullopt ;
    
    return temp ;
} 

EventGroupIsrTransoptor::EventGroupIsrTransoptor( uint32_t photoPin , uint32_t emitPin ) : phototransistorPin(photoPin), emitterPin( emitPin ) {} 

int EventGroupIsrTransoptor::init(EventGroupHandle_t &evtGroup , EventBits_t evtBits)
{
    esp_err_t err_code{} ;

    photoGpio.pin_bit_mask = 1ULL << phototransistorPin ;
    photoGpio.mode = GPIO_MODE_INPUT ; 
    photoGpio.pull_up_en = GPIO_PULLUP_DISABLE ;
    photoGpio.pull_down_en = GPIO_PULLDOWN_ENABLE ;
    photoGpio.intr_type = GPIO_INTR_DISABLE ;

    
    emitGpio.pin_bit_mask = 1ULL << emitterPin ;
    emitGpio.mode = GPIO_MODE_OUTPUT ; 
    emitGpio.pull_up_en = GPIO_PULLUP_DISABLE ; 
    emitGpio.pull_down_en = GPIO_PULLDOWN_ENABLE ; 
    emitGpio.intr_type   = GPIO_INTR_DISABLE ; 


    if ( ( err_code = gpio_config(&photoGpio )) != ESP_OK )  return err_code ; 
    if ( ( err_code = gpio_config(&emitGpio )) != ESP_OK ) return err_code ;



    static bool isrsEnabled =  false ;
    if (!isrsEnabled)   err_code = gpio_install_isr_service( 0 ) ;


    eventGroup = evtGroup ; 
    colorSensorBit =  evtBits ;


    err_code = gpio_isr_handler_add( (gpio_num_t)phototransistorPin , isrHandler, (void*) this ) ;

    return err_code ;
}

bool  EventGroupIsrTransoptor::listenIT(void)  
{
    if ( gpio_set_intr_type( (gpio_num_t)phototransistorPin, GPIO_INTR_POSEDGE) != ESP_OK) return false ;
    return true; 
}

bool  EventGroupIsrTransoptor::stopListeningIT(void)   
{
    if ( gpio_set_intr_type( (gpio_num_t)phototransistorPin, GPIO_INTR_DISABLE) != ESP_OK) return false ;
    return true ;
}


void IRAM_ATTR EventGroupIsrTransoptor::isrHandler(  void * arg){
    

    EventGroupIsrTransoptor* instance = static_cast< EventGroupIsrTransoptor* >( arg ) ;

    gpio_intr_disable((gpio_num_t) instance->phototransistorPin);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE ; 

    BaseType_t result = xEventGroupSetBitsFromISR(
                instance->eventGroup, 
                instance->colorSensorBit, 
                &xHigherPriorityTaskWoken ) ;
                
    if (result == pdPASS && xHigherPriorityTaskWoken == pdTRUE) 
        portYIELD_FROM_ISR();

}


bool EventGroupIsrTransoptor::turnOn(void)
{
    if ( !phototransistorPin || !emitterPin) return false; 

    if ( gpio_set_level( (gpio_num_t)emitterPin, 1 ) != ESP_OK ) return false  ; 

    return true ;
}
bool EventGroupIsrTransoptor::turnOff(void)
{
    if ( !phototransistorPin || !emitterPin) return false; 

    if ( gpio_set_level( (gpio_num_t)emitterPin, 0 ) != ESP_OK ) return false  ; 

    return true ;
}


const void* EventGroupIsrTransoptor::getSample()const 
{
    if ( !phototransistorPin || !emitterPin) return (void*)false; 

    if ( !gpio_get_level( (gpio_num_t)phototransistorPin ))  return (void*)false ;

    return (void*)true ;
}
 