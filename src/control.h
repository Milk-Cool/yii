#pragma once
#include <stdint.h>
#include <string>

void control_handle_advertisement(const uint8_t* addr, std::string data, int8_t rssi);
void control_init();
void control_loop();