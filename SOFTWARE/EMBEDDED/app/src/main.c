#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <dk_buttons_and_leds.h>

#define DEVICE_NAME             "nrf"
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED          DK_LED1
#define CON_STATUS_LED          DK_LED2
#define SIMUL_INTERVAL_MS       1000
#define LOW_ENERGY_INTERVAL_MS  5000 // Intervalle quand Low Energy est activé (5 secondes)

/* --- Variables Globales de Simulation --- */
static uint8_t sim_bpm = 70;
static uint8_t sim_spo2 = 98;
static uint32_t sim_steps = 1250;
static uint8_t sim_battery = 85;
static uint8_t sim_temp = 36; 
static uint8_t last_notified_battery = 85; // Initialisée à la même valeur que sim_battery

// Variables pour stocker les états d'activation (reçus de l'appli Android)
static uint8_t health_activation_state = 0;
static uint8_t low_energy_state = 0;
static uint8_t haptic_alert_level = 0;

/* --- Déclaration des UUIDs GATT (16-bit) --- */
#define UUID_SERV_HRS             0x180D
#define UUID_CHAR_HRS_MEAS        0x2A37
#define UUID_SERV_HTS             0x1809
#define UUID_CHAR_HTS_MEAS        0x2B03
#define UUID_SERV_PLX             0x1822
#define UUID_CHAR_PLX_MEAS        0x2B04
#define UUID_SERV_RSC             0x1814
#define UUID_CHAR_RSC_MEAS        0x1068
#define UUID_SERV_DIS             0x180A
#define UUID_CHAR_HEALTH_ACT      0x2B01
#define UUID_CHAR_LOW_EN_ACT      0x2B02
#define UUID_CHAR_MANUF_NAME      0x2A29
#define UUID_SERV_ANS             0x1811
#define UUID_CHAR_ALERT_LEVEL     0x2A45
#define UUID_SERV_BAS             0x180F
#define UUID_CHAR_BATT_LEVEL      0x2A19

/* --- Callbacks d'écriture de l'application Android --- */
static ssize_t write_health_act(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    health_activation_state = ((uint8_t *)buf)[0];
    printk("[GATT WRITE] Health Activation modifie par Android : %u\n", health_activation_state);
    return len;
}

static ssize_t write_low_en(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    low_energy_state = ((uint8_t *)buf)[0];
    printk("[GATT WRITE] Low Energy Activation modifie par Android : %u\n", low_energy_state);
    return len;
}

static ssize_t write_haptic(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    haptic_alert_level = ((uint8_t *)buf)[0];
    printk("[GATT WRITE] Alerte Haptique recue depuis Android ! Niveau : %u\n", haptic_alert_level);
    return len;
}

/* --- DÉFINITION DE LA TABLE DES SERVICES GATT DE GRING-GO --- */
BT_GATT_SERVICE_DEFINE(gringgo_svc,
    /* 1. Heart Rate Service (BPM) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_HRS)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_HRS_MEAS), 
                           BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* 2. Health Thermometer Service (Température) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_HTS)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_HTS_MEAS), 
                           BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* 3. Pulse Oximeter Service (SpO2) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_PLX)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_PLX_MEAS), 
                           BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* 4. Running Speed and Cadence Service (Steps) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_RSC)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_RSC_MEAS), 
                           BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* 5. Device Information Service (DIS) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_DIS)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_MANUF_NAME), 
                           BT_GATT_CHRC_READ, BT_GATT_PERM_READ, NULL, NULL, "Gring-Go Team"),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_HEALTH_ACT), 
                           BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_health_act, &health_activation_state),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_LOW_EN_ACT), 
                           BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_low_en, &low_energy_state),

    /* 6. Alert Notification Service (Haptic Return) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_ANS)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_ALERT_LEVEL), 
                           BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_haptic, &haptic_alert_level),

    /* 7. Battery Service (BAS) */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(UUID_SERV_BAS)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(UUID_CHAR_BATT_LEVEL), 
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, NULL, NULL, &sim_battery),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/* --- Configuration de l'Advertising --- */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 
                  BT_UUID_16_ENCODE(UUID_SERV_HRS), 
                  BT_UUID_16_ENCODE(UUID_SERV_HTS), 
                  BT_UUID_16_ENCODE(UUID_SERV_BAS)),
};

/* --- Callbacks Connexion Bluetooth --- */
static void connected(struct bt_conn *conn, uint8_t err) {
    if (err) { 
        printk("[STATUS] Echec connexion BLE (err %u)\n", err); 
        return; 
    }
    printk("[STATUS] Connecte a l'application Android !\n");
    dk_set_led_on(CON_STATUS_LED);
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    printk("[STATUS] Deconnecte (Raison : %u)\n", reason);
    dk_set_led_off(CON_STATUS_LED);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

void notify_bpm(uint8_t bpm_val) {
    uint8_t hrm_data[2] = {0x00, bpm_val}; 
    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[2], hrm_data, sizeof(hrm_data));
    if (err && err != -ENOTCONN) printk("[NOTIFY ERR] BPM (err %d)\n", err);
    else if (!err) printk("[NOTIFY SUCCESS] BPM (%u) transmis\n", bpm_val);
}

