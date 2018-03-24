#ifndef HAL_H
#define HAL_H

enum pins {
	RF_TX = 2,
	NFC_CS_0 = 8,
	NFC_CS_1 = 9,
	NFC_CS_2 = 10,
};

void hal_setup(void);

#endif
