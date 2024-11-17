#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "esp.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

void register_scan_networks_cmd(void);
void register_join_network_cmd(void);
void register_disconnect_from_network_cmd(void);
void register_display_network_info_cmd(void);

#endif //_COMMANDS_H_