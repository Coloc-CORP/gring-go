/*
 * @file
 * @brief Bio driver implementation for MAX32664
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "bio_driver.h"
#include "gringgo_types.h"
#include "gringgo_status.h"

LOG_MODULE_REGISTER(bio_driver, LOG_LEVEL_DBG);

/* Récupération des périphériques depuis le Device Tree */
static const struct i2c_dt_spec bio_i2c = I2C_DT_SPEC_GET(DT_ALIAS(bio_i2c));
static const struct gpio_dt_spec bio_reset = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), bio_rst_gpios);
static const struct gpio_dt_spec bio_mfio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), bio_mfio_gpios);

/* Table de correspondance pour le mode d'énergie (SetMode) */
static const uint8_t mode_val_map[] = {
    [MODE_SLEEPING]     = 0x09, /* Valeur pour BIO_FAM_SET_DEVICE_MODE */
    [MODE_WALKING]      = 0x01, /* Valeur pour BIO_FAM_OUTPUT_MODE */
    [MODE_RUNNING]      = 0x02  /* Valeur pour BIO_FAM_OUTPUT_MODE */
};

/* --- Fonctions utilitaires du squelette --- */
/**
 * @brief Write a value to a register and check status
 * @param reg Register address
 * @param val Value to write
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_WriteReg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = {reg, val};
    if (i2c_write_dt(&bio_i2c, tx, 2) != 0) return STATUS_ERR_I2C_COM;
    
    k_msleep(2); // CMD_DELAY impératif selon p.21
    
    uint8_t status;
    if (i2c_read_dt(&bio_i2c, &status, 1) != 0) return STATUS_ERR_I2C_COM;
    return (status == BIO_STATUS_SUCCESS) ? STATUS_OK : STATUS_ERR_INVALID_PARAM;
}

/**
 * @brief Read a value from a register and check status
 * @param reg Register address
 * @param val Pointer to store read value
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_ReadReg(uint8_t reg, uint8_t *val) {
    uint8_t tx[2] = {reg, 0x00}; 
    if (i2c_write_dt(&bio_i2c, tx, 2) != 0) return STATUS_ERR_I2C_COM;
    
    k_msleep(2);
    
    uint8_t rx[2]; // [0] = Status, [1] = Data
    if (i2c_read_dt(&bio_i2c, rx, 2) != 0) return STATUS_ERR_I2C_COM;
    
    *val = rx[1];
    return (rx[0] == BIO_STATUS_SUCCESS) ? STATUS_OK : STATUS_ERR_INVALID_PARAM;
}

/* --- Implémentation des fonctions BIO --- */

/**
 * @brief Initialize the biometric sensor and configure for external accel (Table 8)
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_Init(void) {
    if (!device_is_ready(bio_i2c.bus)) return STATUS_ERR_I2C_COM;

    /* 1. Séquence de démarrage Hardware (Figure 9, p.18) */
    gpio_pin_configure_dt(&bio_reset, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&bio_mfio, GPIO_OUTPUT_ACTIVE);

    gpio_pin_set_dt(&bio_reset, 1); // RSTN Low
    k_msleep(10);
    gpio_pin_set_dt(&bio_mfio, 1); // MFIO High (Mode Application)
    k_msleep(1);
    gpio_pin_set_dt(&bio_reset, 0); // RSTN High
    k_msleep(1000); // Temps d'initialisation de l'application firmware
    
    gpio_pin_configure_dt(&bio_mfio, GPIO_INPUT);

    /* 2. Configuration selon Table 8 (Mode Accéléromètre Externe) */
    
    // Set output mode (0x10 0x00 0x03)
    BIO_WriteReg(BIO_FAM_OUTPUT_MODE, BIO_VAL_OUTPUT_SENSOR_ALGO);

    // Set FIFO threshold (0x10 0x01 0x0F)
    uint8_t cmd_threshold[] = {BIO_FAM_OUTPUT_MODE, 0x01, BIO_VAL_FIFO_THRESHOLD_DEFAULT};
    i2c_write_dt(&bio_i2c, cmd_threshold, 3);
    k_msleep(2);

    // Enable Host Accel Input FIFO (0x44 0x04 0x01 0x01)
    uint8_t cmd_ext_fifo[] = {BIO_FAM_SENSOR_CONF, 0x04, BIO_VAL_EXT_ACCEL_EN, 0x01};
    i2c_write_dt(&bio_i2c, cmd_ext_fifo, 4);
    k_msleep(2);

    // Enable Algorithm (0x52 0x02 0x01)
    uint8_t cmd_algo[] = {BIO_FAM_ALGO_CONF, 0x02, BIO_VAL_ALGO_MODE_WHRM};
    i2c_write_dt(&bio_i2c, cmd_algo, 3);
    k_msleep(2);

    uint8_t mode_verify;
    return BIO_ReadReg(BIO_FAM_READ_DEVICE_MODE, &mode_verify);
}

