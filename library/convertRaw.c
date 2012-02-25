#include <stdio.h>
#include <math.h>
#include "usbtenki.h"
#include "usbtenki_cmds.h"
#include "usbtenki_units.h"


int usbtenki_convertRaw(struct USBTenki_channel *chn, unsigned long flags)
{
	float temperature;
	int chip_fmt = TENKI_UNIT_KELVIN;
	unsigned char *raw_data;

	/*
	int i;
	printf("Raw data: ");
	for (i=0; i<chn->raw_length; i++) {
		int b;
		for (b=0x80; b; b>>=1) {
			printf("%c", raw_data[i] & b ? '1' : '0');
		}
		printf(" ");
	}
	printf("\n");*/

	raw_data = chn->raw_data;

	switch (chn->chip_id)
	{
		case USBTENKI_CHIP_MCP9800:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
				
				/* The sensor will be initailized in 12 bits mode  */
				t = ((raw_data[0] << 4) | (raw_data[1]>>4))<<4;
				t >>= 4;
				temperature = ((float)t) * pow(2.0,-4.0);
				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_ADT7410:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
			
				/* This sensor offers 16 bits of resolution.
				 *
				 * [0]                                 [1]
				 *   7   6   5   4   3   2   1   0      7   6    5   4   3   2   1   0
				 * D12 D11 D10  D9  D8  D7  D6  D5  |  D4  D3  D2   D1  D0   X   X   X
				 */
				
				t = raw_data[0] << 8 | raw_data[1];
				temperature = ((float)t) * pow(2.0,-7.0);
				chip_fmt = TENKI_UNIT_CELCIUS;

			}
			break;

		case USBTENKI_CHIP_SE95:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
			
				/* This sensor offers 13 bits of resolution as follows:
				 *
				 * [0]                                 [1]
				 *   7   6   5   4   3   2   1   0      7   6    5   4   3   2   1   0
				 * D12 D11 D10  D9  D8  D7  D6  D5  |  D4  D3  D2   D1  D0   X   X   X
				 */
				
				t = ((raw_data[0] << 5) | (raw_data[1]>>3))<<3;
				t >>= 3;
				temperature = ((float)t) * pow(2.0,-5.0);
				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_LM75:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
				
				/* The sensor only supports 9 bits */
				t = (raw_data[0] << 1) | (raw_data[1]>>7);
				temperature = ((float)t) * pow(2.0,-1.0);
				chip_fmt = TENKI_UNIT_CELCIUS;

			}
			break;

		case USBTENKI_CHIP_SHT_TEMP:
			{
				unsigned  short t;

				if (chn->raw_length!=2)
					goto wrongData;

				t = (raw_data[0]<<8) | raw_data[1];
			
				temperature = -40.0 + 0.01  * (float)t;

				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_SHT_RH:
			{
				// The following coefficients are from the 2011 Datasheet
				float c1 = -2.0468;
				float c2 = 0.0367;
				float c3 = -1.5955 * powf(10.0, -6.0);
				float sorh;

				if (flags & USBTENKI_FLAG_USE_OLD_SHT75_COMPENSATION) {
					// The following coefficients are from the 2007 Datasheet
					c1 = -4.0;
					c2 = 0.0405;
					c3 = -2.8 * powf(10.0, -6.0);
				}

				if (chn->raw_length!=2)
					goto wrongData;

				sorh = (float)( (unsigned short)((raw_data[0]<<8) | raw_data[1]) );
	
				// Keep the raw value around for the temperature compensated
				// humidity reading	
				chn->raw_value = sorh;
			
				temperature = c1 + c2*sorh + c3 * powf(sorh, 2.0);
				chip_fmt = TENKI_UNIT_RH;
			}
			break;


		case USBTENKI_CHIP_BS02_TEMP:
			{
				unsigned  short t;

				if (chn->raw_length!=2)
					goto wrongData;

				t = (raw_data[0]<<8) | raw_data[1];
			
				temperature = -40.0 + 0.04  * (float)t;

				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_BS02_RH:
			{
				float c1 = -4.0;
				float c2 = 0.648;
				float c3 = -7.2 * powf(10.0, -4.0);
				float sorh;

				if (chn->raw_length!=2)
					goto wrongData;

				sorh = (float)( (unsigned short)((raw_data[0]<<8) | raw_data[1]) );
			
				temperature = c1 + c2*sorh + c3 * powf(sorh, 2.0);
				chip_fmt = TENKI_UNIT_RH;
			}
			break;

		case USBTENKI_CHIP_MP3H6115A:
		case USBTENKI_CHIP_MPX4115:
			{
				float vout, vs, p;
				unsigned short adc_out;

				/* -- Sensor formulas:
				 *   "Vout = Vs * (.009 * P -0.095)"
				 *   where Vs is 5.1 Vdc. Output is ratiometric
				 *   to Vs between 4.85 and 5.35 volts.
				 *
				 * -- Atmel adc:
				 *   In 10 bit mode, 0x000 represents ground and 0x3ff represents
				 *   the selected reference voltage (VCC in our case) minus one
				 *   LSB.
				 *
				 * The code in the Atmel averages multiple samples and
				 * outputs a 16 bit value. 
				 * 
				 * The ADC reference voltage is the same as the sensor's Vs,
				 * So Vs does not really matter here.
				 */
				vs = 5.0;
				adc_out = raw_data[0] << 8 | raw_data[1];
				//vout = (adc_out * vs) / 1024.0;
				vout = (adc_out * vs) / (float)0xffff;
				p = ((vout/vs)+0.095)/.009;

				temperature = p;
				chip_fmt = TENKI_UNIT_KPA;
			}
			break;

		case USBTENKI_CHIP_TSL2561_IR_VISIBLE_16X:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2561_IR_16X:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2568_IR_VISIBLE_16X:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2568_IR_16X:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2561_IR_VISIBLE:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2561_IR:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2568_IR_VISIBLE:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TSL2568_IR:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_D6F_V03A1:
			{
				unsigned short adc_out;
				float vs;
				float voltage;
				float flow;
				
				/*the code in the atmel averages multiple samples and
				 * outputs a 16 bit value. 
				 * 
				 * the adc reference voltage is the same as the sensor's vs,
				 * so this is ok for ratiometric measurements.
				 */
				vs = 5.0;
				adc_out = raw_data[0] << 8 | raw_data[1];

				voltage = (adc_out * vs) / (float)0xffff;

				// The datasheet contains a curve and a table with 4 data points
				// Flow m/sec :  0      0.75    1.50    2.25    3.00
				// Voltage    :  0.50   0.70    1.11    1.58    2.00
				//
				// Note: All voltage +/- 0.15
				
				if (voltage < 0.70) {
					flow = 0 + (voltage - 0.5) / (0.70 - 0.50) * 0.75;
				} else if (voltage < 1.11) {
					flow = 0.75 + (voltage - 0.70) / (1.11 - 0.70) * 0.75;
				} else if (voltage < 1.58) {
					flow = 1.50 + (voltage - 1.11) / (1.58 - 1.11) * 0.75;
				} else { // voltage < 3 ?
					flow = 2.25 + (voltage - 1.58) / (2.00 - 1.58) * 0.75;
				}

				if (flow<0)
					flow = 0;
				if (flow>3.0) 
					flow = 3.0;

				temperature = flow;				
				chip_fmt = TENKI_UNIT_METER_SEC;
			}
			break;

		case USBTENKI_CHIP_VOLTS_REVERSE:
		case USBTENKI_CHIP_VOLTS:
			{
				unsigned short adc_out;
				float vs;
				
				/*the code in the atmel averages multiple samples and
				 * outputs a 16 bit value. 
				 * 
				 * the adc reference voltage is the same as the sensor's vs,
				 * so this is ok for ratiometric measurements.
				 */
				vs = 5.0;
				adc_out = raw_data[0] << 8 | raw_data[1];

				if (chn->chip_id==USBTENKI_CHIP_VOLTS_REVERSE)
					adc_out ^= 0xffff;

				temperature = (adc_out * vs) / (float)0xffff;
				chip_fmt = TENKI_UNIT_VOLTS;
			}
			break;

		case USBTENKI_MCU_ADC0:
		case USBTENKI_MCU_ADC1:
		case USBTENKI_MCU_ADC2:
		case USBTENKI_MCU_ADC3:
		case USBTENKI_MCU_ADC4:
		case USBTENKI_MCU_ADC5:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		case USBTENKI_CHIP_TACHOMETER:
			{
				float clk = 12000000 / 1024;
				unsigned short count = raw_data[0]<<8 | raw_data[1];

				temperature = 1 / ((1/clk) * count);

				chip_fmt = TENKI_UNIT_HZ;
				if (1)
				{
					int i;
					for (i=0; i<chn->raw_length; i++) {
						printf("%02X ", raw_data[i]);
					}
					printf("\n");
				}
			}
			break;

		default:
			temperature = raw_data[1] << 8 | raw_data[0];
			chip_fmt = TENKI_UNIT_RAW;
			
			printf("Unknown chip id 0x%02x\n", chn->chip_id);

			break;
	}

	chn->converted_data = temperature;
	chn->converted_unit = chip_fmt;

	return 0;

wrongData:
	fprintf(stderr, "Wrong data received\n");
	return -1;
}


