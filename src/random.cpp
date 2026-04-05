#include "random.h"
#include "storage.h"

void random_init() {
    srand(storage().getUInt("seed"));
}
unsigned int random_get() {
    unsigned int n = (unsigned int)rand();
    storage().putUInt("seed", (unsigned int)rand());
    return n;
}