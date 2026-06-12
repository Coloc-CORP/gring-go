#ifndef CHARGE_DRIVER_H_
#define CHARGE_DRIVER_H_

#include <zephyr/types.h>
#include "gringgo_status.h"
#include "gringgo_types.h"

/* Adresses des registres (Section 9.6) */
#define BQ_REG_STATUS          0x00
#define BQ_REG_FAULT           0x01
#define BQ_REG_TS_CONTROL      0x02
#define BQ_REG_FAST_CHG        0x03
#define BQ_REG_TERM_ADDR       0x04
#define BQ_REG_BAT_VOLT        0x05
#define BQ_REG_SYS_VOUT        0x06
#define BQ_REG_LDO_CTRL        0x07

/* Masques d'état (Table 12) */
#define BQ_STAT_CHG_MASK       0xC0
#define BQ_STAT_READY          0x00
#define BQ_STAT_CHG_IN_PROG    0x40
#define BQ_STAT_CHG_DONE       0x80
#define BQ_STAT_FAULT          0xC0
#define BQ_CHG_DISABLE_MASK    0x02  /* Bit 1 (CE) : 1 = Disabled */
#define BQ_SHIP_MODE_EN_MASK   0x20  /* Bit 5 (EN_SHIPMODE) */

/* Prototypes */
int CHG_Init(void);
int CHG_GetStatus(charge_state_t *state);
int CHG_Control(bool enable);
int CHG_SetCurrent(uint16_t ma);
int CHG_EnableLDO(bool enable);
int CHG_EnterShipMode(void);

int CHG_WriteReg(uint8_t reg, uint8_t val);
int CHG_ReadReg(uint8_t reg, uint8_t *val);
#endif /* CHARGE_DRIVER_H_ */