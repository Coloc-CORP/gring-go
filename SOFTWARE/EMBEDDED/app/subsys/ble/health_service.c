#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

static uint8_t hrs_bpm = 0;

/* Definition du service Heart Rate (Standard SIG) */
BT_GATT_SERVICE_DEFINE(hrs_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_HRS),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT,
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
    hrs_data[0] = 0x10; // On indique que le BPM est sur 8 bits et qu'on ajoute l'intervalle RR (HRV)
    hrs_data[1] = (uint8_t)bpm;
    
    /* Le HRV en Bluetooth est souvent envoyé en format "RR Interval" */
    hrs_data[2] = (uint8_t)(hrv_ms & 0xFF);
    hrs_data[3] = (uint8_t)((hrv_ms >> 8) & 0xFF);

    bt_gatt_notify(NULL, &hrs_svc.attrs[1], hrs_data, 4);
}