void notify_temperature(uint8_t temp_val) {
    uint8_t hts_data[2] = {0x00, temp_val}; 
    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[6], hts_data, sizeof(hts_data));
    if (err && err != -ENOTCONN) printk("[NOTIFY ERR] TEMP (err %d)\n", err);
    else if (!err) printk("[NOTIFY SUCCESS] TEMP (%u°C) transmis\n", temp_val);
}

void notify_spo2(uint8_t spo2_val) {
    uint8_t plx_data[2] = {0x00, spo2_val}; 
    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[10], plx_data, sizeof(plx_data));
    if (err && err != -ENOTCONN) printk("[NOTIFY ERR] SPO2 (err %d)\n", err);
    else if (!err) printk("[NOTIFY SUCCESS] SPO2 (%u%%) transmis\n", spo2_val);
}

void notify_steps(uint32_t steps_val) {
    uint8_t step_data[4];
    sys_put_le32(steps_val, step_data); 
    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[14], step_data, sizeof(step_data));
    if (err && err != -ENOTCONN) printk("[NOTIFY ERR] STEPS (err %d)\n", err);
    else if (!err) printk("[NOTIFY SUCCESS] STEPS (%u) transmis\n", steps_val);
}

void notify_battery(uint8_t batt_val) {
    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[28], &batt_val, sizeof(batt_val));
    if (err && err != -ENOTCONN) printk("[NOTIFY ERR] BATT (err %d)\n", err);
    else if (!err) printk("[NOTIFY SUCCESS] BATT (%u%%) transmis\n", batt_val);
}

int main(void)
{
    int blink_status = 0;
    int err;

    printk("\n--- INITIALISATION DU SYSTEME SIMULE ---\n");

    err = dk_leds_init();
    if (err) {
        printk("[ERR] Initialisation des LEDs impossible (err %d)\n", err);
        return 0;
    }
    printk("[INIT] LEDs initialisees avec succes\n");

    err = bt_enable(NULL);
    if (err) { 
        printk("[ERR] Initialisation du Bluetooth echouee (err %d)\n", err); 
        return 0; 
    }
    printk("[INIT] Stack Bluetooth activee\n");

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) { 
        printk("[ERR] Lancement de l'Advertising echoue (err %d)\n", err); 
        return 0; 
    }
    printk("[INIT] Advertising lance avec succes, en attente du smartphone...\n");

    for (;;) {
        dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);

        printk("\n[SIMUL] --- Generation Top %d ---", blink_status);
        printk(" | Sante: %s | Low Energy: %s\n",
            (health_activation_state == 1) ? "ON" : "OFF",
            (low_energy_state == 1) ? "ON" : "OFF");

        /* --- LOGIQUE 1 : MODE SANTÉ --- */
        // Mise à jour des variables de simulation (toujours active)
        if (health_activation_state == 1) {
            sim_bpm++; if(sim_bpm > 110) sim_bpm = 65;
            sim_steps += 2;
            sim_spo2 = (blink_status % 20 == 0) ? 97 : 99;
            sim_temp = (blink_status % 10 == 0) ? 37 : 36;
        }

        /* --- LOGIQUE 2 : ENVOI DES NOTIFICATIONS --- */
        // On n'envoie les notifications que si :
        // - Le mode Santé est activé ET le mode Low Energy est désactivé
        if (health_activation_state == 1 && low_energy_state == 0) {
            notify_bpm(sim_bpm);
            notify_temperature(sim_temp);
            notify_spo2(sim_spo2);
            notify_steps(sim_steps);
        } else if (low_energy_state == 1) {
            printk("[SIMUL] Mode Low Energy actif : notifications des capteurs désactivées.\n");
        } else {
            printk("[SIMUL] Suivi Santé désactivé, capteurs en veille.\n");
        }

        /* --- LA BATTERIE --- */
        // La batterie est toujours envoyée, même en Low Energy
        if (blink_status % 60 == 0 && sim_battery > 5) {
            sim_battery--;
            notify_battery(sim_battery); // notify_battery vérifie si la valeur a changé
        }

        /* --- LOGIQUE 3 : MODE LOW ENERGY --- */
        // On ajuste le temps de pause en fonction de l'état du Low Energy
        int current_delay = (low_energy_state == 1) ? LOW_ENERGY_INTERVAL_MS : SIMUL_INTERVAL_MS;
        k_sleep(K_MSEC(current_delay));
    }

    void notify_battery(uint8_t batt_val) {
    // Ne pas envoyer de notification si la valeur n'a pas changé
    if (batt_val == last_notified_battery) {
        return;
    }

    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[28], &batt_val, sizeof(batt_val));
    if (err && err != -ENOTCONN) {
        printk("[NOTIFY ERR] BATT (err %d)\n", err);
    } else if (!err) {
        printk("[NOTIFY SUCCESS] BATT (%u%%) transmis\n", batt_val);
        last_notified_battery = batt_val; // Mettre à jour la dernière valeur envoyée
    }
}
}