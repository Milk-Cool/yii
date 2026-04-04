#include "state.h"
#include <vector>
#include <string.h>
#include "storage.h"
#include "blecomm.h"
#include <esp_mac.h>
#include "random.h"
#include "tts.h"
#include "data/data.h"

#define ADVERTISEMENT_LENGTH 31
#define FREE_SPACE_LENGTH (ADVERTISEMENT_LENGTH - 7 - 2) // 7 bytes occupied by other fields, 2 bytes occupied by length and type

static std::vector<uint8_t> advertisement;

static char name[13] = { 0 };
static uint8_t activity = 0;
static uint8_t variant = 0;
static uint8_t dialogue = 0;
static uint8_t level = 0;
static uint8_t xp = 0; // 4b
static uint8_t saturation = 0; // 4b
static uint8_t thing = 0;
static uint8_t special_data[4] = { 0 };

static std::vector<uint8_t> met;

static uint8_t my_mac[6];

static bool speaking = false;

static void update() {
    advertisement.erase(advertisement.begin(), advertisement.end());
    
    for(uint8_t i = 0; i < 12; i++) advertisement.push_back(name[i]);
    advertisement.push_back(activity);
    advertisement.push_back(variant);
    advertisement.push_back(dialogue);
    advertisement.push_back(level);
    advertisement.push_back((xp << 4) | saturation);
    advertisement.push_back(thing);
    for(uint8_t i = 0; i < 4; i++) advertisement.push_back(special_data[i]);

    storage().putString("name", name);
    storage().putUChar("level", level);
    storage().putUChar("xp", xp);
    storage().putUChar("saturation", saturation);

    ble_set_data(advertisement, activity != 0);
}

void state_init() {
    esp_read_mac(my_mac, ESP_MAC_BT);

    String name_loaded = storage().getString("name");
    memcpy(name, name_loaded.c_str(), name_loaded.length());

    level = storage().getUChar("level");
    xp = storage().getUChar("xp");
    saturation = storage().getUChar("saturation");

    if(storage().getBytesLength("met") > 0) {
        met.reserve(storage().getBytesLength("met"));
        storage().getBytes("met", met.data(), met.size());
    }

    update();
}

int16_t get_met(uint8_t* mac) {
    for(int i = 0; i < met.size(); i += 7) {
        if(!memcmp(mac, met.data() + i, 6))
            return met[i + 6];
    }
    return -1;
}
void put_met(uint8_t* mac, uint8_t val) {
    for(int i = 0; i < met.size(); i += 7)
        if(!memcmp(mac, met.data() + i, 6)) {
            met[i + 6] = val;
            storage().putBytes("met", met.data(), met.size());
            return;
        }
    for(int i = 0; i < 6; i++)
        met.push_back(mac[i]);
    met.push_back(val);
    storage().putBytes("met", met.data(), met.size());
}

void handle_advertisement(const uint8_t* addr, std::string data, int8_t rssi) {
    uint8_t mac[6];
    for(uint8_t i = 0; i < 6; i++)
        mac[i] = addr[5 - i];
    if(!memcmp(my_mac, mac, 6))
        return; // that's me!
    
    Serial.println(rssi);
    if(rssi < -65) return; // not close enough!!
    char their_name[13] = { 0 };
    memcpy(their_name, data.data(), 12);
    Serial.println(their_name);
    uint8_t their_activity = data[12];
    uint8_t their_variant = data[13];
    uint8_t their_dialogue = data[14];
    uint8_t their_level = data[15];
    uint8_t their_xp = (data[16] >> 4) & 0xf;
    uint8_t their_saturation = data[16] & 0xf;
    uint8_t their_thing = data[17];
    uint8_t their_special_data[4];
    memcpy(their_special_data, data.data() + 18, 4);

    if(activity == 0 && their_activity == 0 && memcmp(my_mac, mac, 6) < 0) {
        // my mac is less than theirs, i can start the conversation
        // yes, seriously -- this is how it works
        if(get_met(mac) == -1) {
            variant = random_get() % activities[ACTIVITY_INTRODUCTION].size();
            activity = ACTIVITY_INTRODUCTION;
            dialogue = 0;
            speaking = true;
            tts_play(activities[activity][variant][dialogue]);
        }
    } else if(their_activity != 0 && (activity == 0 || their_activity == activity) && their_dialogue > dialogue) {
        if(their_dialogue != activities[their_activity][their_variant].size() - 1) {
            activity = their_activity;
            variant = their_variant;
            dialogue = their_dialogue + 1;
            speaking = true;
            tts_play(activities[activity][variant][dialogue]);
        }
    }

    if(speaking && tts_done())
        speaking = false;
    if(!speaking) update();
}

static uint64_t saturation_last_decreased = 0;

void state_loop() {
    bool changed = false;
    uint64_t now = millis();
    if(now - saturation_last_decreased >= 8 * 60 * 1000) {
        if(saturation > 0) saturation--;
        saturation_last_decreased = now;
        changed = true;
    }

    if(changed) update();
}