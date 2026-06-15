/*
 * @file
 * @brief Sensors Wrapper - Centralized sensor data management
 */

#ifndef SENSORS_WRAPPER_H_
#define SENSORS_WRAPPER_H_

#include "gringgo_types.h" // Contient ta structure sensors_data_t

/* ========================================================================= */
/* --- CONFIGURATION DES CAPTEURS (true = simulé, false = capteur réel) ---- */
/* ========================================================================= */
#define SIMUL_BPM        true
#define SIMUL_SPO2       true
#define SIMUL_STEPS      true
#define SIMUL_TEMP       false
#define SIMUL_BATTERY    true
#define SIMUL_HAPTIC     false
/* ========================================================================= */

/* La seule et unique structure qui contient toutes les données de la bague */
extern sensors_data_t global_sensors_data;

int sensors_wrapper_init(void);
void sensors_wrapper_update(void);

#endif /* SENSORS_WRAPPER_H_ */