/*
 * @file
 * @brief Main application for Gringgo BLE ring
 * Reads sensor data and transmits via Bluetooth GATT services
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

#include "temp_driver.h"
#include "bio_driver.h"
#include "imu_driver.h"
#include "charge_driver.h"
#include "haptic_driver.h"

#include "temperature_service.h"
#include "pulse_oximeter_service.h"
#include "speed_cadence_service.h"
#include "health_service.h"
#include "battery_service.h"
#include "device_info_service.h"
#include "alert_notification_service.h"

LOG_MODULE_REGISTER(gringgo_main, CONFIG_LOG_DEFAULT_LEVEL);

/* ===== Bluetooth Configuration ===== */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_AD_FLAGS, (BT_AD_GENERAL | BT_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_AD_UUID16_ALL,
                  0x0d, 0x18, /* Heart Rate Service */
                  0x09, 0x18, /* Health Thermometer */
                  0x22, 0x18, /* Pulse Oximeter */
                  0x14, 0x18, /* Running Speed/Cadence */
                  0x0a, 0x18, /* Device Information */
                  0x11, 0x18, /* Alert Notification */
                  0x0f, 0x18),/* Battery */
    BT_DATA_BYTES(BT_AD_NAME_COMPLETE, 'G', 'r', 'i', 'n', 'g', 'g', 'o'),
};

/* ===== Bluetooth Connection Callbacks ===== */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed (err 0x%02x)", err);
    } else {
        LOG_INF("Connected!");
        HAPTIC_PlayPattern(ALERT_VIB_INCOMING_CALL); /* Haptic feedback on connection */
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected (reason 0x%02x)", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

/* ===== Sensor Data Structure ===== */
static sensors_data_t sensor_data = {
    .bio = {.bpm = 0, .spo2 = 0, .hrv = 0},
    .imu = {.steps_count = 0},
    .charge = {.battery_level = 100, .is_charging = false, .voltage_mv = 0},
    .temp_c = 25.0f,
};

/* ===== Sensor Reading Task ===== */
#define SENSOR_STACK_SIZE 2048
#define SENSOR_PRIORITY 7

static void sensor_thread(void *p1, void *p2, void *p3)
{
    LOG_INF("Sensor thread started");

    /* Sensor initialization */
    int ret;
    
    LOG_INF("Initializing temperature sensor...");
    ret = TEMP_Init();
    if (ret != STATUS_OK) {
        LOG_ERR("Temperature sensor init failed: %d", ret);
    }

    LOG_INF("Initializing biometric sensor...");
    ret = BIO_Init();
    if (ret != STATUS_OK) {
        LOG_ERR("Biometric sensor init failed: %d", ret);
    }

    LOG_INF("Initializing IMU...");
    ret = IMU_Init();
    if (ret != STATUS_OK) {
        LOG_ERR("IMU init failed: %d", ret);
    }

    LOG_INF("Initializing charge controller...");
    ret = CHG_Init();
    if (ret != STATUS_OK) {
        LOG_ERR("Charge controller init failed: %d", ret);
    }

    LOG_INF("Initializing haptic driver...");
    ret = HAPTIC_Init();
    if (ret != STATUS_OK) {
        LOG_ERR("Haptic driver init failed: %d", ret);
    }

    /* Enable biometric sensor measurements */
    BIO_SetMode(MODE_WALKING);
    IMU_EnableSensor(BHI260_SENSOR_ID_STEP_COUNTER, 1.0f);

    LOG_INF("All sensors initialized successfully");

    /* Main sensor reading loop */
    while (1) {
        /* Read Temperature */
        ret = TEMP_ReadTemperature(&sensor_data.temp_c);
        if (ret == STATUS_OK) {
            LOG_INF("Temp: %.2f°C", sensor_data.temp_c);
            TEMP_SERVICE_Update(sensor_data.temp_c);
        }

        /* Read Biometric Data */
        ret = BIO_ReadData(&sensor_data.bio);
        if (ret == STATUS_OK) {
            LOG_INF("BPM: %d, SpO2: %d%%, HRV: %d", 
                    sensor_data.bio.bpm, sensor_data.bio.spo2, sensor_data.bio.hrv);
            HRS_Notify(sensor_data.bio.bpm, sensor_data.bio.hrv);
            PLX_SERVICE_Update(sensor_data.bio.spo2, sensor_data.bio.bpm);
        }

        /* Read IMU Data */
        ret = IMU_ReadData(&sensor_data);
        if (ret == STATUS_OK) {
            LOG_INF("Steps: %d", sensor_data.imu.steps_count);
            RSC_SERVICE_Update(sensor_data.imu.steps_count);
        }

        /* Read Charge Status */
        ret = CHG_GetStatus((charge_state_t *)&sensor_data.charge.is_charging);
        if (ret == STATUS_OK) {
            LOG_INF("Charge: %d%%", sensor_data.charge.battery_level);
            BAS_NotifyLevel(sensor_data.charge.battery_level);
        }

        /* Sleep before next read */
        k_msleep(1000); /* 1 second interval */
    }
}

K_THREAD_DEFINE(sensor_tid, SENSOR_STACK_SIZE, sensor_thread, NULL, NULL, NULL,
                SENSOR_PRIORITY, 0, K_NO_WAIT);

/* ===== Main Application ===== */
int main(void)
{
    int err;

    LOG_INF("\n--- Gringgo BLE Ring ---\n");

    /* Initialize Bluetooth subsystem */
    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return err;
    }
    LOG_INF("Bluetooth initialized");

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }
    LOG_INF("Advertising started");

    /* Main loop */
    while (1) {
        k_msleep(10000); /* 10 second tick */
    }

    return 0;
}
