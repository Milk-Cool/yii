#pragma once
#include <string>
#include <stdint.h>

void state_init();
void state_handle_advertisement(const uint8_t* addr, std::string data, int8_t rssi);
void state_loop();