#include "random.h"
#include "storage.h"

// remove me maybe?
void random_init() {
}
unsigned int random_get() {
    return esp_random();
}