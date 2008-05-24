package com.raphnet.tenki;


public class TenkiDeviceNotFoundException extends TenkiException
{
	public TenkiDeviceNotFoundException(String serial)
	{	
		super(serial);
	}
}
