#include <RH_ASK.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <maybe.h>
#include "hal.h"

/* #define DEBUG */

#define die()	while (1)

struct uid {
	uint8_t val[7];
	uint8_t length;
};

enum constants {
	NUM_NFC = 2,
	NFC_INIT_ATTEMPTS = 3,
};

static const char *WIN_MESSAGE = "win";

static const uint8_t WIN_TAG_DATA[3] = { 0x01, 0x02, 0x03 };

static RH_ASK radio(2000, 0, RF_TX);

static PN532_SPI pn532spi_0(SPI, NFC_CS_0);
static PN532 nfc_0(pn532spi_0);
static PN532_SPI pn532spi_1(SPI, NFC_CS_1);
static PN532 nfc_1(pn532spi_1);
static PN532_SPI pn532spi_2(SPI, NFC_CS_2);
static PN532 nfc_2(pn532spi_2);

static PN532 *nfc[3] = { &nfc_0, &nfc_1, &nfc_2 };

static struct uid uid[3] = {
	{ .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7 },
	{ .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7 },
	{ .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7 },
};
static uint8_t buf[32];
static uint8_t tag_data[3];

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

	uint8_t i;
	for (i = 0; i < NUM_NFC; i++) {
		if (!init_nfc(nfc[i])) {
			Serial.print("Didn't find PN53x board: ");
			Serial.println(i);
			die();
		}
	}

#	ifdef DEBUG
	Serial.println("Ready");
#	endif
}

void loop(void) {
	maybe_do
		(find_tags,
		 read_tags,
		 handle_win);

	delay(500);
}

/* Finds Mifare Ultralight tag */
/* void -> maybe uid[] */
static maybe find_tags(void *empty_) {
	uint8_t success = true;
	uint8_t i;
	for (i = 0; i < NUM_NFC && success; i++) {
		nfc[i]->getFirmwareVersion(); /* Needed for successful consecutive readings */
		success = nfc[i]->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid[i].val, &uid[i].length);
		success &= (uid[i].length == 7);

#		ifdef DEBUG
		if (success) {
			Serial.println("UID: ");
			nfc[i]->PrintHex(uid[i].val, uid[i].length);
			Serial.println("");
		}
#		endif
	}

	if (success) {
		return mreturn(&uid);
	} else {
		return nothing();
	}
}

/* Reads Mifare Ultralight tag data to buffer */
/* void -> maybe uint8_t[3] */
static maybe read_tags(void *empty_) {
	uint8_t success = true;
	uint8_t i;
	for (i = 0; i < NUM_NFC && success; i++) {
		success = nfc[i]->mifareultralight_ReadPage(4, buf);
		if (success) {
			tag_data[i] = buf[0];
#			ifdef DEBUG
			Serial.println("Reading page 4:");
			nfc[i]->PrintHexChar(buf, 4);
			Serial.println("");
#			endif
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
static maybe handle_win(void *data_) {
	uint8_t *data = (uint8_t *) data_;
	if (data[0] == WIN_TAG_DATA[0]) {
		radio.send((uint8_t *) WIN_MESSAGE, strlen(WIN_MESSAGE));
		radio.waitPacketSent();
#	   	ifdef DEBUG
		Serial.print("Sent: ");
		Serial.println(WIN_MESSAGE);
#		endif
	}
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
