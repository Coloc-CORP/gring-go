#ifndef HEALTH_SERVICE_H_
#define HEALTH_SERVICE_H_

#include <stdint.h>

/* Update heart rate and HRV, send notification */
void HRS_Notify(uint16_t bpm, uint16_t hrv_ms);

#endif /* HEALTH_SERVICE_H_ */
