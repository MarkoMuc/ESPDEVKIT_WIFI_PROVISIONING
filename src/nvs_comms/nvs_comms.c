#include "nvs_comms.h"

uint8_t init_NVS() {
    uint8_t boot_type;

    ESP_LOGI("NVS INIT", "Initializing NVS Flash.");

    // Initializes NVS Flash
    esp_err_t errCode = nvs_flash_init();
    if ( errCode == ESP_ERR_NVS_NO_FREE_PAGES || errCode == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
        // In case of no free pages in the NVS Flash or corrupted/unrecognizable data, erase it
        // and reinitialize 
        ESP_ERROR_CHECK(nvs_flash_erase());
        errCode = nvs_flash_init();
    }

    ESP_ERROR_CHECK(errCode);

    ESP_LOGI("NVS INIT", "FInished initializing NVS Flash & Starting NVS Read.");

    // Creates a handle and opens a connection to the NVS Flash
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    errCode = nvs_get_u8(nvs_handle, KEY_BOOT, &boot_type);
    
    switch (errCode) {
        case ESP_OK:
            ESP_LOGI("NVS BOOT","Boot Type = %" PRIu8 "\n", boot_type);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI("NVS BOOT","The value is not initialized yet!\n");
            boot_type = 0; // Defaults to AP mode
            break;
        default :
            ESP_ERROR_CHECK(errCode);
    }

    if(boot_type > 1) {
        // Boot type can never be over 1, so set it to 0(default)
        boot_type = 0;
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, KEY_BOOT, boot_type));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle));
        ESP_LOGI("NVS BOOT", "NVS boot type : %u", boot_type);
    } 
    nvs_close(nvs_handle);
    
    ESP_LOGI("NVS BOOT", "Finished NVS READ and WRITE.");

    return boot_type;
}

void save_boot_type_reset(uint8_t boot_type) {
    nvs_handle_t nvs_handle;
    // Opens a connection to the NVS Flash and saves boot_type
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, KEY_BOOT, boot_type));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    
    nvs_close(nvs_handle);
    
    // Restarts the device
    esp_restart();
}

void save_creds_reset(char* password, char* ssid, uint8_t* boot_type) {
    nvs_handle_t nvs_handle;
    *boot_type = 1;
    // Opens a connection to the NVS Flash and saves credentials and boot_type
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, KEY_PASS, password));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, KEY_SSID, ssid));
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, KEY_BOOT, *boot_type));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    
    nvs_close(nvs_handle);
    // Restarts the device
    esp_restart();
}

esp_err_t nvs_get_creds(char* password, char* ssid) {
    nvs_handle_t nvs_handle;
    size_t length;
    size_t length2;
    
    // Opens a connection to the NVS Flash and returns the credentials from it
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READONLY, &nvs_handle));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, KEY_PASS, password, &length));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, KEY_SSID, ssid, &length2));
    ESP_LOGI("NVS READ", "pass=%s | length:%d, sid=%s | length %d",password,length,ssid,length2);
    
    nvs_close(nvs_handle);

    return ESP_OK;
}