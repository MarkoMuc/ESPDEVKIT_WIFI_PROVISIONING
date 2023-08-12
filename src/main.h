#include "wifi_stations/wifi_stations.h"

#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif

// MACROS

#define LED_RED     GPIO_NUM_12
#define LED_YELLOW  GPIO_NUM_14
#define LED_GREEN   GPIO_NUM_27
#define LED_BIT_MASK ((1ULL << LED_RED) | (1ULL << LED_YELLOW) | (1ULL << LED_GREEN))

EventGroupHandle_t s_wifi_event_group;

// Function prototypes

// Description: Initializes and configures 3 pins for usage as control LEDs
//
// Parameters : uint64_t gpio_bit_mask - Bit mask of the 3 GPIO pins
//
// Returns :  returns esp_err_t type
//           ESP_OK - success
//           ESP_ERR_INVALID_ARG - Parameter error
esp_err_t  init_leds(uint64_t gpio_bit_mask);
// Description: Event handler for the AP station events
//              Responds to a client connecting or disconnecting to the WIFI station
// Parameters : void* arg - pointer to the arguments
//              esp_event_base_t event_base - pointer to subsystem events
//              int32_t event_id - event id
//              void* event_data - event data
// Returns :  no return
static void wifi_ap_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
// Description: Event handler for the STA station events
//              Responds to the device connecting and receiving IP from the STA WIFI station
// Parameters : void* arg - pointer to the arguments
//              esp_event_base_t event_base - pointer to subsystem events
//              int32_t event_id - event id
//              void* event_data - event data
// Returns :  no return
static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

// Description: Initializes the system peripherals, 
//              
// Parameters : void* ap_event - pointer to the AP event handler
//              void* sta_event - pointer to the STA event handler
//              uint8_t test_reset - To reset the device to AP mode
// Returns :  no return
void system_boot(void* ap_event, void* sta_event, uint8_t test_reset);

// Description: Resets back to AP mode
//              
// Parameters : no parameters
// Returns :  no return
void reset_testing();
