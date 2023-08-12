#include "html_strings/html_strings.h"

#ifndef EVENTS_WIFI_SERVER
#define EVENTS_WIFI_SERVER
#include <esp_http_server.h>
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_tls.h"
#include "esp_tls_crypto.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sys/param.h"
#endif

#ifndef NVS_COMMS
#define NVS_COMMS
#include "../nvs_comms/nvs_comms.h"
#endif

#ifndef GPIO
#define GPIO
#include "driver/gpio.h"
#endif

#define AP_SSID "ESP32AP"
#define AP_PASS "useruser"
#define AP_CHANNEL 1
#define AP_MAX_CONN 4

#define STA_MAXIMUM_RETRY  16
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define PARSE_PASSWORD_OFFSET 9
#define PARSE_SSID_OFFSET 6

// Description: Initializes the WIFI, either in AP or STA mode
//              
// Parameters : void *ap_event - pointer to the AP event handler
//              void *sta_event - pointer to the STA event handler
//              uint8_t boot_type - boot type either AP or STA
//              int* leds - GPIO pins for the control LEDs
// Returns :  no return
void init_wifi(void *ap_event, void *sta_event, uint8_t boot_type, int* leds);

// Description: Initializes the WIFI in AP mode
//              
// Parameters : void *event - pointer to the AP event handler
//              int* leds - GPIO pins for the control LEDs
// Returns :  no return
void init_ap(void* event, int* leds);

// Description: Initializes the WIFI in STA mode
//              
// Parameters : void *event - pointer to the STA event handler
//              int* leds - GPIO pins for the control LEDs
// Returns :  no return
void init_sta(void* event, int* leds);

// Description: HTTP GET Request handler
//
// Parameters : httpd_req_t *req - The request, handled by the API
//
// Returns :  returns esp_err_t type
//           ESP_OK - success
esp_err_t get_handler(httpd_req_t *req);

// Description: HTTP POST Request handler
//
// Parameters : httpd_req_t *req - The request, handled by the API
//
// Returns :  returns esp_err_t type
//           ESP_OK - success
//           ESP_FAIL - fail
esp_err_t post_handler(httpd_req_t *req);

// Description: Starts the HTTP Web Server
//
// Parameters : No parameters
//
// Returns :  returns httpd_handle_t
//            NULL - if it fails
httpd_handle_t start_webserver();

extern EventGroupHandle_t s_wifi_event_group;


