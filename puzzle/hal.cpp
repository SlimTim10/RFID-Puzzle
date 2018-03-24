#include "hal.h"
#include <Arduino.h>

void hal_setup(void) {
	pinMode(NFC_CS_0, OUTPUT);
	pinMode(NFC_CS_1, OUTPUT);
	pinMode(NFC_CS_2, OUTPUT);
	digitalWrite(NFC_CS_0, HIGH);
	digitalWrite(NFC_CS_1, HIGH);
	digitalWrite(NFC_CS_2, HIGH);
}
