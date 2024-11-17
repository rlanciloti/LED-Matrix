#include "string.h"
#include "wifi_helper.h"
#include "commands.h"

static int scan_networks_cmd(int argc, char** argv);
static int join_network_cmd(int argc, char** argv);
static int disconnect_from_network_cmd(int argc, char** argv);
static int display_network_info_cmd(int argc, char** argv);

static int scan_networks_cmd(int argc, char** argv) {
    wifi_ap_record_t* ap_info;
    uint16_t ap_count;

    scan_networks(&ap_info, &ap_count);
    display_scanned_aps(ap_info, ap_count);

    return 0;
}

void register_scan_networks_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "scan",
        .help = "Scan for Wi-Fi networks",
        .hint = NULL,
        .func = &scan_networks_cmd
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct join_network_cmd_args {
    struct arg_str* ssid;
    struct arg_str* password;
    struct arg_end* end;
} join_network_cmd_args;

typedef struct join_network_cmd_args join_network_cmd_args_t;

static int join_network_cmd(int argc, char** argv) {
    int nerrors = arg_parse(argc, argv, (void **) &join_network_cmd_args);
    
    if (nerrors != 0) {
        arg_print_errors(stderr, join_network_cmd_args.end, argv[0]);
        return 1;
    }

    char* ssid = (char*)(*join_network_cmd_args.ssid->sval);
    char* password = (char*)(*join_network_cmd_args.password->sval);

    if(strlen(ssid) > 32) {
        PRINT("Provided SSID is too long")
        return 1;
    }

    wifi_ap_record_t record;
    strcpy((char*)record.ssid, ssid);

    return connect_to_network(&record, password);
}

void register_join_network_cmd(void) {
    join_network_cmd_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID of AP");
    join_network_cmd_args.password = arg_str0(NULL, NULL, "<pass>", "Password of the network");
    join_network_cmd_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "connect",
        .help = "Attempt to join a near-by Wi-Fi network",
        .hint = NULL,
        .argtable = &join_network_cmd_args,
        .func = &join_network_cmd
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int disconnect_from_network_cmd(int argc, char** argv) {
    disconnect_from_network();
    return 0;
}

void register_disconnect_from_network_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "disconnect",
        .help = "If connected to a network, disassociate from said network.",
        .hint = NULL,
        .func = &disconnect_from_network_cmd
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int display_network_info_cmd(int argc, char** argv) {
    WifiConnectionDetails conn = {0};

    get_network_details(&conn);

    if(strlen(conn.ip_address)) {
        printf("Client IP: %s | Gateway IP: %s | Netmask: %s\n", 
            conn.ip_address, 
            conn.gateway_address, 
            conn.netmask
        );
        printf("Client MAC: %s\n\n", conn.mac_address);
    } else {
        printf("Not connected to network.\n");
    }

    return 0;
}

void register_display_network_info_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "ifconfig",
        .help = "Displays information about the current network configuration",
        .hint = NULL,
        .func = &display_network_info_cmd
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
