#include "tts.h"
#include "storage.h"
#include <BackgroundAudioSpeech.h>
#include <ESP32I2SAudio.h>
#include <libespeak-ng/voice/en_us.h>

static unsigned char* cvoice;
static ESP32I2SAudio* audio;
static BackgroundAudioSpeech* BMP;
void tts_init() {
    audio = new ESP32I2SAudio(4, 5, 27);
    BMP = new BackgroundAudioSpeech(*audio);
    cvoice = (unsigned char*) malloc(512);
    memset(cvoice, 0, 512);
    memcpy(cvoice, voice_en_us.data, voice_en_us.len);
    strcat((char*)cvoice, (String("pitch ") + storage().getUChar("pitch_base", 82) + " " + storage().getUChar("pitch_range", 118) + "\n").c_str());
    strcat((char*)cvoice, (String("flutter ") + storage().getUChar("flutter", 0) + "\n").c_str());
    strcat((char*)cvoice, (String("speed ") + storage().getUChar("speed", 100) + "\n").c_str());
    float tone1 = ((float)storage().getUChar("tone1", 128) - 128) / 128;
    float tone2 = ((float)storage().getUChar("tone2", 128) - 128) / 128;
    float tone3 = ((float)storage().getUChar("tone3", 128) - 128) / 128;
    strcat((char*)cvoice, (String("tone 600 ") + (170 + tone1 * 20) + " 1200 " + (135 + tone2 * 20) + " 2000 " + (110 + tone3 * 20) + "\n").c_str());
    BackgroundAudioVoice vdata = {
        .name = voice_en_us.name,
        .len = strlen((char*)cvoice),
        .data = cvoice
    };
    BMP->setVoice(vdata);
    BMP->begin();
    BMP->setGain((float)storage().getUChar("gain", 50) / 100);
    BMP->setPitch(99);
}
bool tts_done() {
    return BMP->done();
}
void tts_play(const char* text) {
    Serial.println(String("speaking: ") + text);
    tts_stop();
    BMP->speak(text);
}
void tts_stop() {
    BMP->flush();
}