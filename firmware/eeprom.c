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
	if (memcmp(g_eeprom_data.magic, magic, 7)) {
		memcpy(g_eeprom_data.magic, magic, 7);
		memset(g_eeprom_data.serial, '?', sizeof(g_eeprom_data.serial));
		memset(g_eeprom_data.adc_chips, USBTENKI_CHIP_HIDDEN, 
												sizeof(g_eeprom_data.adc_chips));

		eeprom_commit();
	}
}

