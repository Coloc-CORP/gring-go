/*
 * @file
 * @brief Gringgo types definitions
 */

#ifndef GRINGGO_TYPES_H_
#define GRINGGO_TYPES_H_

#include <zephyr/types.h>

typedef enum {
    MODE_STILL, 
    MODE_WALKING, 
    MODE_RUNNING, 
    MODE_SLEEPING
} activity_mode_t;

typedef enum {
    /* IDs basés sur la Library 6 (LRA) du DRV2605L */
    ALERT_VIB_INCOMING_CALL = 14, // Long Buzz 100%
    ALERT_VIB_NOTIFICATION  = 1,  // Strong Click 100%
    ALERT_VIB_FINDMYRING    = 16, // Triple Click 100% (Alerte forte)
    ALERT_VIB_ALARM         = 10, // Double Click 100%
    ALERT_VIB_NONE          = 0   // Fin de séquence / Arrêt
} alert_effect_t;

typedef struct {
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
} charge_status_t;

typedef struct {
    bio_data_t bio;
    imu_data_t imu;
    charge_status_t charge;
    float temp_c;
} sensors_data_t;

#endif /* GRINGGO_TYPES_H_ */
