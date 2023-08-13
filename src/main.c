#include "main.h"

// Global Variables
httpd_handle_t server = NULL; // Holds the handle to the server
uint8_t boot_type = -1; // Boot type : AP mode = 0, STA mode = 1
static int retry_num = 0; // Number of retries

void app_main() {
    system_boot(&wifi_ap_event_handler, &wifi_sta_event_handler, 0);   
}

esp_err_t init_leds(uint64_t gpio_bit_mask) {
    // Struct for holding the configured settings
    gpio_config_t config_gpio = {};

    ESP_LOGI("LED", "Configuring LED GPIO pins.");

    // GPIO pins are set to OUTPUT mode with interrupt, pull up and down disabled   
    config_gpio.intr_type = GPIO_INTR_DISABLE;
    config_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config_gpio.pull_up_en = GPIO_PULLUP_DISABLE;
    config_gpio.mode = GPIO_MODE_OUTPUT;
    config_gpio.pin_bit_mask = gpio_bit_mask;

    ESP_LOGI("LED", "Configured LED pins.");
    return gpio_config(&config_gpio);
}

static void wifi_ap_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI("AP STATION", "Client joined station "MACSTR" AID=%d",
                 MAC2STR(event->mac), event->aid);
        if (server == NULL) {
            server = start_webserver();
        }
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI("AP STATION", "Client left station "MACSTR" AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retry_num < STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI("WIFI STA", "Retrying to connect to the AP %d.",retry_num);
        } else {
            // If max retries have been met, stops the connecting
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI("WIFI STA","Connecting to the AP failed.");

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("WIFI STA", "Received IP:" IPSTR, IP2STR(&event->ip_info.ip));
        
        retry_num = 0;
        
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void system_boot(void* ap_event, void* sta_event, uint8_t test_reset) {
    int leds[] = {LED_RED, LED_YELLOW, LED_GREEN};
    ESP_ERROR_CHECK(init_leds(LED_BIT_MASK)); // Inits control LEDs
    gpio_set_level(LED_RED, 1);

    boot_type = init_NVS();

    if(test_reset) {
        reset_testing();
    }

    init_wifi(ap_event, sta_event, boot_type, leds);
}

void reset_testing() {
    save_boot_type_reset(0);
}

esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI("WEB SERVER", "HTTP GET received.");
    
    ESP_ERROR_CHECK(httpd_resp_send(req, getHTMLString, HTTPD_RESP_USE_STRLEN));
    
    return ESP_OK;
}

esp_err_t post_handler(httpd_req_t *req)
{
    char content[100]; // Holds the HTTP POST content
    char ssid[32]; // Holds the SSID
    char password[32]; // HOLD the PASSWORD
    int j = PARSE_PASSWORD_OFFSET; // For iterating through the content
    int count = 0; // For saving the content

    ESP_LOGI("WEB SERVER", "HTTP POST received.");
    size_t recv_size = MIN(req->content_len, sizeof(content));

    // Receives the POST request
    int ret = httpd_req_recv(req, content, recv_size);
    
    // If return value is <=0 then something went wrong
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            // In case of timeout, return 408
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI("WEB SERVER", "HTTP content: %s.",content);

    // Parses the Password and SSID
    for(int i=0 ; i<2; i++) {
        while(1) {
            if( i == 0 ) {
                if(content[j] == '&' && content[j+1] == 'S' 
                    && content[j+2] == 'S' && content[j+3] == 'I'
                    && content[j+4] == 'D' && content[j+5] == '=') {
                    password[count] = '\0';
                 
                    break;
                }
                password[count] = content[j];
            } else {
                if(content[j] == '\0' || (content[j] == 'e' 
                    && content[j+1] == 'E' && content[j+2] == 'N' 
                    && content[j+3] == 'D' && content[j+4] == 'd')) {
                    ssid[count] = '\0';
                    break;
                }
                ssid[count] = content[j];
            }
            j++;
            count++;
        }
        j = j + PARSE_SSID_OFFSET;
        count = 0;
    }

    // Send response
    ESP_ERROR_CHECK(httpd_resp_send(req, postHTMLString, HTTPD_RESP_USE_STRLEN));

    save_creds_reset(password, ssid, &boot_type);
    
    return ESP_OK;
}

