#ifndef TEMP_DRIVER_H_
#define TEMP_DRIVER_H_

#include <stdint.h>
#include "gringgo_types.h"
#include "gringgo_status.h"

/* STTS22H register map used by temp_driver.c */
#define STTS22H_REG_WHOAMI      0x0F
#define STTS22H_REG_CTRL        0x04
#define STTS22H_REG_STATUS      0x27
#define STTS22H_REG_TEMP_L_OUT  0x06

/* STTS22H values used by temp_driver.c */
#define STTS22H_VAL_WHOAMI      0xA0
#define STTS22H_STATUS_BUSY     0x01

int TEMP_WriteReg(uint8_t reg, uint8_t val);
int TEMP_ReadReg(uint8_t reg, uint8_t *val);
int TEMP_Init(void);
int TEMP_ReadTemperature(float *temp_c);
int TEMP_SetMode(temp_op_mode_t mode);

#endif /* TEMP_DRIVER_H_ */