/**
 * @brief Set the operational mode of the biometric sensor
 * @param mode Desired activity mode
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_SetMode(activity_mode_t mode) {
    if (mode == MODE_STILL) {
        gpio_pin_set_dt(&bio_reset, 1);
        return STATUS_OK;
    }
    
    gpio_pin_set_dt(&bio_reset, 0);

    if (mode == MODE_SLEEPING) {
        // Commande sur la famille 0x01 (Device Mode)
        return BIO_WriteReg(BIO_FAM_SET_DEVICE_MODE, mode_val_map[mode]);
    } else {
        // Commande sur la famille 0x10 (Output Mode)
        return BIO_WriteReg(BIO_FAM_OUTPUT_MODE, mode_val_map[mode]);
    }
}

/**
 * @brief Retrieve sensor data (HR, SpO2, Confidence)
 * @param data Pointer to bio_data_t structure to fill
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_ReadData(bio_data_t *data) {
    uint8_t tx[2] = {BIO_FAM_READ_DATA, 0x01}; // Famille 0x12, Index 0x01
    uint8_t rx[10]; 
    
    if (i2c_write_dt(&bio_i2c, tx, 2) != 0) return STATUS_ERR_I2C_COM;
    k_msleep(2);
    
    if (i2c_read_dt(&bio_i2c, rx, 10) == 0 && rx[0] == BIO_STATUS_SUCCESS) {
        data->bpm = rx[2];
        data->spo2 = rx[3];
        data->hrv = rx[4];
        return STATUS_OK;
    }
    return STATUS_ERR_I2C_COM;
}

/**
 * @brief Inject motion data (IMU) into the biometric sensor (Table 8)
 * @param motion_data Pointer to imu_data_t structure containing motion data
 * @return STATUS_OK on success, error code otherwise
 */
int BIO_InjectMotionData(imu_data_t *motion_data) {
    /* Injection via famille 0x14 (Input FIFO) */
    uint8_t tx[8];
    tx[0] = BIO_FAM_INPUT_FIFO;
    tx[1] = 0x00; // Index
    
    // Conversion Big Endian pour le HUB (X, Y, Z sur 16 bits)
    tx[2] = (uint8_t)(motion_data->accel_x >> 8);
    tx[3] = (uint8_t)(motion_data->accel_x & 0xFF);
    tx[4] = (uint8_t)(motion_data->accel_y >> 8);
    tx[5] = (uint8_t)(motion_data->accel_y & 0xFF);
    tx[6] = (uint8_t)(motion_data->accel_z >> 8);
    tx[7] = (uint8_t)(motion_data->accel_z & 0xFF);

    if (i2c_write_dt(&bio_i2c, tx, 8) != 0) return STATUS_ERR_I2C_COM;
    k_msleep(2);
    
    uint8_t status;
    i2c_read_dt(&bio_i2c, &status, 1);
    return (status == BIO_STATUS_SUCCESS) ? STATUS_OK : STATUS_ERR_INVALID_PARAM;
}
