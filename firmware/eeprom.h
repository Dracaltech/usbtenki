#ifndef _eeprom_h__
#define _eeprom_h__

struct eeprom_data_struct {
	unsigned char magic[7]; /* 'TenkiCfg' */
	unsigned char serial[6];
	unsigned char adc_chips[6];
};

extern struct eeprom_data_struct g_eeprom_data;
void eeprom_commit(void);
void eeprom_init(void);

#endif // _eeprom_h__

