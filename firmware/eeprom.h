#ifndef _eeprom_h__
#define _eeprom_h__

#define EEPROM_MAGIC_SIZE		7
#define EEPROM_SERIAL_SIZE		6
#define EEPROM_ADC_CHIPS_SIZE	8

struct eeprom_data_struct {
	unsigned char magic[EEPROM_MAGIC_SIZE]; /* 'TenkiCfg' */
	unsigned char serial[EEPROM_SERIAL_SIZE];
	unsigned char adc_chips[EEPROM_ADC_CHIPS_SIZE];
};

extern struct eeprom_data_struct g_eeprom_data;
void eeprom_commit(void);
void eeprom_init(void);

#endif // _eeprom_h__

