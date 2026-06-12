#ifndef CHARGE_DRIVER_H_
#define CHARGE_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

#include "gringgo_types.h"
#include "gringgo_status.h"

/* BQ25120A compatibility constants used by charge_driver.c */
#define BQ_REG_SYS_VOUT       0x06
#define BQ_REG_BAT_VOLT       0x07
#define BQ_REG_FAST_CHG       0x09
#define BQ_REG_STATUS         0x0A
#define BQ_REG_LDO_CTRL       0x0B

#define BQ_CHG_DISABLE_MASK   0x80

#define BQ_STAT_CHG_MASK      0x30
#define BQ_STAT_READY         0x00
#define BQ_STAT_CHG_IN_PROG   0x10
#define BQ_STAT_CHG_DONE      0x20
#define BQ_STAT_FAULT         0x30

int CHG_WriteReg(uint8_t reg, uint8_t val);
int CHG_ReadReg(uint8_t reg, uint8_t *val);
int CHG_Init(void);
int CHG_Control(bool enable);
int CHG_SetCurrent(uint16_t ma);
int CHG_GetStatus(charge_state_t *state);
int CHG_EnableLDO(bool enable);
int CHG_EnterShipMode(void);

#endif /* CHARGE_DRIVER_H_ */
