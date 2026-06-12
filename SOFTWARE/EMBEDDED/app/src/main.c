#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "imu_driver.h"
#include "gringgo_types.h"
#include "gringgo_status.h"

#define IMU_TEST_DELAY_MS 500

int main(void) {
    int ret;
    sensors_data_t gringgo_sensors = {0};

    printk("\n========================================\n");
    printk("   STARTING GRING-GO IMU BHI260AP TEST  \n");
    printk("========================================\n");

    /* 1. Initialisation et chargement du Firmware dans la RAM du Bosch */
    printk("[IMU_INIT] Initialisation du Fuser2 & Upload du binaire...\n");
    ret = IMU_Init();
    if (ret != STATUS_OK) {
        printk("[CRITICAL] Code d'erreur IMU_Init : %d\n", ret);
        printk("-> Verifie l'adresse I2C (0x28), tes connecteurs JST et le fichier .bin.inc\n");
        // Blocage si le hardware ne boot pas
        while (1) { k_msleep(1000); }
    }
    printk("[IMU_INIT] BHI260AP pret, firmware charge et execute !\n");

    /* 2. Configuration du capteur virtuel de pas (ID 52) */
    // On l'échantillonne à 1 Hz pour un test simple en polling
    printk("[CONFIG] Activation du capteur virtuel Step Counter (ID %d)...\n", BHI260_SENSOR_ID_STEP_COUNTER);
    ret = IMU_EnableSensor(BHI260_SENSOR_ID_STEP_COUNTER, 1.0f);
    if (ret != STATUS_OK) {
        printk("[CONFIG] Erreur d'activation du capteur : %d\n", ret);
    } else {
        printk("[CONFIG] Capteur de pas active avec succes !\n");
    }

    printk("\n[LOOP] Debut de la boucle de lecture (Fais bouger la bague / ton DK)...\n");

    /* 3. Boucle principale de lecture des pas */
    while (1) {
        // Lecture et parsing de la FIFO NWU (Non Wake-Up) du canal 2
        ret = IMU_ReadData(&gringgo_sensors);
        
        if (ret == STATUS_OK) {
            // Ton driver met directement à jour la structure sensors_data_t
            printk("[PAS] Compteur global : %u pas\n", gringgo_sensors.imu.steps_count);
        } else {
            printk("[ERR] Echec de lecture burst ou corruption de la FIFO (Code: %d)\n", ret);
            
            // Tentative de secours automatique en réinitialisant le capteur
            printk("[RECOVERY] Re-initialisation du BHI260AP...\n");
            if (IMU_Init() == STATUS_OK) {
                IMU_EnableSensor(BHI260_SENSOR_ID_STEP_COUNTER, 1.0f);
                printk("[RECOVERY] BHI260AP recupere !\n");
            }
        }

        k_msleep(IMU_TEST_DELAY_MS);
    }

    return 0;
}