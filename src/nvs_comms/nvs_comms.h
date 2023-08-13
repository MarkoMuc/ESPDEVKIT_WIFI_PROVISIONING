#ifndef ESP_LOG
#define ESP_LOG
#include "esp_log.h"
#endif

#ifndef NVS_FLASH
#define NVS_FLASH
#include "nvs_flash.h"
#endif

#include "nvs.h"
#ifndef SYSTEM
#define SYSTEM
#include "esp_system.h"
#endif

// MACROS

#define KEY_BOOT "BTYPE" // Key for storing the boot type
#define KEY_PASS "PASS" // Key for storing the password
#define KEY_SSID "SSID" // Key for storing the ssid


// Description: Initializes the NVS flash and reads the boot_type from it
//              
// Parameters : no parameters
// Returns :  returns a uint8_t that is either 0 - AP mode or 1 - STA mode
uint8_t init_NVS();

// Description: Saves the boot_type to NVS Flash and resets the device
//              
// Parameters : uint8_t boot_type - value to set the boot type to
// Returns :  no return
void save_boot_type_reset(uint8_t boot_type);

// Description: Saves the password, SSID to NVS Flash.
//              Also sets boot_type to 1 (STA mode), at the end restarts the device
// Parameters : uint8_t boot_type - value to set the boot type to
// Returns :  no return
void save_creds_reset(char* password, char* ssid, uint8_t* boot_type);

// Description: Gets the WIFI credentials, password and SSID from NVS flash
//              
// Parameters : char* password - pointer to the password
//              char* ssid - pointer to the ssid 
// Returns :  ESP_OK - success
esp_err_t nvs_get_creds(char* password, char* ssid);