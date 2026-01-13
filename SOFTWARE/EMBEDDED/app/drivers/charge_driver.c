#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include "charge_driver.h"

LOG_MODULE_REGISTER(charge_driver, LOG_LEVEL_INF);

static const struct i2c_dt_spec chg_i2c = I2C_DT_SPEC_GET(DT_ALIAS(chg_i2c));

/** 
 *  @brief Écrit un registre du BQ25120A via I2C.
 *  @param reg Adresse du registre à écrire.
 *  @param val Valeur à écrire dans le registre.
 *  @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_WriteReg(uint8_t reg, uint8_t val) {
    if (i2c_reg_write_byte_dt(&chg_i2c, reg, val) != 0) {
        return STATUS_ERR_I2C_COM;
    }
    return STATUS_OK;
}

int CHG_ReadReg(uint8_t reg, uint8_t *val) {
    if (i2c_reg_read_byte_dt(&chg_i2c, reg, val) != 0) {
        return STATUS_ERR_I2C_COM;
    }
    return STATUS_OK;
}


/**
 * @brief Initialisation du driver de CHG BQ25120A.
 * @param None
 * @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_Init(void) {
    if (!device_is_ready(chg_i2c.bus)) return STATUS_ERR_I2C_COM;

    /* 1. Configuration du Buck à 3.0V (Table 18/19)
       SYS_SEL = 11 (plage 1.8V - 3.3V)
       Calcul : 1.8V + (12 * 100mV) = 3.0V -> Code SYS_VOUT = 12 (0xC)
       Reg 0x06 = EN_SYS_OUT(1) | SYS_SEL(11) | SYS_VOUT(1100) | Reserved(0) 
       Valeur = 1 11 01100 0 -> 0xF8 (mais attention B0 est toujours 0)
    */
    CHG_WriteReg(BQ_REG_SYS_VOUT, 0xF8); 
    CHG_WriteReg(BQ_REG_BAT_VOLT, 0x3C << 1);
    CHG_SetCurrent(10);

    LOG_INF("BQ25120A Initialisé (Sortie SYS: 3.0V)");
    return STATUS_OK;
}

/**
 * @brief Active ou désactive la CHG de la batterie.
 * @param enable true pour CHGr, false pour stopper la CHG.
 * @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_Control(bool enable) {
    uint8_t val;
    if (CHG_ReadReg(BQ_REG_FAST_CHG, &val) != STATUS_OK) return STATUS_ERR_I2C_COM;

    if (enable) {
        val &= ~BQ_CHG_DISABLE_MASK; // CE = 0 (charger Enabled)
    } else {
        val |= BQ_CHG_DISABLE_MASK;  // CE = 1 (charger Disabled)
    }

    return CHG_WriteReg(BQ_REG_FAST_CHG, val);
}

/**
 * @brief Configure le courant de CHG (Fast CHG)
 * @param ma Courant en milliampères (5mA à 300mA)
 * @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_SetCurrent(uint16_t ma) {
    uint8_t reg_val = 0;
    
    if (ma >= 40) {
        // Range 40mA - 300mA (Steps de 10mA)
        if (ma > 300) ma = 300;
        reg_val = BIT(7) | ((ma - 40) / 10) << 2;
    } else {
        // Range 5mA - 35mA (Steps de 1mA)
        if (ma < 5) ma = 5;
        reg_val = ((ma - 5) / 1) << 2;
    }

    /* Note: On préserve les bits CE et HZ_MODE lors de l'écriture */
    uint8_t current_reg;
    CHG_ReadReg(BQ_REG_FAST_CHG, &current_reg);
    reg_val |= (current_reg & 0x03); 

    return CHG_WriteReg(BQ_REG_FAST_CHG, reg_val);
}

/** 
 * @brief Récupère l'état de charge actuel.
 *  @param state Pointeur vers la variable de type charge_state_t pour stocker l'état.
 *  @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_GetStatus(charge_state_t *state) {
    uint8_t val;
    if (CHG_ReadReg(BQ_REG_STATUS, &val) != STATUS_OK) return STATUS_ERR_I2C_COM;

    uint8_t status_bits = val & BQ_STAT_CHG_MASK;

    switch (status_bits) {
        case BQ_STAT_READY:       *state = CHG_STATE_READY;    break;
        case BQ_STAT_CHG_IN_PROG: *state = CHG_STATE_CHARGING; break;
        case BQ_STAT_CHG_DONE:    *state = CHG_STATE_DONE;     break;
        case BQ_STAT_FAULT:       *state = CHG_STATE_FAULT;    break;
    }
    return STATUS_OK;
}

/** 
 *  @brief Active ou désactive le LDO pour les capteurs.
 *  @param enable true pour activer le LDO, false pour désactiver.
 *  @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_EnableLDO(bool enable) {
    uint8_t val;
    CHG_ReadReg(BQ_REG_LDO_CTRL, &val);
    
    if (enable) val |= 0x80;  // B7: EN_LS_LDO
    else val &= ~0x80;

    return CHG_WriteReg(BQ_REG_LDO_CTRL, val);
}

/** 
 *  @brief Met le chargeur en Ship Mode pour minimiser la consommation.
 *  @return STATUS_OK si succès, sinon code d'erreur
 */
int CHG_EnterShipMode(void) {
    LOG_WRN("Entrée en Ship Mode...");
    // B5: EN_SHIPMODE (Write Only)
    return CHG_WriteReg(BQ_REG_STATUS, 0x20);
}