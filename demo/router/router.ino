#include <RH_ASK.h>
#include "hal.h"

#define die()	while (1)

enum settings {
	PAYLOAD_LEN = 8,
	SERIAL_BUF_LEN = 8,
	SERIAL_BAUD = 115200,
};

static RH_ASK radio(2000, RF_RX, RF_TX);

static char payload[PAYLOAD_LEN];
static uint8_t payload_len = sizeof(payload);
static char serial_buf[SERIAL_BUF_LEN];
static boolean new_data;

void setup() {
	Serial.begin(115200);

	if (!radio.init()) {
		Serial.println("Radio initialization failed");
		die();
	}
	
	new_data = false;

	Serial.println("Ready!");
}

void loop() {
	if (Serial.available()) {
		recv_with_end_marker();
		send_new_data();
	}
	radio.setModeRx();
	if (radio.recv(payload, &payload_len)) {
		memcpy(serial_buf, payload, SERIAL_BUF_LEN);
		Serial.println(serial_buf);
	}
}

void recv_with_end_marker() {
	static byte idx = 0;
	char end_marker = '\n';
	char rc;
   
	while (Serial.available() > 0 && new_data == false) {
		rc = Serial.read();

		if (rc != end_marker) {
			serial_buf[idx] = rc;
			idx++;
			if (idx >= SERIAL_BUF_LEN) {
				idx = SERIAL_BUF_LEN - 1;
			}
		} else {
			serial_buf[idx] = '\0';
			idx = 0;
			new_data = true;
		}
	}
}

void send_new_data() {
	if (new_data == true) {
		memcpy(payload, serial_buf, PAYLOAD_LEN);
		
		/* Serial.print("Sending: "); Serial.println(payload); */
		
		radio.setModeTx();
		radio.send((uint8_t *) payload, strlen(payload));
		radio.waitPacketSent();
		new_data = false;
	}
}
