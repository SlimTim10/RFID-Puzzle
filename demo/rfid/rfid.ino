#include <RH_ASK.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <maybe.h>
#include "hal.h"

#define DEBUG
#include "debug.h"

#define die()	while (1)

static const uint8_t DEVICE_ID = 0x01;

struct uid {
	uint8_t val[7];
	uint8_t length;
};

enum settings {
	NUM_NFC = 1,
	NFC_INIT_ATTEMPTS = 3,
	NFC_FIND_ATTEMPTS = 3,
	MSG_REPEAT = 1,
	READ_DELAY = 2000,		/* ms after reading */
	PAYLOAD_LEN = 8,
};

static RH_ASK radio(2000, 0, RF_TX);

static PN532_SPI pn532spi_0(SPI, NFC_CS_0);
static PN532 nfc_0(pn532spi_0);

static PN532 *nfc[NUM_NFC] = { &nfc_0 };

static struct uid uid[NUM_NFC] = {
	{ .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7 },
};
static uint8_t nfc_buf[32];
static uint8_t tag_data[NUM_NFC];
static char payload[PAYLOAD_LEN];

void setup(void) {
	hal_setup();

	dbg_begin();

	dbg_println("Initializing radio");

	if (!radio.init()) {
		dbg_println("Radio initialization failed");
		die();
	}

	dbg_println("Initializing NFC boards");
	uint8_t i;
	for (i = 0; i < NUM_NFC; i++) {
		if (init_nfc(nfc[i])) {
			dbg_print("Found board #"); dbg_println(i);
		} else {
			dbg_print("Didn't find PN53x board #"); dbg_println(i);
			die();
		}
	}

	dbg_println("Ready!");
}

void loop(void) {
	maybe_do
		(find_tags,
		 read_tags,
		 send_data);

	delay(500);
}

static bool nfc_read_passive_target(uint8_t x) {
	bool found = false;
	uint8_t j;
	for (j = 0; j < NFC_FIND_ATTEMPTS && !found; j++) {
		found = nfc[x]->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid[x].val, &uid[x].length);
	}
	return found;
}

/* Finds Mifare Ultralight tag */
/* void -> maybe uid[] */
static maybe find_tags(void *empty_) {
	bool success = true;
	uint8_t i;
	for (i = 0; i < NUM_NFC && success; i++) {
		success = nfc_read_passive_target(i);
		success &= (uid[i].length == 7);

		if (success) {
			dbg_println("UID: ");
#			ifdef DEBUG
			nfc[i]->PrintHex(uid[i].val, uid[i].length);
#			endif
			dbg_println("");
		}
	}

	if (success) {
		return mreturn(&uid);
	} else {
		return nothing();
	}
}

/* Reads Mifare Ultralight tag data to buffer */
/* void -> maybe uint8_t[NUM_NFC] */
static maybe read_tags(void *empty_) {
	bool success = true;
	uint8_t i;
	for (i = 0; i < NUM_NFC && success; i++) {
		success = nfc[i]->mifareultralight_ReadPage(4, nfc_buf);
		if (success) {
			tag_data[i] = nfc_buf[0];
			dbg_println("Reading page 4:");
#			ifdef DEBUG
			nfc[i]->PrintHexChar(nfc_buf, 4);
#			endif
			dbg_println("");
		}
	}

	if (success) {
		return mreturn(&tag_data);
	} else {
		return nothing();
	}
}

/* Handles the winning condition if it is met */
/* uint8_t[] -> maybe void */
static maybe send_data(void *data_) {
	uint8_t *data = (uint8_t *) data_;

	uint8_t tag_id = data[0];
	
	payload[0] = DEVICE_ID;
	payload[1] = tag_id;
	payload[2] = 0x00;

	radio.setModeTx();
	radio.send((uint8_t *) payload, strlen(payload));
	radio.waitPacketSent();
	dbg_print("Sent: "); dbg_println(payload);

	delay(READ_DELAY);
	
	return nothing();
}

/* Initializes NFC board */
static bool init_nfc(PN532 *x) {
	uint32_t versiondata;
	versiondata = false;
	uint8_t i;
	for (i = 0; i < NFC_INIT_ATTEMPTS && !versiondata; i++) {
		x->begin();
		versiondata = x->getFirmwareVersion();
	}
	if (!versiondata) {
		return false;
	}

	x->SAMConfig();
	
	return true;
}
