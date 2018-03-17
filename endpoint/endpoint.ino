#include <RH_ASK.h>
#include <Servo.h>
#include <maybe.h>
#include "hal.h"

/* #define DEBUG */

#define die()	while (1)

const char *WIN_MESSAGE = "win";

static RH_ASK radio(2000, RF_RX, 0);
static uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
static uint8_t buflen = sizeof(buf);
static Servo servo;

void setup(void) {
#	ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Initializing radio");
#	endif

	if (!radio.init()) {
#		ifdef DEBUG
		Serial.println("Init failed");
#		endif
		die();
	}

	servo.attach(SERVO);
	sweep_right();

#	ifdef DEBUG
	Serial.println("Ready");
#	endif
}

void loop(void) {
	maybe_do
		(receive_msg,
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
#		ifdef DEBUG
		Serial.println("Winner!");
#		endif
		sweep_left();
		delay(1000);
		sweep_right();
	}
	return nothing();
}

static void sweep_left(void) {
	servo.write(180);
}

static void sweep_right(void) {
	servo.write(90);
}
