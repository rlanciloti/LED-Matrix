#ifndef _WIFI_HELPER_
#define _WIFI_HELPER_
#include <esp_wifi.h>

enum NetworkConnectErrors {
    NetworkConnectErrors_SUCCESS,
    NetworkConnectErrors_INDEX_OUT_OF_RANGE,
    NetworkConnectErrors_FAILED_TO_CONNECT,
    NetworkConnectErrors_NULL_AP_INFO,
    NetworkConnectErrors_NULL_PASSWORD,
    NetworkConnectErrors_UNKNOWN
};

typedef struct WifiConnectionDetails_s {
    char ip_address[16];
    char gateway_address[16];
    char netmask[16];
    char mac_address[18];
} WifiConnectionDetails;


void wifi_sta_init();
void scan_networks(wifi_ap_record_t** ap_info, uint16_t* ap_count);
void display_scanned_aps(wifi_ap_record_t* ap_info, uint16_t ap_count);
int connect_to_network(wifi_ap_record_t* ap_info, char* pass);
int disconnect_from_network();
int get_is_connected_to_wifi();
void get_network_details(WifiConnectionDetails* details);
void tcp_server_init(int porttcp_server_init);

#endif  // _WIFI_HELPER_
