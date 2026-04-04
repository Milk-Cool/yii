Import("env")

from os.path import join, exists

PATH = join(".pio", "libdeps", "esp32dev", "BackgroundAudio", "src", "libespeak-ng", "synthesize.h")

def patch():
    if not exists(PATH):
        return
    f = open(PATH, "r")
    d = f.read()
    f.close()
    f = open(PATH, "w")
    f.write(d.replace("#define N_PHONEME_LIST 1000", "#define N_PHONEME_LIST 250"))
    f.close()

patch()