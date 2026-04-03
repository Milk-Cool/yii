#pragma once
#include <Arduino.h>
#include <vector>
#include <string>

#define DEVICE_NAME "Y$"

typedef void(*DeviceCallback)(const uint8_t* addr, std::string data, int8_t rssi);
void ble_init();
void ble_set_data(std::vector<uint8_t> vec, bool minimize_intervals);
void ble_set_cb(DeviceCallback acb);