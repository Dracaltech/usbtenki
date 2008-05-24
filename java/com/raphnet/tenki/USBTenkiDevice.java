package com.raphnet.tenki;


public class USBTenkiDevice implements TenkiDevice
{
	private long _native_pointer;
	private String _serial;
	private TenkiDeviceChannel[] _channels;

	public USBTenkiDevice(String serial)
	{
		_serial = serial;
		_native_pointer = 0;
	}

	public void open() throws TenkiException
	{
		int nchn;

		_native_pointer = n_openBySerial(_serial);
		if (_native_pointer == 0) {
			throw new TenkiDeviceNotFoundException(_serial);
		}

		nchn = n_getNumChannels(_native_pointer);
		_channels = new TenkiDeviceChannel[nchn];

		for (int i=0; i<nchn; i++)
		{
			/* Retreive channel ID from list index */
			int id = n_getChannelId(_native_pointer, i);

			_channels[i] = new TenkiDeviceChannel(id,
						n_getChannelName(_native_pointer, id),
						n_getChannelChipId(_native_pointer, id),
						n_getChannelTypeName(_native_pointer, id));
		}

	}

	public String getType() { 
		return "USB";
	}

	public String getSerial() {
		return _serial;
	}

	public String getVersion() {
		int v = n_getVersion(_native_pointer);
		return new String("" + ((v&0xff00) >> 8) + "." + (v & 0xff));
	}

	public TenkiDeviceChannel[] getAvailableChannels()
	{
		return _channels;
	}

	public void finalize()
	{
		if (_native_pointer != 0) {
			n_cleanUp(_native_pointer);
			_native_pointer = 0;
		}
	}


	protected native long n_openBySerial(String serial);
	
	protected native int n_getVersion(long handle);

	protected native int n_getNumChannels(long handle);
	protected native int n_getChannelId(long handle, int index);

	protected native String n_getChannelName(long handle, int id);
	protected native int n_getChannelChipId(long handle, int id);
	protected native String n_getChannelTypeName(long handle, int id);

	protected native void n_cleanUp(long handle);
	
	protected native static void n_initLibusb();

	static {		
		System.loadLibrary("tenki");
		n_initLibusb();
	}

}

