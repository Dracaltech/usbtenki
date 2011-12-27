/*   USBTenki - Interfacing sensors to USB 
 *   Copyright (C) 2007-2011  Raphaël Assénat <raph@raphnet.net>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <avr/io.h>
#include "sser.h"

void dly() __attribute__ ((noinline));

void dly()
{
	// 1 us
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
	asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
}

void sser_init(void)
{
	/* Clock output, normally low */
	SSER_SCK_DDR |= SSER_SCK_BIT;
	SSER_SCK_PORT &= ~SSER_SCK_BIT;

	/* Clock as input, with pullup */
	SSER_DATA_DDR &= ~SSER_DATA_BIT;
	SSER_DATA_PORT |= SSER_DATA_BIT;
}

static void pullData()
{
	/* Disable pullup first in order to pull
	 * when we change the direction to output */
	SSER_DATA_PORT &= ~SSER_DATA_BIT;
	SSER_DATA_DDR |= SSER_DATA_BIT;
}

static void releaseData()
{
	SSER_DATA_DDR &= ~SSER_DATA_BIT;
	SSER_DATA_PORT |= SSER_DATA_BIT;
}

static void clockHigh()
{
	SSER_SCK_PORT |= SSER_SCK_BIT;
}

static void clockLow()
{
	SSER_SCK_PORT &= ~SSER_SCK_BIT;
}

int sser_cmd(unsigned char cmd)
{
	char i;
	char ack;

	/* Transmission Start */
	clockHigh(); 
	dly();
	pullData(); 
	dly();
	clockLow();
	dly();
	clockHigh();	
	dly();
	releaseData();
	dly();
	clockLow();
	dly();

	/* 3 address bits + 5 command bits */
	for (i=0; i<8; i++)
	{
		if (cmd & 0x80)
			releaseData();
		else
			pullData();
		
		dly();
		clockHigh();
		dly();
		clockLow();

		cmd <<= 1;
	}
	releaseData();

	/* ack */	
	dly();
	clockHigh();
	dly();
	ack = SSER_DATA_PIN & SSER_DATA_BIT;
	clockLow();
	
	if (ack) {
		return -1; // no ack!
	}

	// let the slave relase data
	while (!(SSER_DATA_PIN & SSER_DATA_BIT)) 
		{ /* empty */	}

	// clk low
	// data floating

	return 0;
}

/**
 * NOT FINISHED NOR TESTED
 */
int sser_writeByte(unsigned char dat)
{
	char ack, i;

	dly();
	/* 3 address bits + 5 command bits */
	for (i=0; i<8; i++)
	{
		if (dat & 0x80)
			releaseData();
		else
			pullData();
		
		dly();
		clockHigh();
		dly();
		clockLow();
		dly();

		dat <<= 1;
	}
	releaseData();

	/* ack */	
	dly();
	clockHigh();
	dly();
	ack = SSER_DATA_PIN & SSER_DATA_BIT;
	clockLow();
	
	if (ack) {
		return -1; // no ack!
	}

	// let the slave relase data
	while (!(SSER_DATA_PIN & SSER_DATA_BIT)) 
		{ /* empty */	}

	return 0;
}

/** \brief Read and ack a byte from the sensor
 *
 * Note: This should be called after the transmission
 * start, address and commands are sent and after the slave
 * has pulled data low again indicating that the conversion
 * is completed. */
int sser_readByte(unsigned char *dst, char skip_ack)
{
	unsigned char tmp;
	int i;

	for (tmp=0,i=0; i<8; i++) {
		dly();
		clockHigh();
		dly();
		tmp <<= 1;
		if (SSER_DATA_PIN & SSER_DATA_BIT) {
			tmp |= 1;
		} else {
			// tmp &= ~1;
		}
		clockLow();
	}
	*dst = tmp;
	
	/* Ack the byte by pulling data low during a 9th clock cycle */
	if (!skip_ack)
		pullData();
	dly();
	clockHigh();
	dly();
	clockLow();
	releaseData();
	dly();

	return 0;
}

int sser_getWord(unsigned char cmd, unsigned char dst[2])
{
	if (sser_cmd(cmd))
		return -1;
	
	/* The slave pulls data low when conversion is done */
	while ((SSER_DATA_PIN & SSER_DATA_BIT)) 
		{ /* empty */	}

	sser_readByte(&dst[0], 0);
	sser_readByte(&dst[1], 1);
//	sser_readByte(&crc, 1);

	return 0;
}




