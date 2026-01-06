#ifndef BIO_DRIVER_H_
#define BIO_DRIVER_H_

#include <zephyr/types.h>
#include "gringgo_types.h"
#include "gringgo_status.h"

/* --- Familles de Commandes (UG6806) --- */
#define BIO_FAM_HUB_STATUS         0x00
#define BIO_FAM_SET_DEVICE_MODE    0x01
#define BIO_FAM_READ_DEVICE_MODE   0x02
#define BIO_FAM_OUTPUT_MODE        0x10
#define BIO_FAM_READ_DATA          0x12
#define BIO_FAM_INPUT_FIFO         0x14
#define BIO_FAM_SENSOR_CONF        0x44
#define BIO_FAM_ALGO_CONF          0x52
#define BIO_FAM_BTLDR_FLASH        0x80
#define BIO_FAM_IDENTITY           0xFF

/* --- Valeurs de Statut (Table 5, p.20) --- */
#define BIO_STATUS_SUCCESS         0x00
#define BIO_STATUS_ERR_TRY_AGAIN   0xFE

/* --- Valeurs de Configuration (Table 8) --- */
#define BIO_VAL_OUTPUT_SENSOR_ALGO     0x03  /* Sensor + Algo Data */
#define BIO_VAL_FIFO_THRESHOLD_DEFAULT 0x0F  /* Seuil FIFO standard */
#define BIO_VAL_EXT_ACCEL_EN           0x01  /* Activation source externe */
#define BIO_VAL_ALGO_MODE_WHRM         0x01  /* Mode algorithme 1 (WHRM) */

/* --- Prototypes du Driver --- */

int BIO_Init(void);
int BIO_SetMode(activity_mode_t mode);
int BIO_ReadData(bio_data_t *data);
int BIO_ReadHeartRate(bio_data_t *data);
int BIO_ReadSpO2(bio_data_t *data);
int BIO_InjectMotionData(imu_data_t *motion_data);

/* Fonctions utilitaires demandées par le squelette */
int BIO_WriteReg(uint8_t reg, uint8_t val);
int BIO_ReadReg(uint8_t reg, uint8_t *val);

#endif /* BIO_DRIVER_H_ */