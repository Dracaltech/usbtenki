#include <avr/eeprom.h>
#include <string.h>
#include "eeprom.h"
#include "../common/usbtenki_cmds.h"

struct eeprom_data_struct g_eeprom_data;

void eeprom_commit(void)
{
	eeprom_write_block(&g_eeprom_data, (void*)0x00, sizeof(struct eeprom_data_struct));
}

void eeprom_init(void)
{
	char *magic = "TenkiCfg";
	eeprom_read_block(&g_eeprom_data, (void*)0x00, sizeof(struct eeprom_data_struct));

	/* Check for magic number */
	if (memcmp(g_eeprom_data.magic, magic, EEPROM_MAGIC_SIZE)) {
		memcpy(g_eeprom_data.magic, magic, EEPROM_MAGIC_SIZE);
		memset(g_eeprom_data.serial, '?', EEPROM_SERIAL_SIZE);
		memset(g_eeprom_data.adc_chips, USBTENKI_CHIP_NONE, 
										EEPROM_ADC_CHIPS_SIZE);

		eeprom_commit();
	}
}

