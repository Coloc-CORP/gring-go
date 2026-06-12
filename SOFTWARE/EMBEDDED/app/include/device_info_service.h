#ifndef DEVICE_INFO_SERVICE_H_
#define DEVICE_INFO_SERVICE_H_

#include <stdint.h>

uint8_t DIS_GetHealthActivation(void);
uint8_t DIS_GetLowEnergyMode(void);
void DIS_SetHealthActivation(uint8_t active);
void DIS_SetLowEnergyMode(uint8_t active);

#endif /* DEVICE_INFO_SERVICE_H_ */
