#include <Arduino.h>
#include <BackgroundAudioSpeech.h>
#include <libespeak-ng/voice/en_us.h>
#include "bus.h"
#include "storage.h"

#ifdef ESP32
// #include <ESP32I2SAudio.h>
#else
#include <PWMAudio.h>
PWMAudio audio(2);
#endif
// BackgroundAudioSpeech BMP(audio);

unsigned char cvoice[1024] = { 0 };
static uint8_t mode;
static bool setup_write = false;

void setup() {
    Serial.begin(115200);
    Serial.println("i'm alive!!");
    storage_init();
    mode = storage().getUChar("mode", 0); // 0 - setup, 1 - control, 2 - doll

    // memcpy(cvoice, voice_en_us.data, voice_en_us.len);
    // strcat((char*)cvoice, "pitch 80 110\n");
    // BackgroundAudioVoice vdata = {
    //     .name = voice_en_us.name,
    //     .len = strlen((char*)cvoice),
    //     .data = cvoice
    // };
    // BMP.setVoice(vdata);
    // BMP.begin();
    // BMP.setGain(1);
    // BMP.setPitch(99);
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

void loop() {
    // BMP.flush();
    // BMP.speak("hello world");
    // delay(2000);
    // BMP.speak("my name is john");
    // delay(2000);

    if(mode == 0) {
        BusData data = get_serial_data();
        BusData out;
        for(BusEntry& ent : data) {
            if(ent.val.type == TYPE_NAME) continue; // something is wrong
            Serial.println(ent.name);
            Serial.println(ent.val.type);
            if(ent.name == "!setup_write" && ent.val.type == TYPE_U8) {
                setup_write = ent.val.u;
                continue;
            }
            if(setup_write) {
                if(ent.val.type == TYPE_STR) storage().putString(ent.name.c_str(), ent.val.str);
                else if(ent.val.type == TYPE_U8) storage().putUChar(ent.name.c_str(), ent.val.u);
                else if(ent.val.type == TYPE_U16) storage().putUShort(ent.name.c_str(), ent.val.u);
                else if(ent.val.type == TYPE_U32) storage().putUInt(ent.name.c_str(), ent.val.u);
            }
            if(ent.val.type == TYPE_STR) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_STR, .str = storage().getString(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U8) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U8, .u = storage().getUChar(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U16) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U16, .u = storage().getUShort(ent.name.c_str()) } });
            else if(ent.val.type == TYPE_U32) out.push_back((BusEntry){ .name = ent.name, .val = { .type = TYPE_U32, .u = storage().getUInt(ent.name.c_str()) } });
        }
        if(out.size() > 0) {
            auto bin = bus_compile(out);
            Serial.write(bin.data(), bin.size());
        }
        delay(1);
    }
}