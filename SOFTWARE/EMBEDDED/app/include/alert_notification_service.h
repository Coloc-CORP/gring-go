#ifndef ALERT_NOTIFICATION_SERVICE_H_
#define ALERT_NOTIFICATION_SERVICE_H_

#include <stdint.h>

/* Send alert notification with type and level */
void ANS_SendAlert(uint8_t alert_type, uint8_t alert_level);

/* Get current alert level */
uint8_t ANS_GetAlertLevel(void);

#endif /* ALERT_NOTIFICATION_SERVICE_H_ */
