#include "hal.h"
#include <Arduino.h>

void hal_setup(void) {
	pinMode(NFC_CS_0, OUTPUT);
	digitalWrite(NFC_CS_0, HIGH);
}
