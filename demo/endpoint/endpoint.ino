#include <RH_ASK.h>
#include <Servo.h>
#include <maybe.h>
#include "hal.h"

#define DEBUG
#include "debug.h"

#define die()	while (1)

static const uint8_t DEVICE_ID = 0x02;

static const char WIN_MESSAGE[] = { DEVICE_ID, 0x01 };

static RH_ASK radio(2000, RF_RX, 0);
static uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
static uint8_t buflen = sizeof(buf);
static Servo servo;

void setup(void) {
	hal_setup();

	dbg_begin();

	dbg_println("Initializing radio");

	if (!radio.init()) {
		dbg_println("Radio initialization failed");
		die();
	}

	servo.attach(SERVO);
	sweep_right();

	dbg_println("Ready!");
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
		dbg_println("Winner!");
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
