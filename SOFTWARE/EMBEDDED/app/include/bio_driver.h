#ifndef BIO_DRIVER_H_
#define BIO_DRIVER_H_

#include <stdint.h>

#include "gringgo_types.h"
#include "gringgo_status.h"

/*
 * BIO compatibility constants used by drivers/bio_driver.c.
 * Keep aliases grouped here to avoid repeating the same values in the C file.
 */
#define BIO_FAM_DEVICE_MODE           0x01
#define BIO_FAM_SET_DEVICE_MODE       BIO_FAM_DEVICE_MODE
#define BIO_FAM_READ_DEVICE_MODE      BIO_FAM_DEVICE_MODE

#define BIO_FAM_OUTPUT_MODE           0x10
#define BIO_FAM_READ_DATA             0x12
#define BIO_FAM_INPUT_FIFO            0x14
#define BIO_FAM_SENSOR_CONF           0x44
#define BIO_FAM_ALGO_CONF             0x52

#define BIO_VAL_OUTPUT_SENSOR_ALGO    0x03
#define BIO_VAL_FIFO_THRESHOLD_DEFAULT 0x0F
#define BIO_VAL_EXT_ACCEL_EN          0x01
#define BIO_VAL_ALGO_MODE_WHRM        0x01

int BIO_WriteReg(uint8_t reg, uint8_t val);
int BIO_ReadReg(uint8_t reg, uint8_t *val);
int BIO_Init(void);
int BIO_SetMode(activity_mode_t mode);
int BIO_ReadData(bio_data_t *data);
int BIO_InjectMotionData(imu_data_t *motion_data);

#endif /* BIO_DRIVER_H_ */
