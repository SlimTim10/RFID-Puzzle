#include <RH_ASK.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include "hal.h"

#define DEBUG
#define die()	while (1)

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
#		ifdef DEBUG
		Serial.println("UID: ");
		nfc.PrintHex(uid.val, uid.length);
		Serial.println("");
#		endif
		
		success = auth_tag(&uid);
		if (success) {
#			ifdef DEBUG
			Serial.println("Sector 1 (blocks 4-7) has been authenticated");
#			endif
			
			uint8_t data[16];
			success = read_tag(data);
			if (success && data[0] == 0x01) {
#				ifdef DEBUG
				Serial.println("Reading block 4:");
				nfc.PrintHexChar(data, 16);
				Serial.println("");
#				endif
				
				radio.send((uint8_t *) msg, strlen(msg));
				radio.waitPacketSent();
				
#				ifdef DEBUG
				Serial.print("Sent: ");
				Serial.println(msg);
#				endif
			}
		}
	}
}

static bool find_tag(struct uid *uid) {
	uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid->val, &uid->length);
	return (success && uid->length == 4);
}

static bool auth_tag(struct uid *uid) {
	uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid->val, uid->length, 4, 0, keya);
	return success;
}

static bool read_tag(uint8_t *data) {
	uint8_t success = nfc.mifareclassic_ReadDataBlock(4, data);
	return success;
}
