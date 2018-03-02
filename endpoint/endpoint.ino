#include <RH_ASK.h>
#include "hal.h"

/* #define DEBUG */

#define die()	while (1)

const char *msg = "win";

RH_ASK radio(2000, RF_RX, 0);

void setup(void) {
#	ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Initializing radio");
#	endif

	pinMode(LED, OUTPUT);

	led_off();

	if (!radio.init()) {
#		ifdef DEBUG
		Serial.println("Init failed");
#		endif
		die();
	}
}

void loop(void) {
	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
	uint8_t buflen = sizeof(buf);

	if (radio.recv(buf, &buflen)) {
#		ifdef DEBUG
		radio.printBuffer("Received:", buf, buflen);
#		endif
		if (strncmp((const char *) buf, msg, strlen(msg)) == 0) {
			led_on();
			delay(1000);
			led_off();
		}
	}

	
}

static void led_off(void) {
	digitalWrite(LED, LOW);
}

static void led_on(void) {
	digitalWrite(LED, HIGH);
}
