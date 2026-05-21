#include <zephyr/kernel.h>
#include "haptic_driver.h"

int main(void) {
    printk("\n--- Test du Driver Haptique (DRV2605L) ---\n");

    while (HAPTIC_Init() != STATUS_OK) {
        printk("Erreur: Impossible d'initialiser le DRV2605L\n");
        k_msleep(1000); // Attendre 1 seconde avant de réessayer
    }

    printk("DRV2605L pret ! Lancement de la sequence de test...\n");

    while (1) {
        printk("Effet: Notification (Click)\n");
        HAPTIC_PlayPattern(ALERT_VIB_NOTIFICATION);
        k_msleep(2000);

        printk("Effet: Appel entrant (Long Buzz)\n");
        HAPTIC_PlayPattern(ALERT_VIB_INCOMING_CALL);
        k_msleep(3000);

        printk("Effet: Alerte (Double Click)\n");
        HAPTIC_PlayPattern(ALERT_VIB_ALARM);
        k_msleep(2000);
    }

    return 0;
}