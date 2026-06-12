#ifndef PULSE_OXIMETER_SERVICE_H_
#define PULSE_OXIMETER_SERVICE_H_

#include <stdint.h>

/* Update SpO2 and pulse rate, send notification */
void PLX_SERVICE_Update(uint8_t spo2, uint8_t pulse_rate);

#endif /* PULSE_OXIMETER_SERVICE_H_ */
