#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "esp.h"
#include "sysconfig.h"
#include "wifi_helper.h"

#define WIFI_CONNECT_SUCCESS    0x01
#define WIFI_CONNECT_FAIL       0x02

#define KEEPALIVE_IDLE 5
#define KEEPALIVE_INTERVAL 5
#define KEEPALIVE_COUNT 3

static EventGroupHandle_t wifi_event_group;
static int wifi_connect_attempts = 0;

static ip_event_got_ip_t* ip_connect_event;
static esp_netif_t* sta_netif;

static int WIFI_INITIALIZED = 0;
static const char* get_auth_mode_string(int authmode);
static void wifi_connect_event_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
);
static void reconnect_to_network();

static int disconnect_called = false;
static int is_connected_to_wifi = false;

static const char* get_auth_mode_string(int authmode) {
    switch (authmode) {
        case WIFI_AUTH_OPEN:
            return "WIFI_AUTH_OPEN";

        case WIFI_AUTH_OWE:
            return "WIFI_AUTH_OWE";
            
        case WIFI_AUTH_WEP:
            return "WIFI_AUTH_WEP";

        case WIFI_AUTH_WPA_PSK:
            return "WIFI_AUTH_WPA_PSK";

        case WIFI_AUTH_WPA2_PSK:
            return "WIFI_AUTH_WPA2_PSK";

        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WIFI_AUTH_WPA_WPA2_PSK";
            
        case WIFI_AUTH_ENTERPRISE:
            return "WIFI_AUTH_ENTERPRISE";
            
        case WIFI_AUTH_WPA3_PSK:
            return "WIFI_AUTH_WPA3_PSK";
            
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WIFI_AUTH_WPA2_WPA3_PSK";
            
        case WIFI_AUTH_WPA3_ENT_192:
            return "WIFI_AUTH_WPA3_ENT_192";
            
        default:
            return "WIFI_AUTH_UNKNOWN";
    }
}

static void wifi_connect_event_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        is_connected_to_wifi = false;
        if(!disconnect_called){
            if(wifi_connect_attempts < MAX_WIFI_CONNECTION_RETRIES) {
                DBG_PRINT("Failed connecting to Wi-Fi, retrying.")
                esp_wifi_connect();
                wifi_connect_attempts++;
            } else {
                xEventGroupSetBits(wifi_event_group, WIFI_CONNECT_FAIL);
            }
        }
        disconnect_called = false;

    } else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        is_connected_to_wifi = true;
        ip_connect_event = (ip_event_got_ip_t*) event_data;
        wifi_connect_attempts = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECT_SUCCESS);
    }
}

void wifi_sta_init() {
    if(WIFI_INITIALIZED) return;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_connect_event_handler,
        NULL,
        &instance_any_id
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_connect_event_handler,
        NULL,
        &instance_got_ip
    ));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    reconnect_to_network();

    WIFI_INITIALIZED = 1;
}

void scan_networks(wifi_ap_record_t** ap_info, uint16_t* ap_count) {
    esp_err_t ret;
    uint16_t ap_info_size;
    uint8_t scan_attempt = 0;

    do {
        ESP_ERROR_CHECK(esp_wifi_clear_ap_list());
        DBG_PRINT("Scanning Networks\n")

        ret = esp_wifi_scan_start(NULL, true);
        if(ret == ESP_ERR_WIFI_TIMEOUT || ret == ESP_ERR_WIFI_STATE)
        {
            DBG_PRINT("Scanning Networks - Attempt #%d\n", scan_attempt + 1)
            scan_attempt++;
            sleep_ms(500);
        }

    } while(scan_attempt <= MAX_SCAN_RETRIES && (ret == ESP_ERR_WIFI_TIMEOUT || ret == ESP_ERR_WIFI_STATE));

    if(scan_attempt > MAX_SCAN_RETRIES && ret != ESP_OK)
    {
        PRINT(
            "Failed to scan network %d time(s). Please manual reboot device or wait %d second(s).\n", 
            scan_attempt - 1,
            ERR_DELAY_BEFORE_REBOOT_SECONDS
        )

        for(int i = 0; i < ERR_DELAY_BEFORE_REBOOT_SECONDS; i++)
        {
            PRINT("\rReboot in %d seconds.", ERR_DELAY_BEFORE_REBOOT_SECONDS - i)
            sleep_s(1);
        }
        esp_restart();
    }

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_info_size));

    *ap_count = MIN(ap_info_size, MAX_SCAN_LIST_SIZE);
    *ap_info = calloc(sizeof(wifi_ap_record_t), *ap_count);

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(ap_count, *ap_info));
}

void display_scanned_aps(wifi_ap_record_t* ap_info, uint16_t ap_count) {
    if(ap_info == NULL) {
        PRINT("Provided argument `ap_info` is null.\n")
        return;
    }

    for(int i = 0; i < ap_count; i++) {
        char mac[19] = {0};
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
            ap_info[i].bssid[0],
            ap_info[i].bssid[1],
            ap_info[i].bssid[2],
            ap_info[i].bssid[3],
            ap_info[i].bssid[4],
            ap_info[i].bssid[5]
        );

        PRINT(
            "[%02d] SSID: %*s | MAC: %s | AUTH: %-35s | Strength: %-3d\n", 
            i,
            (35 + (strlen((const char*)ap_info[i].ssid) - strlen_unicode((const char*)ap_info[i].ssid))) * -1,
            ap_info[i].ssid, 
            mac,
            get_auth_mode_string(ap_info[i].authmode),
            ap_info[i].rssi
        )
    }
}

