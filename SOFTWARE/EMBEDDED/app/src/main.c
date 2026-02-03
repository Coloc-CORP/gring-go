#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "temp_driver.h"

int main(void) {
    int ret;
    float current_temp = 0.0f;

    printk("\n--- Test du Driver STTS22H ---\n");

    /* 1. Initialisation */
	printk("Initialisation...\n");
    while (TEMP_Init() != STATUS_OK) {}
    printk("STTS22H pret et identifie !\n");

    /* 2. Boucle de lecture */
    while (1) {
        ret = TEMP_ReadTemperature(&current_temp);
        
        if (ret == STATUS_OK) {
            // Zephyr printk ne supporte pas toujours les float par défaut
            // On sépare la partie entière et décimale pour être sûr
            int entier = (int)current_temp;
            int decimal = (int)((current_temp - entier) * 100);
            if (decimal < 0) decimal = -decimal;

            printk("Temperature: %d.%02d C\n", entier, decimal);
        } else {
            printk("Erreur de lecture: %d\n", ret);
			while (TEMP_Init() != STATUS_OK) {
				printk("Reconnexion...\n");
			}
			printk("Capteur reconnecté et configuré !\n");
        }

        k_msleep(1000); // Attendre 1 seconde
    }

    return 0;
}