#ifndef GRINGGO_BLE_H_
#define GRINGGO_BLE_H_

#include <zephyr/types.h>
#include <zephyr/bluetooth/conn.h>

/* --- Etats de contrôle --- */
extern uint8_t health_activation_state;
extern uint8_t low_energy_state;
extern uint8_t haptic_alert_level;

/* --- Prototypes --- */
int gringgo_ble_init(void);

/* --- Fonction d'arbitrage BLE --- */
void gringgo_ble_update(int blink_status);

/* Prototypes des fonctions d'envoi définies dans le sous-système gatt */
void notify_bpm(uint8_t bpm_val);
void notify_temperature(uint8_t temp_val);
void notify_spo2(uint8_t spo2_val);
void notify_steps(uint32_t steps_val);
void notify_battery(uint8_t batt_val);

#endif /* GRINGGO_BLE_H_ */