int connect_to_network(wifi_ap_record_t* ap_info, char* pass) {   
    if(ap_info == NULL) return NetworkConnectErrors_NULL_AP_INFO;
    if(pass == NULL) return NetworkConnectErrors_NULL_PASSWORD; 
    if(is_connected_to_wifi) disconnect_from_network();

    wifi_config_t config = {
        .sta = {
            .sae_pwe_h2e = WPA3_SAE_PWE_HUNT_AND_PECK,
            .sae_h2e_identifier = ""
        },
    };

    memcpy_safe((char*) config.sta.ssid, (char*) ap_info->ssid, 32);
    memcpy_safe((char*) config.sta.password, (char*) pass, 64);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group, 
        WIFI_CONNECT_SUCCESS | WIFI_CONNECT_FAIL,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY 
    );

    if(bits & WIFI_CONNECT_SUCCESS) {
        return NetworkConnectErrors_SUCCESS;
        
    } else if(bits & WIFI_CONNECT_FAIL) {
        PRINT("Failed to connecte to AP\n")
        return NetworkConnectErrors_FAILED_TO_CONNECT;

    } else {
        PRINT("Unexpected case reached.")
        return NetworkConnectErrors_UNKNOWN;
    }
}

int disconnect_from_network() {
    if(!is_connected_to_wifi) return 0;
    disconnect_called = true;
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    return 0;
}

int get_is_connected_to_wifi() {
    return is_connected_to_wifi;
}

void get_network_details(WifiConnectionDetails* details) {
    if(!is_connected_to_wifi) return;

    uint8_t mac[6];
    esp_netif_ip_info_t ip_info;
    
    esp_netif_get_ip_info(sta_netif, &ip_info);
    esp_wifi_get_mac(WIFI_IF_STA, mac);

    sprintf(details->ip_address, IPSTR, IP2STR(&ip_info.ip));
    sprintf(details->gateway_address, IPSTR, IP2STR(&ip_info.gw));
    sprintf(details->netmask, IPSTR, IP2STR(&ip_info.netmask));
    sprintf(details->mac_address, MACSTR, MAC2STR(mac));
}

static void reconnect_to_network() {
    if(is_connected_to_wifi) disconnect_from_network();
    
    wifi_config_t conf;
    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &conf));

    if(strlen((char*)conf.sta.ssid)) {
        printf("Reconnecting to Wi-Fi: \"%s\"\n", conf.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &conf));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}

static void echo(const int sock) {
    int len;
    char rx_buffer[128];

    do {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if(len < 0) {
            PRINT("Connection closed.\n")
        } else if(len == 0) {
            PRINT("Transmission finished.")
        } else {
            rx_buffer[len] = 0;
            PRINT("Recieved %d bytes: %s", len, rx_buffer);

            int total_bytes_to_send = len;
            while(total_bytes_to_send > 0) {
                int written = send(
                    sock, 
                    rx_buffer + (len - total_bytes_to_send), 
                    total_bytes_to_send, 
                    0
                );
                if(written < 0) {
                    PRINT("Failed to echo message.")
                    return;
                }
                total_bytes_to_send -= written;
            }
        }
    } while(len > 0);
}

static void tcp_server_task(void* pvParamters) {
    struct sockaddr_storage dest_addr;

    int err;
    int server_socket;
    char addr[128] = {0};
    
    int opt = 1;
    int port = (int)pvParamters;
    int ip_protocol = IPPROTO_IP;
    int addr_family = AF_INET;
    
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT; 

    struct sockaddr_in* dest_addr_ip4 = (struct sockaddr_in*) &dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = addr_family;
    dest_addr_ip4->sin_port = htons(port);
    
    server_socket = socket(addr_family, SOCK_STREAM, ip_protocol);

    if(server_socket < 0) {
        PRINT("Could not create socket. Exiting.\n")
        vTaskDelete(NULL);
        return;
    }

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    err = bind(server_socket, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if(err) {
        PRINT("Could not bind the socket on port %d.\n - errno: %d\n", port, errno);
    } else {
        PRINT("Socket created on port %d.\n", port);

        err = listen(server_socket, 1);
        if(err) {
            PRINT("Error occurred during listen: errno %d", errno);
        }
    }


    while(!err) {
        PRINT("Listen for connections.\n")
        
        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);
        
        int sock = accept(server_socket, (struct sockaddr*)&source_addr, &addr_len);

        if(sock < 0) {
            ESP_LOGE("tcp_server_task", "Unable to accept connection: errno %d", errno);
            break;
        }

        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount));

        if(source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in*)&source_addr)->sin_addr, addr, sizeof(addr) - 1);
        }

        PRINT("Connection to client accepted: %s\n", addr)
        
        echo(sock);

        shutdown(sock, 0);
        close(sock);
    }

    PRINT("Shutting down server.")
    close(server_socket);
    vTaskDelete(NULL);
}

void tcp_server_init(int port){
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)port, 5, NULL);
}
