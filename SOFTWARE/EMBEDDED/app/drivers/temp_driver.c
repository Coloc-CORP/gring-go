/*
 * @file
 * @brief Temperature sensor driver for STTS22H
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "temp_driver.h"

LOG_MODULE_REGISTER(temp_drv, CONFIG_LOG_DEFAULT_LEVEL);

/* Récupération des périphériques depuis le Device Tree */
static const struct i2c_dt_spec temp_i2c = I2C_DT_SPEC_GET(DT_ALIAS(temp_i2c));

int TEMP_WriteReg(uint8_t reg, uint8_t val) {
    return i2c_reg_write_byte_dt(&temp_i2c, reg, val) == 0 ? STATUS_OK : STATUS_ERR_I2C_COM;
}

int TEMP_ReadReg(uint8_t reg, uint8_t *val) {
    return i2c_reg_read_byte_dt(&temp_i2c, reg, val) == 0 ? STATUS_OK : STATUS_ERR_I2C_COM;
}

/*
 * @brief Initialize the temperature sensor
 * @return status code
 */
int TEMP_Init(void) {
    /* 1. Vérification du matériel */
    uint8_t whoami;

    if (!device_is_ready(temp_i2c.bus)) {
        LOG_ERR("Bus I2C non pret");
        return STATUS_ERR_I2C_COM;
    }

    /* Vérification WHO_AM_I */
    if (TEMP_ReadReg(STTS22H_REG_WHOAMI, &whoami) != STATUS_OK || whoami != STTS22H_VAL_WHOAMI) {
        LOG_ERR("STTS22H introuvable (ID: 0x%02X)", whoami);
        return STATUS_ERR_INVALID_PARAM;
    }

    /* Configuration : 1Hz + BDU (Block Data Update) + Auto-increment */    
    return TEMP_SetMode(TEMP_MODE_LOW_ODR);
}

/*
 * @brief Read temperature in Celsius
 * @param temp_c Pointer to store the temperature value
 * @return status code
 */
int TEMP_ReadTemperature(float *temp_c) {
    uint8_t status;
    uint8_t data[2];
    int16_t temp_raw;
    int retry = 10; // Sécurité pour ne pas boucler à l'infini

    /* 1. Attendre que la donnée soit disponible (BUSY == 0) */
    do {
        if (TEMP_ReadReg(STTS22H_REG_STATUS, &status) != STATUS_OK) {
            return STATUS_ERR_I2C_COM;
        }
        if (!(status & STTS22H_STATUS_BUSY)) {
            break; // Sort de la boucle si BUSY == 0
        }
        k_msleep(1); // Petite pause
    } while (--retry > 0);

    if (retry == 0) {
        return STATUS_ERR_NOT_READY;
    }

    /* 2. Lecture burst des deux registres (0x06 et 0x07) */
    if (i2c_burst_read_dt(&temp_i2c, STTS22H_REG_TEMP_L_OUT, data, 2) != 0) {
        return STATUS_ERR_I2C_COM;
    }

    temp_raw = (int16_t)((data[1] << 8) | data[0]);

    /* 3. Conversion 100 LSB/°C */
    *temp_c = (float)temp_raw / 100.0f;

    return STATUS_OK;
}

/*
 * @brief Change operational mode of the temperature sensor
 * @param mode The new operational mode
 * @return status code
 */
int TEMP_SetMode(temp_op_mode_t mode) {
    return TEMP_WriteReg(STTS22H_REG_CTRL, (uint8_t)mode);
}