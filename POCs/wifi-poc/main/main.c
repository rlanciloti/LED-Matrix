#include "string.h"
#include "sysconfig.h"
#include "esp.h"
#include "wifi_helper.h"

#define DELAY_BEFORE_RESCAN_SECONDS 60

#if(DEMO == 0)
int main(void) {
    while(1)
    {
        wifi_ap_record_t* ap_info = NULL;
        uint16_t ap_count;

        scan_networks(&ap_info, &ap_count);

        display_scanned_aps(ap_info, ap_count);
        PRINT("\n")

        for(int i = DELAY_BEFORE_RESCAN_SECONDS; i >= 0; i--) {
            PRINT("\rRESCANNING FOR NETWORKS IN %d SECOND(S).", i);
            sleep_s(1);
        }

        PRINT("\n\n")

        free(ap_info);
    }

    return 0;
}

#elif(DEMO == 1)

int main(void) {
    int index;
    wifi_ap_record_t* ap_info;
    uint16_t ap_count;

    scan_networks(&ap_info, &ap_count);
    display_scanned_aps(ap_info, ap_count);
    PRINT("\n")

    for(int i = 0; i < 5; i++) {
        PRINT("\rAttempting to connecting to SSID: %s in %d seconds.", SSID, 5 - i);
        sleep_s(1);
    }
    PRINT("\n")

    for(index = 0; index < ap_count; index++) {
        if(!strcmp(SSID, (const char*)ap_info[index].ssid)) break;
    }

    if(index != ap_count && connect_to_network(&(ap_info[index]), PASS) == NetworkConnectErrors_SUCCESS)
    {
        tcp_server_init(8888);
    }
}

#elif(DEMO == 2)

int main(void) {

}

#endif
