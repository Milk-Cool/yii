#include "tts.h"
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
    strcat((char*)cvoice, "pitch 80 110\n");
    BackgroundAudioVoice vdata = {
        .name = voice_en_us.name,
        .len = strlen((char*)cvoice),
        .data = cvoice
    };
    BMP->setVoice(vdata);
    BMP->begin();
    BMP->setGain(1);
    BMP->setPitch(99);
}
bool tts_done() {
    return BMP->done();
}
void tts_play(const char* text) {
    tts_stop();
    BMP->speak(text);
}
void tts_stop() {
    BMP->flush();
}