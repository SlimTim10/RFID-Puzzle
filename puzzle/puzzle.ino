#include <RH_ASK.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include "hal.h"
#include "maybe.h"

#define DEBUG
#define die()	while (1)

enum constants {
	WIN_TAG_DATA = 0x01,
};

struct uid {
	uint8_t val[7];
	uint8_t length;
};

const char *msg = "win";

static RH_ASK radio(2000, 0, RF_TX);
static PN532_SPI pn532spi(SPI, NFC_CS);
static PN532 nfc(pn532spi);
struct uid uid = { .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7};
static uint8_t data[32];

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

	nfc.begin();
	uint32_t versiondata = nfc.getFirmwareVersion();
	if (!versiondata) {
		Serial.println("Didn't find PN53x board");
		die();
	}

	nfc.SAMConfig();

#	ifdef DEBUG
	Serial.println("Ready");
#	endif
}

void loop(void) {
	maybe_do
		(mstart,
		 find_tag,
		 read_tag,
		 handle_win);
	
	delay(500);
}

/* Finds Mifare Ultralight tag */
/* void -> maybe uid */
static maybe find_tag(void *empty_) {
	uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid.val, &uid.length);

#	ifdef DEBUG
	if (success) {
		Serial.println("UID: ");
		nfc.PrintHex(uid.val, uid.length);
		Serial.println("");
	}
#	endif

	if (success && uid.length == 7) {
		return mreturn(&uid);
	} else {
		return nothing();
	}
}

/* Reads Mifare Ultralight tag data to buffer */
/* void -> maybe uint8_t[] */
static maybe read_tag(void *empty_) {
	uint8_t success = nfc.mifareultralight_ReadPage(4, data);

#	ifdef DEBUG
	if (success) {
		Serial.println("Reading page 4:");
		nfc.PrintHexChar(data, 4);
		Serial.println("");
	}
#	endif

	if (success) {
		return mreturn(&data);
	} else {
		return nothing();
	}
}

/* Handles the winning condition if it is met */
/* uint8_t[] -> maybe void */
static maybe handle_win(void *data_) {
	uint8_t *data = (uint8_t *) data_;
	if (data[0] == WIN_TAG_DATA) {
		radio.send((uint8_t *) msg, strlen(msg));
		radio.waitPacketSent();
#	   	ifdef DEBUG
		Serial.print("Sent: ");
		Serial.println(msg);
#		endif
	}
	return nothing();
}
