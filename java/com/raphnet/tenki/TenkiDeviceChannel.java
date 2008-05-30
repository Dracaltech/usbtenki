package com.raphnet.tenki;

public class TenkiDeviceChannel
{
	private int _id;
	private String _name;
	private String _type_name;
	private int _chip_id;
	private TenkiChannelDataSource _dev;
	
	private boolean data_valid = false;

	public TenkiDeviceChannel(TenkiChannelDataSource dev, int id, String name, int chip_id, String type_name)
	{
		_dev = dev;
		_id = id;
		_name = name;
		_chip_id = chip_id;
		_type_name = type_name;
	}

	public void refresh() throws TenkiException
	{
		_dev.readChannel(this);
	}

	public float getValue() throws TenkiException
	{ 
		if (!data_valid)
			refresh();
		return _dev.getConvertedChannelValue(this);
	}

	public int getUnit() throws TenkiException
	{
		if (!data_valid)
			refresh();	
		return _dev.getConvertedChannelUnit(this);
	}

	public int getId() { return _id; }
	public String getName() { return _name; }
	public int getChipId() { return _chip_id; }
	public String getTypeName() { return _type_name; }
}


