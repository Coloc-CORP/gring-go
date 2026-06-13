#include "imu_driver.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(imu_drv, CONFIG_LOG_DEFAULT_LEVEL);
static const struct i2c_dt_spec imu_i2c = I2C_DT_SPEC_GET(DT_ALIAS(imu_i2c));

/* Inclusion du binaire via CMake */
static const uint8_t bhi260_fw[] = { 
    #include <bhi260ap_arc_fw.bin.inc> 
};

int IMU_WriteReg(uint8_t reg, uint8_t val) {
    return i2c_reg_write_byte_dt(&imu_i2c, reg, val) == 0 ? STATUS_OK : STATUS_ERR_I2C_COM;
}

/**
 * @brief Upload du firmware dans la RAM du BHI260AP
 * @return STATUS_OK ou code d'erreur
 */
int IMU_UploadFW(void) {
    uint32_t total_len = sizeof(bhi260_fw);
    uint16_t total_words = (uint16_t)(total_len / 4);
    
    // Annonce de l'upload (Cmd 0x0002)
    uint8_t cmd[5] = { 
        BHI260_REG_CH0_CMD, 
        (uint8_t)(BHI260_CMD_FW_UPLOAD & 0xFF), 0x00,
        (uint8_t)(total_words & 0xFF), (uint8_t)((total_words >> 8) & 0xFF) 
    };
    if (i2c_write_dt(&imu_i2c, cmd, 5) != 0) return STATUS_ERR_I2C_COM;

    // Transfert des données (Multiple de 4 octets requis)
    for (uint32_t i = 0; i < total_len; i += 32) {
        uint8_t block[33] = { BHI260_REG_CH0_CMD };
        memcpy(&block[1], &bhi260_fw[i], 32);
        if (i2c_write_dt(&imu_i2c, block, 33) != 0) return STATUS_ERR_I2C_COM;
    }
    return STATUS_OK;
}

/**
 * @brief Initialisation du capteur IMU BHI260AP
 * @return STATUS_OK ou code d'erreur
 */
int IMU_Init(void) {
    uint8_t status, prod_id;

    // 1. Vérification Hardware
    i2c_reg_read_byte_dt(&imu_i2c, BHI260_REG_PRODUCT_ID, &prod_id);
    if (prod_id != 0x89) return STATUS_ERR_INVALID_PARAM;

    // 2. Reset et attente Ready
    IMU_WriteReg(BHI260_REG_RESET_REQ, BHI260_VAL_RESET_FUSER2);
    k_msleep(50);
    while (1) {
        i2c_reg_read_byte_dt(&imu_i2c, BHI260_REG_BOOT_STATUS, &status);
        if (status & 0x10) break; // Host Interface Ready
        k_msleep(5);
    }

    // 3. Chargement et Boot
    if (IMU_UploadFW() != STATUS_OK) return STATUS_ERR_I2C_COM;
    uint8_t boot_cmd[3] = { BHI260_REG_CH0_CMD, (uint8_t)(BHI260_CMD_FW_BOOT & 0xFF), 0x00 };
    i2c_write_dt(&imu_i2c, boot_cmd, 3);
    
    // 4. Mode Économie d'énergie (Long Run 20MHz)
    k_msleep(20);
    IMU_WriteReg(BHI260_REG_CHIP_CTRL, BHI260_VAL_CHIP_CTRL_TURBO_DIS);

    return STATUS_OK;
}

/**
 * @brief Configuration et activation d'un capteur virtuel
 * @param sensor_id ID du capteur (BHI260_SENSOR_ID_XXX)
 * @param freq_hz Fréquence d'échantillonnage en Hz
 * @return STATUS_OK ou code d'erreur
 */
int IMU_EnableSensor(uint8_t sensor_id, float freq_hz) {
    uint8_t cmd[8];
    uint32_t rate = (uint32_t)freq_hz; 

    cmd[0] = BHI260_REG_CH0_CMD;
    cmd[1] = (uint8_t)(BHI260_CMD_CONFIGURE_SENSOR & 0xFF); // 0x0D
    cmd[2] = 0x00;
    cmd[3] = sensor_id;
    cmd[4] = (uint8_t)(rate & 0xFF);
    cmd[5] = (uint8_t)((rate >> 8) & 0xFF);
    cmd[6] = 0x00; cmd[7] = 0x00; // Latency à 0

    return i2c_write_dt(&imu_i2c, cmd, 8) == 0 ? STATUS_OK : STATUS_ERR_I2C_COM;
}

/**
 * @brief Lecture des données du capteur IMU
 * @param data Pointeur vers la structure de données à remplir
 * @return STATUS_OK ou code d'erreur
 */
int IMU_ReadData(sensors_data_t *data) {
    uint8_t header[4], buffer[256];

    // Lecture du descripteur (2 octets de taille + 2 octets timestamp)
    if (i2c_burst_read_dt(&imu_i2c, BHI260_REG_CH2_NWU_FIFO, header, 4) != 0) return STATUS_ERR_I2C_COM;
    uint16_t len = (uint16_t)(header[0] | (header[1] << 8));

    if (len <= 4) return STATUS_OK;

    // Lecture Burst du contenu
    uint16_t to_read = (len - 4 > 256) ? 256 : len - 4;
    i2c_burst_read_dt(&imu_i2c, BHI260_REG_CH2_NWU_FIFO, buffer, to_read);

    // Parsing selon Table 88
    for (int i = 0; i < to_read; ) {
        uint8_t id = buffer[i++];
        switch (id) {
            case BHI260_SENSOR_ID_STEP_COUNTER:
                data->imu.steps_count = (uint32_t)(buffer[i] | (buffer[i+1] << 8) | (buffer[i+2] << 16) | (buffer[i+3] << 24));
                i += 4;
                break;
            case BHI260_SENSOR_ID_ACTIVITY:
                i += 3; // Payload activité = 3 octets
                break;
            case BHI260_ID_META_EVENT:
                i += 3; // Payload méta-événement = 3 octets
                break;
            case BHI260_ID_PADDING:
                continue;
            default:
                return STATUS_OK; // ID inconnu, arrêt pour éviter corruption
        }
    }
    return STATUS_OK;
}