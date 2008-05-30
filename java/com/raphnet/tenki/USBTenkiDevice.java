package com.raphnet.tenki;


public class USBTenkiDevice implements TenkiDevice, TenkiChannelDataSource
{
	private long _native_pointer;
	private String _serial;
	private TenkiDeviceChannel[] _channels;
	private boolean show_unused_channels = false;

	public USBTenkiDevice(String serial)
	{
		_serial = serial;
		_native_pointer = 0;
	}

	public void open() throws TenkiException
	{
		int nchn;

		_native_pointer = JNI.openBySerial(_serial);
		if (_native_pointer == 0) {
			throw new TenkiDeviceNotFoundException(_serial);
		}

		nchn = JNI.getNumChannels(_native_pointer);
		_channels = new TenkiDeviceChannel[nchn];

		for (int i=0; i<nchn; i++)
		{
			/* Retreive channel ID from list index */
			int id = JNI.getChannelId(_native_pointer, i);
			int chip_id;

			_channels[i] = new TenkiDeviceChannel(this,
						id,
						JNI.getChannelName(_native_pointer, id),
						JNI.getChannelChipId(_native_pointer, id),
						JNI.getChannelTypeName(_native_pointer, id));
		}

	}

	public String getType() { 
		return "USB";
	}

	public String getSerial() {
		return _serial;
	}

	public String getVersion() {
		int v = JNI.getVersion(_native_pointer);
		return new String("" + ((v&0xff00) >> 8) + "." + (v & 0xff));
	}

	public TenkiDeviceChannel[] getAvailableChannels()
	{
		return _channels;
	}

	public void finalize()
	{
		if (_native_pointer != 0) {
			JNI.cleanUp(_native_pointer);
			_native_pointer = 0;
		}
	}

	public float getConvertedChannelValue(TenkiDeviceChannel chn)
	{
		return JNI.getConvertedChannelValue(_native_pointer, chn.getId());
	}
	
	public int getConvertedChannelUnit(TenkiDeviceChannel chn)
	{
		return JNI.getConvertedChannelUnit(_native_pointer, chn.getId());
	}

	public void readChannel(TenkiDeviceChannel chn) throws TenkiException
	{
		JNI.readChannel(_native_pointer, chn.getId());
	}


}

