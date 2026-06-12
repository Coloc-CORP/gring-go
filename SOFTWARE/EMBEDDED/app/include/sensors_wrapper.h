#ifndef SENSORS_WRAPPER_H_
#define SENSORS_WRAPPER_H_

#include "gringgo_types.h"
#include "gringgo_status.h"

int SENSORS_Init(void);
int SENSORS_ReadAll(sensors_data_t *data);

#endif /* SENSORS_WRAPPER_H_ */
