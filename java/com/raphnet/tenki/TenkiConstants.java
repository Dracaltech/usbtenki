package com.raphnet.tenki;

public class TenkiConstants
{
	/* Units from usbtenki.h */
	public static final int UNIT_RAW 	= 		0x00;
	
	public static final int UNIT_CELCIUS	=	0x01;
	public static final int UNIT_FAHRENHEIT	= 	0x02;
	public static final int UNIT_KELVIN		=	0x03;
	public static final int UNIT_RH			=	0x04; // relative humidity

	public static final int UNIT_KPA 		= 	0x10;
	public static final int UNIT_HPA 		= 	0x11;
	public static final int UNIT_BAR		=	0x12;
	public static final int UNIT_AT			=	0x13;
	public static final int UNIT_ATM		=	0x14;
	public static final int UNIT_TORR		=	0x15;
	public static final int UNIT_PSI		=	0x16;

	public static final int UNIT_VOLTS		=	0x20;

	public static final int UNIT_LUX		=	0x30;

	public static String unitToString(int unit)
	{
		String libname = JNI.unitToName(unit);

		if (libname == null) {
			return "Unknown unit";
		}

		return libname;
	}

	
	public static final int CHIP_MCP9800	=	0x00;
	public static final int CHIP_LM75		=	0x01;
	public static final int CHIP_LM92		=	0x02;
	public static final int CHIP_SHT_TEMP	=	0x03;
	public static final int CHIP_SHT_RH		=	0x04;
	public static final int CHIP_TSL2561_IR_VISIBLE	=	0x05;
	public static final int CHIP_TSL2561_IR	=	0x06;
	public static final int CHIP_BS02_TEMP	=	0x07;
	public static final int CHIP_BS02_RH	=	0x08;

	public static final int CHIP_MCU_ADC0	=	0x80;
	public static final int CHIP_MCU_ADC1	=	0x81;
	public static final int CHIP_MCU_ADC2	=	0x82;
	public static final int CHIP_MCU_ADC3	=	0x83;
	public static final int CHIP_MCU_ADC4	=	0x84;
	public static final int CHIP_MCU_ADC5	=	0x85;

	public static final int CHIP_MPX4115	=	0x90;

	public static final int CHIP_VOLTS		=	0x91;
	public static final int CHIP_VOLTS_REVERSE	=	0x92;

	public static final int CHIP_NONE		=	0xff;

	public static String chipToString(int chip)
	{
		String libname = JNI.chipToName(chip);

		if (libname == null) {
			return "Unknown unit";
		}

		return libname;
	}


}
