#include <RH_ASK.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include "hal.h"

/* #define DEBUG */
#define die()	while (1)

enum constants {
	WIN_TAG_DATA = 0x01,
};

struct uid {
	uint8_t val[7];
	uint8_t length;
};

const char *msg = "win";

RH_ASK radio(2000, 0, RF_TX);
PN532_SPI pn532spi(SPI, NFC_CS);
PN532 nfc(pn532spi);

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
	struct uid uid = { .val = {0, 0, 0, 0, 0, 0, 0}, .length = 7};
	bool success = find_tag(&uid);
	if (success) {
		uint8_t data[32];
		success = read_tag(data);
		if (success && data[0] == WIN_TAG_DATA) {
			radio.send((uint8_t *) msg, strlen(msg));
			radio.waitPacketSent();
#			ifdef DEBUG
			Serial.print("Sent: ");
			Serial.println(msg);
#			endif
		}
	}

	delay(500);
}

/* Finds Mifare Ultralight tag */
static bool find_tag(struct uid *uid) {
	uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid->val, &uid->length);

#	ifdef DEBUG
	if (success) {
		Serial.println("UID: ");
		nfc.PrintHex(uid->val, uid->length);
		Serial.println("");
	}
#	endif

	return (success && uid->length == 7);
}

/* Reads Mifare Ultralight tag data to buffer */
static bool read_tag(uint8_t *data) {
	uint8_t success = nfc.mifareultralight_ReadPage (4, data);

#	ifdef DEBUG
	if (success) {
		Serial.println("Reading page 4:");
		nfc.PrintHexChar(data, 4);
		Serial.println("");
	}
#	endif

	return success;
}
