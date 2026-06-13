# 📋 API Logicielle du Projet Gring-Go (Zephyr RTOS) - Convention Concise

## 1. Couche Drivers Hôtes (`drivers/`)

| Fichier | Nom de la Fonction | Entrées | Sorties | Utilité |
| :--- | :--- | :--- | :--- | :--- |
| **`haptic_driver.c`** | `HAPTIC_Init()` | `void` | `int` | Initialise le DRV2605L. |
| | `HAPTIC_PlayPattern()` | `alert_effect_t` | `int` | Déclenche un effet vibrant (Library 6 LRA). |
| | `HAPTIC_WriteReg()` | `uint8_t, uint8_t` | `static int` | Écriture registre interne. |
| **`imu_driver.c`** | `IMU_Init()` | `void` | `int` | Initialisation du BHI260AP. |
| | `IMU_UploadFW()` | `void` | `int` | Charge le firmware ARC en RAM. |
| | `IMU_SetPerformanceMode()`| `imu_perf_mode_t` | `int` | Ajuste l'ODR et la consommation. |
| | `IMU_WriteReg()` | `uint8_t, uint8_t` | `int` | Écriture registre de configuration. |
| | `IMU_ReadData()` | `sensors_data_t *` | `int` | Lit les pas, l'activité et l'accélération brute. |
| **`bio_driver.c`** | `BIO_Init()` | `void` | `int` | Initialisation + Config Accel Ext (Table 8). |
| | `BIO_SetMode()` | `activity_mode_t` | `int` | Gère les modes Shutdown/Sleep/Active. |
| | `BIO_ReadData()` | `bio_data_t *` | `int` | Lit le bloc BPM, SpO2 et HRV. |
| | `BIO_ReadHeartRate()` | `bio_data_t *` | `int` | Lecture spécifique du BPM. |
| | `BIO_ReadSpO2()` | `bio_data_t *` | `int` | Lecture spécifique de la SpO2. |
| | `BIO_InjectMotionData()`| `imu_data_t *` | `int` | Transmet l'accélération pour compensation (FIFO). |
| | `BIO_WriteReg()` | `uint8_t, uint8_t` | `int` | Écriture registre (Famille/Index). |
| | `BIO_ReadReg()` | `uint8_t, uint8_t*` | `int` | Lecture registre (Famille/Index). |
| **`temp_driver.c`** | `TEMP_Init()` | `void` | `int` | Initialise le STTS22H. |
| | `TEMP_WriteReg()` | `uint8_t, uint8_t` | `int` | Écriture registre. |
| | `TEMP_ReadReg()` | `uint8_t, uint8_t*` | `int` | Lecture registre. |
| | `TEMP_SetMode()` | `temp_op_mode_t` | `int` | Définit le mode opératoire du capteur. |
| | `TEMP_ReadTemperature()` | `float *` | `int` | Lecture de la température en Celsius. |
| **`charge_driver.c`** | `CHG_Init()` | `void` | `int` | Initialise le BQ25120A (SYS=3.0V). |
| | `CHG_GetStatus()` | `charge_state_t *` | `int` | Récupère l'état (Ready, Charging, Done, Fault). |
| | `CHG_Control()` | `bool enable` | `int` | Active/Désactive la charge (Bit CE). |
| | `CHG_SetCurrent()` | `uint16_t ma` | `int` | Configure le courant Fast Charge. |
| | `CHG_EnableLDO()` | `bool enable` | `int` | Active/Désactive le LDO interne. |
| | `CHG_EnterShipMode()` | `void` | `int` | Coupe la batterie pour stockage prolongé. |
| | `CHG_WriteReg()` | `uint8_t, uint8_t` | `int` | Écriture registre. |
| | `CHG_ReadReg()` | `uint8_t, uint8_t*` | `int` | Lecture registre. |

## 2. Codes de Statut Personnalisés (`include/gringgo_status.h`)

| Nom de la Macro | Valeur | Utilité |
| :--- | :--- | :--- |
| `STATUS_OK` | `0` | Succès de l'opération. |
| `STATUS_ERR_I2C_COM` | `-1001` | Erreur de communication I2C. |
| `STATUS_ERR_FW_FLASH` | `-1002` | Échec de chargement Firmware (Sensor Hubs). |
| `STATUS_ERR_NOT_READY` | `-1003` | Donnée indisponible ou capteur occupé. |
| `STATUS_ERR_NO_BLE_CONN`| `-1004` | Pas de connexion Bluetooth active. |
| `STATUS_ERR_INVALID_PARAM`| `-1005` | Paramètre erroné ou registre inexistant. |

## 3. Définitions des Types (`include/gringgo_types.h`)

```c
typedef enum {
    MODE_STILL, MODE_WALKING, MODE_RUNNING, MODE_SLEEPING
} activity_mode_t;

typedef enum {
    ALERT_VIB_INCOMING_CALL = 14,
    ALERT_VIB_NOTIFICATION  = 1,
    ALERT_VIB_FINDMYRING    = 16,
    ALERT_VIB_ALARM         = 10,
    ALERT_VIB_NONE          = 0
} alert_effect_t;

typedef struct {
    int16_t accel_x; int16_t accel_y; int16_t accel_z;
    int16_t gyro_x; int16_t gyro_y; int16_t gyro_z;
    uint32_t steps_count;
    activity_mode_t mode;
} imu_data_t;

typedef struct {
    uint16_t bpm;
    uint8_t spo2;
    uint16_t hrv;
} bio_data_t;

typedef struct {
    uint8_t battery_level;
    bool is_charging;
    uint16_t voltage_mv;
} charge_data_t;

typedef enum {
    CHG_STATE_READY, CHG_STATE_CHARGING, CHG_STATE_DONE, CHG_STATE_FAULT
} charge_state_t;

typedef struct {
    bio_data_t bio;
    imu_data_t imu;
    charge_data_t charge;
    float temp_c;
} sensors_data_t;