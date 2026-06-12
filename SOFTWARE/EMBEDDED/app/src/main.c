#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bio_driver.h"
#include "gringgo_types.h"
#include "gringgo_status.h"

#define TEST_DELAY_MS 1000

int main(void) {
    int ret;
    bio_data_t test_bio_data = {0};
    
    // Structure de fausses données IMU pour alimenter l'algo de Maxim
    // On simule une bague immobile au repos (1G sur l'axe Z)
    imu_data_t fake_imu = {
        .accel_x = 0,
        .accel_y = 0,
        .accel_z = 16384 // Equivalent à 1G si l'IMU est en sensibilité +-2g (16384 LSB/g)
    };

    printk("\n========================================\n");
    printk("   STARTING GRING-GO BIO SENSOR TEST   \n");
    printk("========================================\n");

    /* 1. Initialisation complète du MAX32664 */
    printk("[INIT] Tentative de boot et configuration...\n");
    ret = BIO_Init();
    if (ret != STATUS_OK) {
        printk("[CRITICAL] Echec BIO_Init: Code d'erreur %d\n", ret);
        printk("-> Verifie le cablage des 5 fils et les pull-ups I2C !\n");
        // On bloque ici si le hardware ne repond pas
        while (1) { k_msleep(1000); }
    }
    printk("[INIT] MAX32664 identifie et configure avec succes !\n");

    /* 2. Passage en mode actif (Walking par exemple) */
    printk("[MODE] Passage en mode MODE_WALKING...\n");
    BIO_SetMode(MODE_WALKING);

    /* 3. Boucle principale de simulation et de lecture */
    while (1) {
        printk("\n--- Nouvelle iteration ---\n");

        /* A. Injection obligatoire de la fausse donnée IMU */
        // L'algorithme interne du MAX32664 exige une synchro temporelle avec l'accelo
        ret = BIO_InjectMotionData(&fake_imu);
        if (ret == STATUS_OK) {
            printk("[IMU] Injection Fake IMU reussie (Z: %d)\n", fake_imu.accel_z);
        } else {
            printk("[IMU] Erreur d'injection: %d\n", ret);
        }

        /* B. Lecture des constantes vitales */
        ret = BIO_ReadData(&test_bio_data);
        if (ret == STATUS_OK) {
            // Note : Si ton doigt n'est pas sur le capteur, 
            // le MAX32664 va cracher 0 BPM / 0% SpO2 (c'est normal).
            if (test_bio_data.bpm > 0) {
                printk("[BIO DATA] ❤️ BPM  : %u\n", test_bio_data.bpm);
                printk("[BIO DATA] 🫁 SpO2 : %u %%\n", test_bio_data.spo2);
                printk("[BIO DATA] 📈 HRV  : %u ms\n", test_bio_data.hrv);
            } else {
                printk("[BIO DATA] Capteur libre (Place ton doigt sur la led rouge)\n");
            }
        } else {
            printk("[BIO DATA] Erreur I2C lors de la lecture des donnees: %d\n", ret);
            
            // Tentative de secours : On relance l'init si le capteur a decroche
            printk("[RECOVERY] Tentative de re-initialisation...\n");
            if (BIO_Init() == STATUS_OK) {
                printk("[RECOVERY] Capteur recupere !\n");
                BIO_SetMode(MODE_WALKING);
            }
        }

        // Fais varier légèrement la fausse donnée IMU pour simuler un petit tremblement
        fake_imu.accel_x += 10;
        if(fake_imu.accel_x > 500) fake_imu.accel_x = -500;

        k_msleep(TEST_DELAY_MS);
    }

    return 0;
}