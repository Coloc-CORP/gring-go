#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <dk_buttons_and_leds.h>

#include "gringgo_ble.h"
#include "sensors_wrapper.h"
#include "gringgo_types.h"
#include "haptic_driver.h"

#define DEVICE_NAME             "nrf"
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

uint8_t health_activation_state = 0;
uint8_t low_energy_state = 0;
uint8_t haptic_alert_level = 0;

static uint8_t last_notified_battery = 85;

/* --- Callbacks d'écriture Android --- */
static ssize_t write_health_act(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    health_activation_state = ((uint8_t *)buf)[0];
    return len;
}

static ssize_t write_low_en(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    low_energy_state = ((uint8_t *)buf)[0];
    return len;
}

static ssize_t write_haptic(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (len != 1) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    
    haptic_alert_level = ((uint8_t *)buf)[0];
    printk("[GATT ANS] Ordre recu depuis Android pour le vibreur ! Effet ID : %u\n", haptic_alert_level);

    /* --- DÉCLENCHEMENT DU MOTEUR --- */
#if !SIMUL_HAPTIC
    // Si le moteur physique est activé, on lui envoie la commande I2C
    HAPTIC_PlayPattern(haptic_alert_level);
#else
    // Si on est en simulation, on affiche juste un retour visuel dans la console
    printk("[SIMUL] Bzz Bzz ! (Vibration simulée, effet %u joué)\n", haptic_alert_level);
#endif

    return len;
}

/* --- Table GATT --- */
BT_GATT_SERVICE_DEFINE(gringgo_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x180D)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A37), BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x1809)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2B03), BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x1822)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2B04), BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x1814)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x1068), BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x180A)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A29), BT_GATT_CHRC_READ, BT_GATT_PERM_READ, NULL, NULL, "Gring-Go Team"),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2B01), BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_health_act, &health_activation_state),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2B02), BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_low_en, &low_energy_state),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x1811)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A45), BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_haptic, &haptic_alert_level),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x180F)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A19), BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, NULL, NULL, &global_sensors_data.charge.battery_level),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/* --- Envois Intelligents Adaptatifs --- */
void notify_bpm(uint8_t bpm_val) {
    uint8_t hrm_data[2] = {0x00, bpm_val};
    if (bt_gatt_notify(NULL, &gringgo_svc.attrs[2], hrm_data, sizeof(hrm_data)) == 0) {
        printk("[GATT NOTIFY] BPM envoyé : %u\n", bpm_val);
    }
}

void notify_temperature(uint8_t temp_val) {
    uint8_t hts_data[2] = {0x00, temp_val};
    if (bt_gatt_notify(NULL, &gringgo_svc.attrs[6], hts_data, sizeof(hts_data)) == 0) {
        printk("[GATT NOTIFY] Température envoyée : %u°C\n", temp_val);
    }
}

void notify_spo2(uint8_t spo2_val) {
    uint8_t plx_data[2] = {0x00, spo2_val};
    if (bt_gatt_notify(NULL, &gringgo_svc.attrs[10], plx_data, sizeof(plx_data)) == 0) {
        printk("[GATT NOTIFY] SpO2 envoyée : %u%%\n", spo2_val);
    }
}

void notify_steps(uint32_t steps_val) {
    uint8_t step_data[4];
    sys_put_le32(steps_val, step_data);
    if (bt_gatt_notify(NULL, &gringgo_svc.attrs[14], step_data, sizeof(step_data)) == 0) {
        printk("[GATT NOTIFY] Pas envoyés : %u\n", steps_val);
    }
}

/* Modifie notify_battery pour intégrer la logique de ton collègue */
void notify_battery(uint8_t batt_val) {
    if (batt_val == last_notified_battery) {
        return; // Ne pas envoyer si la valeur n'a pas changé
    }

    int err = bt_gatt_notify(NULL, &gringgo_svc.attrs[28], &batt_val, sizeof(batt_val));
    if (err && err != -ENOTCONN) {
        printk("[NOTIFY ERR] BATT (err %d)\n", err);
    } else if (!err) {
        printk("[GATT NOTIFY] BATT (%u%%) transmis\n", batt_val);
        last_notified_battery = batt_val;
    }
}

/* Ajoute cette nouvelle fonction TOUT EN BAS du fichier */
void gringgo_ble_update(int blink_status) {
    /* LOGIQUE 2 : ENVOI DES NOTIFICATIONS */
    if (health_activation_state == 1 && low_energy_state == 0) {
        notify_bpm(global_sensors_data.bio.bpm);
        notify_temperature((uint8_t)global_sensors_data.temp_c);
        notify_spo2(global_sensors_data.bio.spo2);
        notify_steps(global_sensors_data.imu.steps_count);
    } else if (low_energy_state == 1) {
        printk("[BLE] Mode Low Energy actif : notifications des capteurs desactivees.\n");
    } else {
        printk("[BLE] Suivi Sante desactive, capteurs en veille.\n");
    }

    notify_battery(global_sensors_data.charge.battery_level);
}

/* --- Bluetooth Callbacks & Adv --- */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(0x180D), BT_UUID_16_ENCODE(0x1809), BT_UUID_16_ENCODE(0x180F)),
};
static void connected(struct bt_conn *conn, uint8_t err) {
    if (!err) dk_set_led_on(DK_LED2);
}
static void disconnected(struct bt_conn *conn, uint8_t reason) {
    dk_set_led_off(DK_LED2);
}
BT_CONN_CB_DEFINE(conn_callbacks) = { .connected = connected, .disconnected = disconnected };

int gringgo_ble_init(void) {
    int err = bt_enable(NULL);
    if (err) return err;
    return bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
}