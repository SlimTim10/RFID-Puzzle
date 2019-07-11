#ifndef HAL_H
#define HAL_H

enum pins {
	RF_TX = 2,
	NFC_CS_0 = 10,
};

void hal_setup(void);

#endif
