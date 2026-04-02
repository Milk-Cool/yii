#include <Arduino.h>
#include <BackgroundAudioSpeech.h>
#include <libespeak-ng/voice/en_us.h>

#ifdef ESP32
// #include <ESP32I2SAudio.h>
#else
#include <PWMAudio.h>
PWMAudio audio(2);
#endif
BackgroundAudioSpeech BMP(audio);

unsigned char cvoice[1024] = { 0 };

void setup() {
    Serial.begin(115200);
    Serial.println("i'm alive!!");
    memcpy(cvoice, voice_en_us.data, voice_en_us.len);
    strcat((char*)cvoice, "pitch 140 160\n");
    BackgroundAudioVoice vdata = {
        .name = voice_en_us.name,
        .len = strlen((char*)cvoice),
        .data = cvoice
    };
    BMP.setVoice(vdata);
    BMP.begin();
    BMP.setGain(1);
    BMP.setPitch(99);
}
void loop() {
    BMP.flush();
    BMP.speak("hello world");
    delay(5000);
    BMP.speak("my name is john");
    delay(5000);
}