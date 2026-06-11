#include <Arduino.h>
#include "bus.h"
#include "storage.h"
#include "blecomm.h"
#include "state.h"
#include "control.h"
#include "random.h"
#include "tts.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>

WebServer server(80);
IPAddress ip_self(192, 168, 121, 1);
IPAddress ip_mask(255, 255, 255, 0);
DNSServer dnsServer;

static uint8_t mode;
static bool setup_write = false;

extern const uint8_t config_html_start[] asm("_binary_res_config_html_start");

String redirect_from[] = {
    "/",
    "/generate_204",
    "/fwlink",
    "/check_network_status.txt",
    "/ncsi.txt",
    "/connecttest.txt",
    "/redirect",
    "/hotspot-detect.html",
    "/library/test/success.html",
    "/success.txt",
    "/canonical.html",
};
String not_found_from[] = {
    "/wpad.dat",
    "/favicon.ico",
    "/chat/favicon.ico",
};

void setup() {
    Serial.begin(115200);
    Serial.println("i'm alive!!");
    storage_init();
    mode = storage().getUChar("mode", 0); // 0 - setup, 1 - control, 2 - doll
    if(mode == 2) {
        tts_init();
        random_init();
        ble_init();
        state_init();
        ble_set_cb(state_handle_advertisement);

        tts_play("ah!");
    } else if(mode == 1) {
        ble_init();
        control_init();
        ble_set_cb(control_handle_advertisement);
    } else if(mode == 0) {
        pinMode(2, OUTPUT);
        digitalWrite(2, HIGH);

        if(storage().getString("password") != "") {
            WiFi.softAPConfig(ip_self, ip_self, ip_mask);
            WiFi.softAP("config - " + storage().getString("name", "unnamed"), storage().getString("password"));
            IPAddress ip = WiFi.softAPIP();
            dnsServer.start(53, "*", ip);

            server.on("/config", HTTP_GET, []() {
                if(server.hasArg("get")) {
                    JsonDocument doc;
                    
                    doc["name"] = storage().getString("name");
                    doc["pitch_base"] = storage().getUChar("pitch_base");
                    doc["pitch_range"] = storage().getUChar("pitch_range");
                    doc["flutter"] = storage().getUChar("flutter");
                    doc["speed"] = storage().getUChar("speed");
                    doc["tone1"] = storage().getUChar("tone1");
                    doc["tone2"] = storage().getUChar("tone2");
                    doc["tone3"] = storage().getUChar("tone3");
                    doc["gain"] = storage().getUChar("gain");

                    String out;
                    serializeJson(doc, out);
                    server.send(200, "text/plain", out);
                }
                server.send(200, "text/html", (const char*)config_html_start);
            });

            server.on("/config", HTTP_POST, []() {
                String t = server.arg("t");
                if(t == "control") {
                    storage().putUChar("mode", 1);
                    ESP.restart();
                } else if(t == "doll") {
                    storage().putUChar("mode", 2);
                    ESP.restart();
                } else if(t == "rmet") {
                    storage().remove("met");
                    ESP.restart();
                }
                String name = server.arg("name");
                String value = server.arg("value");
                if(!t || !name || (t != "reset" && !value)) {
                    server.send(400, "text/plain", "bad request");
                }
                if(t == "string") storage().putString(name.c_str(), value);
                else if(t == "u8") storage().putUChar(name.c_str(), value.toInt());
                else if(t == "u16") storage().putUShort(name.c_str(), value.toInt());
                else if(t == "u32") storage().putUInt(name.c_str(), value.toInt());
                else if(t == "reset") storage().remove(name.c_str());
                server.send(200, "text/html", (const char*)config_html_start);
            });

            for(String path : redirect_from) {
                server.on(path, HTTP_GET, []() {
                    server.sendHeader("Location", "/config", true);
                    server.send(302, "text/plain", "");
                });
            }
            for(String path : not_found_from)
                server.on(path, HTTP_GET, []() { server.send(404); });
            server.onNotFound([]() {
                server.sendHeader("Location", "/config", true);
                server.send(302, "text/plain", "");
            });
            
            ElegantOTA.begin(&server);
            server.begin();
        }
    }
}

// potential loss with consecutive packets but I DON'T CARE !!
static std::vector<uint8_t> data;
static BusData out_data;
static bool get_serial_data() {
    while(Serial.available()) {
        int b = Serial.read();
        if(b == -1) continue;
        data.push_back(b);
    }
    if(bus_is_complete(data)) {
        out_data = bus_parse(data);
        return true;
    }
    return false;
}
static void reset_serial_data() {
    data.erase(data.begin(), data.end());
    out_data.erase(out_data.begin(), out_data.end());
}

static uint64_t boot_held_since;
static bool boot_held = false;
void loop() {
    if(mode == 0) {
        if(get_serial_data()) {
            BusData out;
            for(BusEntry& ent : out_data) {
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
            reset_serial_data();
        }
        dnsServer.processNextRequest();
        server.handleClient();
        ElegantOTA.loop();
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

    if(mode == 1) control_loop();
    else state_loop();
}