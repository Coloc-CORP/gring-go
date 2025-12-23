/*
 * @file
 * @brief Haptic driver implementation
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "haptic_driver.h"

LOG_MODULE_REGISTER(haptic_drv, CONFIG_LOG_DEFAULT_LEVEL);

/* Récupération des périphériques depuis le Device Tree */
static const struct i2c_dt_spec drv_i2c = I2C_DT_SPEC_GET(DT_ALIAS(haptic_i2c));
static const struct gpio_dt_spec drv_en = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), haptic_en_gpios);

/*
 * @brief Write a byte to a register of the haptic driver
 * @param reg Register address
 * @param val Value to write
 * @return status code
 */
static int drv_write(uint8_t reg, uint8_t val) {
    if (i2c_reg_write_byte_dt(&drv_i2c, reg, val) != 0) {
        return STATUS_ERR_I2C_COM;
    }
    return STATUS_OK;
}

/*
 * @brief Initialize the haptic driver
 * @return status code
 */
int HAPTIC_Init(void) {
    /* 1. Vérification du matériel */
    if (!device_is_ready(drv_i2c.bus) || !device_is_ready(drv_en.port)) {
        LOG_ERR("Hardware non pret (I2C ou GPIO)");
        return STATUS_ERR_I2C_COM;
    }

    /* 2. Configurer et Activer la pin EN */
    gpio_pin_configure_dt(&drv_en, GPIO_OUTPUT_ACTIVE);
    k_usleep(250); // Temps de réveil du DRV2605

    /* 3. Configurer le DRV2605L */
    int ret;
    ret = drv_write(0x01, 0x00); // Mode Normal (sortir de standby)
    if (ret != STATUS_OK) return ret;

    ret = drv_write(0x03, 0x06); // Sélection Librairie LRA (Librairie 6 est souvent utilisée pour LRA)
    if (ret != STATUS_OK) return ret;

    ret = drv_write(0x1A, 0xB6); // FEEDBACK : Set N_ERM_LRA (bit 7) + Gain
    if (ret != STATUS_OK) return ret;

    ret = drv_write(0x1D, 0x20); // CONTROL3 : LRA Open Loop (plus stable pour débuter)
    if (ret != STATUS_OK) return ret;

    LOG_INF("Haptic Driver Initialise (LRA Mode)");
    return STATUS_OK;
}

/*
 * @brief Play a haptic pattern based on the alert effect
 * @param effect_id The alert effect identifier
 * @return status code
 */
int HAPTIC_PlayPattern(alert_effect_t effect_id) {
    /* 1. Charger l'effet dans la séquence */
    if (drv_write(0x04, (uint8_t)effect_id) != STATUS_OK) return STATUS_ERR_I2C_COM;
    if (drv_write(0x05, 0x00) != STATUS_OK) return STATUS_ERR_I2C_COM; // Fin de séquence

    /* 2. Register GO */
    return drv_write(0x0C, 0x01);
}
