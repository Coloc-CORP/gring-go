#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hrs_service, CONFIG_LOG_DEFAULT_LEVEL);

static uint8_t hrs_bpm = 0;
static uint16_t hrs_hrv = 0;

/* Heart Rate Service UUID (0x180D) */
static struct bt_uuid_128 hrs_svc_uuid = BT_UUID_INIT_128(
    0x0d, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Heart Rate Measurement UUID (0x2A37) */
static struct bt_uuid_128 hrs_measurement_uuid = BT_UUID_INIT_128(
    0x37, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Definition du service Heart Rate (Standard SIG) */
BT_GATT_SERVICE_DEFINE(hrs_svc,
	BT_GATT_PRIMARY_SERVICE(&hrs_svc_uuid),
	BT_GATT_CHARACTERISTIC(&hrs_measurement_uuid,
			       BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void HRS_Notify(uint16_t bpm, uint16_t hrv_ms)
{
    /* Format standard HRS : 
       Byte 0 : Flags (0x10 -> HRV présent, BPM sur 8 bits ou 0x11 -> HRV présent, BPM sur 16 bits)
       Byte 1-2 : BPM
       Byte 3-4 : HRV (Intervalle RR en 1/1024s)
    */
    uint8_t hrs_data[5];
    hrs_data[0] = 0x10; /* Flags: 8-bit BPM + RR interval present */
    hrs_data[1] = (uint8_t)bpm;
    
    /* HRV sent as RR Interval (1/1024s units) */
    hrs_data[2] = (uint8_t)(hrv_ms & 0xFF);
    hrs_data[3] = (uint8_t)((hrv_ms >> 8) & 0xFF);
    hrs_data[4] = 0x00;

    bt_gatt_notify(NULL, &hrs_svc.attrs[1], hrs_data, 4);
    
    hrs_bpm = (uint8_t)bpm;
    hrs_hrv = hrv_ms;
}
