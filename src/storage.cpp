#include "storage.h"

static Preferences prefs;

void storage_init() {
    prefs.begin("yii");
}
Preferences& storage() {
    return prefs;
}
