package com.raphnet.tenki;

public class JNI
{
	public static native float getConvertedChannelValue(long handle, int id);
	public static native int getConvertedChannelUnit(long handle, int id);
	public static native void readChannel(long handle, int id);

	public static native long openBySerial(String serial);
	
	public static native int getVersion(long handle);

	public static native int getNumChannels(long handle);
	public static native int getChannelId(long handle, int index);
	public static native String getChannelName(long handle, int id);
	public static native int getChannelChipId(long handle, int id);
	public static native String getChannelTypeName(long handle, int id);

	public static native String unitToName(int unit);
	public static native String chipToName(int unit);

	public static native void cleanUp(long handle);
	
	public static native void initLibusb();

	static {		
		System.loadLibrary("tenki");
		initLibusb();
	}

}
