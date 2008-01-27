#ifndef _sensirion_serial_h__
#define _sensirion_serial_h__

#define SSER_SCK_BIT	(1<<5)
#define SSER_SCK_PORT	PORTC
#define SSER_SCK_DDR	DDRC

#define SSER_DATA_BIT	(1<<4)
#define SSER_DATA_PORT	PORTC
#define SSER_DATA_DDR	DDRC
#define SSER_DATA_PIN	PINC

#define SHT_CMD_MEASURE_TEMPERATURE	0x03
#define SHT_CMD_MEASURE_HUMIDITY	0x05
#define SHT_CMD_READ_STATUS			0x07
#define SHT_CMD_WRITE_STATUS		0x06
#define SHT_CMD_SOFT_RESET			0x36


void sser_init(void);
int sser_cmd(unsigned char cmd);
int sser_readByte(unsigned char *dst, char skip_ack);
int sser_getWord(unsigned char cmd, unsigned char dst[2]);
int sser_writeByte(unsigned char dat);


#endif // _sensirion_serial_h__

