#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "com_raphnet_tenki_USBTenkiDevice.h"
#include "usbtenki.h"


#define MAX_CHANNELS	256


struct tenki_context {
	usb_dev_handle *hdl;
	struct USBTenki_info info;
	struct USBTenki_channel channels[MAX_CHANNELS];
	int num_channels;
};

static int indexFromId(JNIEnv *env, struct tenki_context *ctx, int chn_id)
{
	int i;
	jclass clazz;
	

	for (i=0; i<ctx->num_channels; i++) 
	{
		if (ctx->channels[i].channel_id == chn_id) {
			return i;
		}
	}

	clazz = (*env)->FindClass(env, "com/raphnet/tenki/TenkiException");
	(*env)->ThrowNew(env, clazz, "Channel ID not found");

	return -1;
}

static void validateContext(JNIEnv *env, struct tenki_context *ctx)
{
	if (!ctx) {
		jclass clazz;
		clazz = (*env)->FindClass(env, "com/raphnet/tenki/TenkiException");
		(*env)->ThrowNew(env, clazz, "Bad context");
	}
}

#define PTR_TO_JAVA(p) ( (jlong)(long)(p) )
#define PTR_FROM_JAVA(j) ( (void*)((long)(j)) )

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_openBySerial
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1openBySerial
  (JNIEnv *env, jobject obj, jstring serial)
{
	usb_dev_handle *hdl;
	jboolean is_copy;
	const char *txt_serial;
	struct tenki_context *ctx;
	struct USBTenki_info info;
	
	
	txt_serial = (*env)->GetStringUTFChars(env, serial, &is_copy);
	

	hdl = usbtenki_openBySerial(txt_serial, &info);
	if (is_copy) {
		(*env)->ReleaseStringUTFChars(env, serial, txt_serial);
	}

	if (!hdl)
		return 0;

	ctx = malloc(sizeof(struct tenki_context));
	if (!ctx) {
		perror("malloc");
		return 0;
	}

	ctx->hdl = hdl;
	memcpy(&ctx->info, &info, sizeof(struct USBTenki_info));

	/* Now list the channels */
	ctx->num_channels = usbtenki_listChannels(ctx->hdl, ctx->channels, MAX_CHANNELS);

	return PTR_TO_JAVA(ctx);
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getVersion
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getVersion
  (JNIEnv *env, jobject obj, jlong ptr)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	validateContext(env, ctx);

	return ctx->info.major << 8 | ctx->info.minor;
	
}


/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getNumChannels
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getNumChannels
  (JNIEnv *env, jobject obj, jlong ptr)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	validateContext(env, ctx);
	return ctx->num_channels;
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getChannelId
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getChannelId
  (JNIEnv *env, jobject obj, jlong ptr, jint index)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	validateContext(env, ctx);

	if (index<0 || index >= ctx->num_channels) {
		jclass clazz = (*env)->FindClass(env, "com/raphnet/tenki/TenkiException");
		(*env)->ThrowNew(env, clazz, "Bad channel ID");
		return -1; 
	}

	return ctx->channels[index].channel_id;
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getChannelName
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getChannelName
  (JNIEnv *env, jobject obj, jlong ptr, jint id)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	int idx;
	
	validateContext(env, ctx);

	idx = indexFromId(env, ctx, id);
	if (idx==-1) 
		return NULL;

	return (*env)->NewStringUTF(env, chipToString( ctx->channels[idx].chip_id ) );
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getChannelChipId
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getChannelChipId
  (JNIEnv *env, jobject obj, jlong ptr, jint id)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	int idx;
	
	validateContext(env, ctx);
	
	idx = indexFromId(env, ctx, id);
	if (idx==-1) 
		return -1;

	return ctx->channels[idx].chip_id;
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_getChannelTypeName
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1getChannelTypeName
  (JNIEnv *env, jobject obj, jlong ptr, jint id)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	int idx;
	
	validateContext(env, ctx);

	idx = indexFromId(env, ctx, id);
	if (idx==-1) 
		return NULL;

	return (*env)->NewStringUTF(env, chipToShortString( ctx->channels[idx].chip_id ) );
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_cleanUp
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1cleanUp
  (JNIEnv *env, jobject obj, jlong ptr)
{
	struct tenki_context *ctx = PTR_FROM_JAVA(ptr);
	validateContext(env, ctx);

	usb_release_interface(ctx->hdl, 0);
	usb_close(ctx->hdl);
	printf("Device closed\n");
}

/*
 * Class:     com_raphnet_tenki_USBTenkiDevice
 * Method:    n_initLibusb
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_raphnet_tenki_USBTenkiDevice_n_1initLibusb
  (JNIEnv *env, jclass cls)
{
	usb_init();
}

