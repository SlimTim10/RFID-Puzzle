#include <RH_ASK.h>
#include <maybe.h>
#include "hal.h"

/* #define DEBUG */

#define die()	while (1)

const char *WIN_MESSAGE = "win";

static RH_ASK radio(2000, RF_RX, 0);
static uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
static uint8_t buflen = sizeof(buf);

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
	maybe_do
		(mstart,
		 receive_msg,
		 handle_win);
}

/* Receives a radio message */
/* void -> maybe uint8_t[] */
static maybe receive_msg(void *empty_) {
	if (radio.recv(buf, &buflen)) {
#		ifdef DEBUG
		radio.printBuffer("Received:", buf, buflen);
#		endif
		return mreturn(buf);
	} else {
		return nothing();
	}
}

/* Handles the winning condition if it is met */
/* uint8_t[] -> maybe void */
static maybe handle_win(void *buf_) {
	uint8_t *msg = (uint8_t *) buf_;
	if (strncmp((const char *) msg, WIN_MESSAGE, strlen(WIN_MESSAGE)) == 0) {
		led_on();
		delay(1000);
		led_off();
	}
	return nothing();
}

static void led_off(void) {
	digitalWrite(LED, LOW);
}

static void led_on(void) {
	digitalWrite(LED, HIGH);
}
