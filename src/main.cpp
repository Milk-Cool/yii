#include <Arduino.h>
#include "bus.h"
#include "storage.h"
#include "blecomm.h"
#include "state.h"
#include "random.h"
#include "tts.h"

static uint8_t mode;
static bool setup_write = false;

void setup() {
    Serial.begin(115200);
    Serial.println("i'm alive!!");
    storage_init();
    mode = storage().getUChar("mode", 0); // 0 - setup, 1 - control, 2 - doll
    if(mode != 0) {
        tts_init();
        random_init();
        ble_init();
        state_init();
        ble_set_cb(handle_advertisement);

        tts_play("ah!");
    } else {
        pinMode(2, OUTPUT);
        digitalWrite(2, HIGH);
    }
}

static BusData get_serial_data() {
    std::vector<uint8_t> data;
    while(!bus_is_complete(data)) {
        int b = Serial.read();
        if(b == -1) continue;
        data.push_back(b);
    }
    return bus_parse(data);
}

static uint64_t boot_held_since;
static bool boot_held = false;
void loop() {
    if(mode == 0) {
        BusData data = get_serial_data();
        BusData out;
        for(BusEntry& ent : data) {
            if(ent.val.type == TYPE_NAME) continue; // something is wrong
            if(ent.name == "!setup_write" && ent.val.type == TYPE_U8) {
                setup_write = ent.val.u;
                continue;
            }
            if(setup_write) {
                if(ent.val.type == TYPE_STR) storage().putString(ent.name.c_str(), ent.val.str);
                else if(ent.val.type == TYPE_U8) storage().putUChar(ent.name.c_str(), ent.val.u);
                else if(ent.val.type == TYPE_U16) storage().putUShort(ent.name.c_str(), ent.val.u);
                else if(ent.val.type == TYPE_U32) storage().putUInt(ent.name.c_str(), ent.val.u);
                else if(ent.val.type == TYPE_RESET) storage().remove(ent.name.c_str());
            }
            if(ent.val.type == TYPE_STR) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_STR, .str = storage().getString(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U8) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U8, .u = storage().getUChar(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U16) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U16, .u = storage().getUShort(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U32) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U32, .u = storage().getUInt(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_RESET) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_RESET } });
        }
        if(out.size() > 0) {
            auto bin = bus_compile(out);
            Serial.write(bin.data(), bin.size());
        }
        delay(1);
        return;
    }

    // reboot into config mode if GP0 is held for >=3s
    if(digitalRead(0) == LOW) {
        if(!boot_held) {
            boot_held = true;
            boot_held_since = millis();
        }
        if(millis() - boot_held_since >= 3000) {
            storage().putUChar("mode", 0);
            ESP.restart();
        }
    } else boot_held = false;

    state_loop();
}