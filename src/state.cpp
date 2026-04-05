#include "state.h"
#include <vector>
#include <mutex>
#include <string.h>
#include "storage.h"
#include "blecomm.h"
#include <esp_mac.h>
#include "random.h"
#include "tts.h"
#include "data/data.h"

#define ADVERTISEMENT_LENGTH 31
#define FREE_SPACE_LENGTH (ADVERTISEMENT_LENGTH - 7 - 2) // 7 bytes occupied by other fields, 2 bytes occupied by length and type

#define RESPONSE_TIMEOUT 12000

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
std::mutex upd_mutex;

static void update() {
    Serial.println("adv upd");
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
    std::lock_guard<std::mutex> guard(upd_mutex);
    esp_read_mac(my_mac, ESP_MAC_BT);

    String name_loaded = storage().getString("name");
    memcpy(name, name_loaded.c_str(), name_loaded.length());

    level = storage().getUChar("level");
    xp = storage().getUChar("xp");
    saturation = storage().getUChar("saturation");

    size_t s = storage().getBytesLength("met");
    if(s > 0) {
        uint8_t d[s];
        storage().getBytes("met", d, s);
        for(size_t i = 0; i < s; i++)
            met.push_back(d[i]);
    }

    update();
}

int16_t get_met(uint8_t* mac) {
    for(size_t i = 0; i < met.size(); i += 7) {
        if(!memcmp(mac, met.data() + i, 6))
            return met[i + 6];
    }
    return -1;
}
void put_met(uint8_t* mac, uint8_t val) {
    for(size_t i = 0; i < met.size(); i += 7)
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

String replace_percents(const char* str, const char* their_name) {
    String o = String(str);
    o.replace("%n", name);
    o.replace("%N", their_name);
    o.replace("%t", things[thing]);
    return o;
}

static uint64_t last = 0;
static uint64_t tts_cooldown = 0;
static uint64_t block_until = 0;

static bool check_can_upd(bool* changed) {
    uint64_t now = millis();
    if(now < block_until) return false;
    if(now - tts_cooldown < 400) return false; // race condition fix
    if(speaking && tts_done()) {
        Serial.println("done speaking");
        speaking = false;
        tts_cooldown = now;
        if(changed != NULL) *changed = true;
        return true;
    }
    if(speaking) return false;
    return true;
}

void handle_advertisement(const uint8_t* addr, std::string data, int8_t rssi) {
    std::lock_guard<std::mutex> guard(upd_mutex);
    if(!check_can_upd(NULL)) return;

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
        int16_t m = get_met(mac);
        if(m == -1) {
            variant = random_get() % activities[ACTIVITY_INTRODUCTION].size();
            activity = ACTIVITY_INTRODUCTION;
            dialogue = 0;
            speaking = true;
            put_met(mac, RELATION_GETTING_FAMILIAR_MIN);
            tts_play(replace_percents(activities[activity][variant][dialogue], their_name).c_str());
        } else if((m >= RELATION_GETTING_FAMILIAR_MIN && m <= RELATION_GETTING_FAMILIAR_MAX) || m == RELATION_ACQUAINTANCES) {
            thing = random_get() % things.size();
            variant = random_get() % activities[ACTIVITY_GETTING_TO_KNOW_EACH_OTHER].size();
            activity = ACTIVITY_GETTING_TO_KNOW_EACH_OTHER;
            dialogue = 0;
            speaking = true;
            tts_play(replace_percents(activities[activity][variant][dialogue], their_name).c_str());
            if(m == RELATION_ACQUAINTANCES) {}
            else if(m == RELATION_GETTING_FAMILIAR_MAX) put_met(mac, RELATION_ACQUAINTANCES);
            else put_met(mac, m + 1);
        }
        tts_cooldown = millis();
        last = millis();
    } else if(their_activity != 0 && (activity == 0 || (their_activity == activity && their_dialogue == dialogue + 1))) {
        if(their_dialogue != activities[their_activity][their_variant].size() - 1) {
            thing = their_thing;
            activity = their_activity;
            variant = their_variant;
            dialogue = their_dialogue + 1;
            speaking = true;
            tts_cooldown = millis();
            tts_play(replace_percents(activities[activity][variant][dialogue], their_name).c_str());
            // if(dialogue == activities[activity][variant].size() - 1) activity = 0;
            last = millis();
        } else {
            activity = 0;
            block_until = millis() + 5000;
        }
    }
}

static uint64_t saturation_last_decreased = 0;

void state_loop() {
    std::lock_guard<std::mutex> guard(upd_mutex);
    bool changed = false;
    if(activity != 0 && millis() - last >= RESPONSE_TIMEOUT) {
        activity = 0;
        tts_stop();
        speaking = false;
        changed = true;
    }

    uint64_t now = millis();
    if(!check_can_upd(&changed)) return;

    if(now - saturation_last_decreased >= 8 * 60 * 1000) {
        if(saturation > 0) saturation--;
        saturation_last_decreased = now;
        changed = true;
    }

    if(changed) update();
}