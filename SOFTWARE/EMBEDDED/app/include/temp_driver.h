#ifndef TEMP_DRIVER_H_
#define TEMP_DRIVER_H_

#include "gringgo_types.h"
#include "gringgo_status.h"

/* --- Registres STTS22H (Table 12) --- */
#define STTS22H_REG_WHOAMI          0x01  /* RO - Doit lire A0h */
#define STTS22H_REG_TEMP_H_LIMIT    0x02  /* RW - Seuil haut */
#define STTS22H_REG_TEMP_L_LIMIT    0x03  /* RW - Seuil bas */
#define STTS22H_REG_CTRL            0x04  /* RW - Registre de contrôle principal */
#define STTS22H_REG_STATUS          0x05  /* RO - UNDER/OVER/BUSY */
#define STTS22H_REG_TEMP_L_OUT      0x06  /* RO - Température LSB */
#define STTS22H_REG_TEMP_H_OUT      0x07  /* RO - Température MSB */

/* --- Masques Registre CTRL (04h) --- */
#define STTS22H_CTRL_LOW_ODR_START  BIT(7) /* Active le mode 1 Hz ODR */
#define STTS22H_CTRL_BDU            BIT(6) /* Block Data Update (Sécurise la lecture) */
#define STTS22H_CTRL_IF_ADD_INC     BIT(3) /* Auto-incrément d'adresse (Burst read) */
#define STTS22H_CTRL_FREERUN        BIT(2) /* Mode mesure continue */
#define STTS22H_CTRL_ONE_SHOT       BIT(0) /* Déclenche une mesure unique */

/* --- Valeurs fixes --- */
#define STTS22H_VAL_WHOAMI          0xA0

/* Prototypes */
int TEMP_Init(void);
int TEMP_WriteReg(uint8_t reg, uint8_t val);
int TEMP_ReadReg(uint8_t reg, uint8_t *val);
int TEMP_SetMode(temp_op_mode_t mode);
int TEMP_ReadTemperature(float *temp_c);

#endif /* TEMP_DRIVER_H_ */