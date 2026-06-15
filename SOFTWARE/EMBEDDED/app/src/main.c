#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <dk_buttons_and_leds.h>
#include "gringgo_ble.h"
#include "sensors_wrapper.h"

#define SIMUL_INTERVAL_MS       1000
#define LOW_ENERGY_INTERVAL_MS  5000

int main(void)
{
    int blink_status = 0;

    printk("\n--- BOOT GRING-GO ---\n");

    if (dk_leds_init() != 0 || gringgo_ble_init() != 0) {
        printk("[ERR] Echec de l'initialisation du systeme\n");
        return 0;
    }

    sensors_wrapper_init(); // Initialise le mode hybride

    printk("[INIT] BLE et Affichage prets, en attente de l'application Android...\n");

    for (;;) {
        dk_set_led(DK_LED1, (++blink_status) % 2);

        printk("\n[SYS] --- Top %d --- | Sante: %s | Low Energy: %s\n",
               blink_status,
               (health_activation_state == 1) ? "ON" : "OFF",
               (low_energy_state == 1) ? "ON" : "OFF");

        /* 1. LECTURE DES DONNEES (Simulées ou réelles selon les capteurs) */
        if (health_activation_state == 1) {
            sensors_wrapper_update();
        }

        /* 2. ENVOI BLE */
        gringgo_ble_update(blink_status);

        /* 3. GESTION DE L'ÉNERGIE */
        int current_delay = (low_energy_state == 1) ? LOW_ENERGY_INTERVAL_MS : SIMUL_INTERVAL_MS;
        k_sleep(K_MSEC(current_delay));
    }
}