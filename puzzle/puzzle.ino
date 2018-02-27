#include <RH_ASK.h>
#include "hal.h"

#define DEBUG

#define die()	while (1)

const char *msg = "win";

RH_ASK radio(2000, NULL, RF_TX);

void setup(void) {
#	ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Initializing radio!");
#	endif

	if (!radio.init()) {
#		ifdef DEBUG
		Serial.println("Init failed");
#		endif
		die();
	}
}


void loop(void) {
	radio.send((uint8_t *) msg, strlen(msg));
	radio.waitPacketSent();
#	ifdef DEBUG
	Serial.print("Sent: ");
	Serial.println(msg);
#	endif

	delay(1000);
}
