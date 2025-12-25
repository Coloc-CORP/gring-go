#ifndef IMU_DRIVER_H_
#define IMU_DRIVER_H_

#include "gringgo_types.h"
#include "gringgo_status.h"

/* --- Registres de l'Interface Host (Table 17, p.65) --- */
#define BHI260_REG_CH0_CMD              0x00  /* Canal de commande (Write-only) */
#define BHI260_REG_CH1_WAKEUP_FIFO      0x01  /* FIFO Réveil (Read-only) */
#define BHI260_REG_CH2_NWU_FIFO         0x02  /* FIFO Normale (Read-only) - Pour tes pas */
#define BHI260_REG_CHIP_CTRL            0x05  /* Contrôle Fuser2 (Turbo/Power) */
#define BHI260_REG_RESET_REQ            0x14  /* Reset matériel */
#define BHI260_REG_PRODUCT_ID           0x1C  /* Identification (doit lire 0x89) */
#define BHI260_REG_BOOT_STATUS          0x25  /* État du bootloader (Bit 4 = Ready) */

/* --- Commandes de l'Interface Host (Table 31, p.83) --- */
#define BHI260_CMD_FW_UPLOAD            0x0002 /* Upload vers RAM */
#define BHI260_CMD_FW_BOOT              0x0003 /* Boot depuis RAM */
#define BHI260_CMD_CONFIGURE_SENSOR     0x000D /* Configuration/Activation capteur */

/* --- Valeurs de Registres --- */
#define BHI260_VAL_RESET_FUSER2         0x01   /* Reset complet */
#define BHI260_VAL_CHIP_CTRL_TURBO_DIS  0x01   /* Désactive Turbo (20MHz) pour la bague */
#define BHI260_VAL_CHIP_CTRL_TURBO_EN   0x00   /* Mode 50MHz */

/* --- IDs des Capteurs Virtuels (Table 88, p.120-121) --- */
#define BHI260_SENSOR_ID_STEP_COUNTER   52     /* Pas (32-bit uint) */
#define BHI260_SENSOR_ID_ACTIVITY       63     /* Sport (Still, Walking, Running...) */
#define BHI260_SENSOR_ID_STATIONARY     75     /* Sommeil (Détection immobilité) */
#define BHI260_ID_META_EVENT            254    /* Événements système */
#define BHI260_ID_PADDING               0      /* Padding (à ignorer) */

/* --- Masques pour BHI260_REG_BOOT_STATUS --- */
#define BHI260_STATUS_HOST_IF_READY     0x10  /* Bit 4 : Interface prête */
#define BHI260_STATUS_VERIFY_DONE       0x20  /* Bit 5 : Signature FW valide */
#define BHI260_STATUS_VERIFY_ERROR      0x40  /* Bit 6 : Erreur de signature */

/* Prototypes */
int IMU_Init(void);
int IMU_UploadFW(void);
int IMU_SetPerformanceMode(imu_perf_mode_t mode);
int IMU_WriteReg(uint8_t reg, uint8_t val);
int IMU_ReadData(sensors_data_t *data);

#endif