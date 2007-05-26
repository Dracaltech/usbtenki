#ifndef _adc_h__
#define _adc_h__

void adc_init(void);
unsigned short adc_sample(char id, int n_samples, int interval_ms);

#endif // _adc_h__

