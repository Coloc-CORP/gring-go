/*
 * @file
 * @brief Sensors Wrapper Implementation - Centralized sensor data management (Hybrid Mode)
 */

#include <zephyr/sys/printk.h>
#include "sensors_wrapper.h" // Doit contenir tes #define SIMUL_...
#include "gringgo_types.h"

/* --- Driver Includes --- */
#include "imu_driver.h"
#include "bio_driver.h"
#include "temp_driver.h"
#include "charge_driver.h"
#include "haptic_driver.h"

/* ========================================================================= */
/* --- Global Sensor Data --- */
/* ========================================================================= */
sensors_data_t global_sensors_data = {0};

/* ========================================================================= */
/* --- Initialization Function --- */
/* ========================================================================= */
int sensors_wrapper_init(void)
{
    int err = 0;

    printk("[WRAPPER] Initializing sensors (Hybrid Real/Simul Mode)...\n");

    /* Initialisation des valeurs de base pour éviter d'envoyer '0' en simulation */
    global_sensors_data.bio.bpm = 70;
    global_sensors_data.bio.spo2 = 98;
    global_sensors_data.imu.steps_count = 1250;
    global_sensors_data.temp_c = 36;
    global_sensors_data.charge.battery_level = 85;

    /* --- Initialize IMU (BHI260AP) --- */
#if !SIMUL_STEPS
    err = IMU_Init();
    if (err != 0) {
        printk("[ERR] IMU initialization failed (code: %d)\n", err);
        return err;
    }
    printk("[WRAPPER] ✓ IMU (Hardware) initialized\n");
#else
    printk("[WRAPPER] ⚙ IMU (Simulation) active\n");
#endif

    /* --- Initialize BIO (UG6806) --- */
#if (!SIMUL_BPM || !SIMUL_SPO2)
    err = BIO_Init();
    if (err != 0) {
        printk("[ERR] BIO initialization failed (code: %d)\n", err);
        return err;
    }
    printk("[WRAPPER] ✓ BIO (Hardware) initialized\n");
#else
    printk("[WRAPPER] ⚙ BIO (Simulation) active\n");
#endif

    /* --- Initialize TEMP (STTS22H) --- */
#if !SIMUL_TEMP
    err = TEMP_Init();
    if (err != 0) {
        printk("[ERR] TEMP initialization failed (code: %d)\n", err);
        return err;
    }
    printk("[WRAPPER] ✓ TEMP (Hardware) initialized\n");
#else
    printk("[WRAPPER] ⚙ TEMP (Simulation) active\n");
#endif

    /* --- Initialize CHG (BQ25010) --- */
#if !SIMUL_BATTERY
    err = CHG_Init();
    if (err != 0) {
        printk("[ERR] CHG initialization failed (code: %d)\n", err);
        return err;
    }
    printk("[WRAPPER] ✓ CHG (Hardware) initialized\n");
#else
    printk("[WRAPPER] ⚙ CHG (Simulation) active\n");
#endif

    printk("[WRAPPER] All sensors ready\n");

    /* --- Initialize HAPTIC (DRV2605) --- */
#if !SIMUL_HAPTIC
    err = HAPTIC_Init();
    if (err != 0) {
        printk("[ERR] HAPTIC initialization failed (code: %d)\n", err);
        // Ne fait pas de "return err;" ici pour ne pas bloquer les autres capteurs si le vibreur déconne
    } else {
        printk("[WRAPPER] ✓ HAPTIC (Hardware) initialized\n");
    }
#else
    printk("[WRAPPER] ⚙ HAPTIC (Simulation) active\n");
#endif
    return 0;
}

/* ========================================================================= */
/* --- Update Function --- */
/* ========================================================================= */
void sensors_wrapper_update(void)
{
    static int tick = 0;
    int err = 0;
    charge_state_t charge_state = CHG_STATE_READY;

    /* --- Read IMU Data (Steps) --- */
#if SIMUL_STEPS
    global_sensors_data.imu.steps_count += 2;
#else
    err = IMU_ReadData(&global_sensors_data);
    if (err != 0) printk("[WRAPPER] Warning: IMU read failed (code: %d)\n", err);
#endif

    /* --- Read BIO Data (Heart Rate + SpO2) --- */
    // Pour simplifier, si un seul des deux n'est pas simulé, on lit le vrai capteur
#if (!SIMUL_BPM || !SIMUL_SPO2)
    err = BIO_ReadData(&global_sensors_data.bio);
    if (err != 0) printk("[WRAPPER] Warning: BIO read failed (code: %d)\n", err);
#endif

#if SIMUL_BPM
    global_sensors_data.bio.bpm++; 
    if(global_sensors_data.bio.bpm > 110) global_sensors_data.bio.bpm = 65;
#endif

#if SIMUL_SPO2
    global_sensors_data.bio.spo2 = (tick % 20 == 0) ? 97 : 99;
#endif

    /* --- Read TEMP Data (Temperature in Celsius) --- */
#if SIMUL_TEMP
    global_sensors_data.temp_c = (tick % 10 == 0) ? 37 : 36;
#else
    err = TEMP_ReadTemperature(&global_sensors_data.temp_c);
    if (err != 0) printk("[WRAPPER] Warning: TEMP read failed (code: %d)\n", err);
#endif

    /* --- Read CHG Data (Battery Level + Charging State) --- */
#if SIMUL_BATTERY
    if (tick % 60 == 0 && global_sensors_data.charge.battery_level > 5) {
        global_sensors_data.charge.battery_level--;
    }
#else
    err = CHG_GetStatus(&charge_state);
    if (err != 0) {
        printk("[WRAPPER] Warning: CHG read failed (code: %d)\n", err);
    }
    // Assure-toi d'avoir une fonction CHG_GetBatteryLevel ou similaire pour récupérer le %.
    // Par ex: CHG_GetBatteryLevel(&global_sensors_data.charge.battery_level);
    global_sensors_data.charge.is_charging = (charge_state == CHG_STATE_CHARGING);
#endif
}