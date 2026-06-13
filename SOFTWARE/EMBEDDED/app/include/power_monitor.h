#ifndef POWER_MONITOR_H_
#define POWER_MONITOR_H_

#include <stdbool.h>
#include <stdint.h>

#include "gringgo_types.h"

void POWER_MONITOR_Init(void);
void POWER_MONITOR_Update(const sensors_data_t *data);
uint16_t POWER_MONITOR_GetBatteryMillivolts(void);
bool POWER_MONITOR_IsCharging(void);

#endif /* POWER_MONITOR_H_ */
