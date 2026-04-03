#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <vector>

bool bus_is_complete(std::vector<uint8_t>& inp);

#define TYPE_END 0
#define TYPE_U8 1
#define TYPE_U16 2
#define TYPE_U32 3
#define TYPE_STR 8
#define TYPE_NAME 255
typedef struct {
    uint8_t type;
    uint32_t u; // u8, u16, u32; big endian
    String str; // str, name
} BusValue;
typedef struct {
    String name;
    BusValue val;
} BusEntry;
typedef std::vector<BusEntry> BusData;
BusData bus_parse(std::vector<uint8_t>& inp);

std::vector<uint8_t> bus_compile(BusData& data);