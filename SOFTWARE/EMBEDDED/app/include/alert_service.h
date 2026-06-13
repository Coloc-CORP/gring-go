#ifndef ALERT_SERVICE_H_
#define ALERT_SERVICE_H_

#include <stdint.h>
#include "gringgo_types.h"

void ALERT_SERVICE_Init(void);
void ALERT_SERVICE_Notify(alert_effect_t effect_id);
void ALERT_SERVICE_Clear(void);

#endif /* ALERT_SERVICE_H_ */
