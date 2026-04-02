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

void setup() {
    Serial.begin(115200);
    Serial.println("i'm alive!!");
    BMP.setVoice(voice_en_us);
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