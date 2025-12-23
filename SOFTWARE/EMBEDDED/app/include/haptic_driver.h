/*
 * @file
 * @brief Haptic driver interface
 */

#ifndef HAPTIC_DRIVER_H_
#define HAPTIC_DRIVER_H_

#include "gringgo_types.h"
#include "gringgo_status.h"

int HAPTIC_Init(void);
int HAPTIC_PlayPattern(alert_effect_t effect_id);

#endif /* HAPTIC_DRIVER_H_ */