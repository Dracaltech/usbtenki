#ifndef _interface_h__
#define _interface_h__

void usbtenki_delay_ms(int ms);		

int sensors_init(void);
int sensors_getNumChannels(void);
int sensors_getChipID(unsigned char id);

/** 
 * \brief Called to get a raw value from a sensor. 
 * \return Number of bytes returned (max 6)
 **/
int sensors_getRaw(unsigned char id, unsigned char *dst);


#endif // _interface_h__

