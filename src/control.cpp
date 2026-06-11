#include "control.h"
#include "blecomm.h"
#include <string.h>
#include <ArduinoJson.h>

void control_handle_advertisement(const uint8_t* addr, std::string data, int8_t rssi) {
    uint8_t mac[6];
    for(uint8_t i = 0; i < 6; i++)
        mac[i] = addr[5 - i];
    
    char their_name[13] = { 0 };
    memcpy(their_name, data.data(), 12);

    uint8_t their_level = data[15];
    uint8_t their_xp = (data[16] >> 4) & 0xf;
    uint8_t their_saturation = data[16] & 0xf;

    JsonDocument doc;
    for(uint8_t i = 0; i < 6; i++)
        doc["mac"][i] = mac[i];
    doc["name"] = their_name;
    doc["level"] = their_level;
    doc["xp"] = their_xp;
    doc["saturation"] = their_saturation;

    String out; serializeJson(doc, out);
    Serial.println(out);
}

JsonDocument rx_doc;
std::vector<uint8_t> advertisement;

static uint16_t n;
static String buf;
void control_init() {
    for(char c : String("!contr"))
        advertisement.push_back(c);
    for(uint8_t i = 0; i < 9; i++)
        advertisement.push_back(0);
    ble_set_data(advertisement, false);
}
void control_loop() {
    while(Serial.available()) {
        char c = Serial.read();
        if(c == '\r') continue;
        if(c == '\n') {
            if(buf[0] == '{') {
                DeserializationError err = deserializeJson(rx_doc, buf);
                if(!err && rx_doc.containsKey("mac") && rx_doc.containsKey("action")) {
                    advertisement.clear();

                    for(char c : String("!contr"))
                        advertisement.push_back(c);
                    for(uint8_t i = 0; i < 6; i++) {
                        uint8_t b = rx_doc["mac"][i];
                        advertisement.push_back(b);
                    }

                    uint8_t action = rx_doc["action"];
                    advertisement.push_back(action);

                    advertisement.push_back(n >> 8);
                    advertisement.push_back(n & 0xff);
                    n++;

                    ble_set_data(advertisement, false);
                }
            }
            buf = "";
        } else buf += c;
    }
}