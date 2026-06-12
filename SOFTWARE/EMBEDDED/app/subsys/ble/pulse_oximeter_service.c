/*
 * @file
 * @brief Pulse Oximeter Service (0x1822)
 * PLX Continuous Measurement characteristic (0x2B04)
 */

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(plx_service, CONFIG_LOG_DEFAULT_LEVEL);

static uint8_t spo2_current = 0;
static uint8_t pulse_current = 0;

/* PLX Continuous Measurement read callback */
static ssize_t plx_read_callback(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf, uint16_t len, uint16_t offset)
{
    uint8_t plx_data[6];
    
    /* Format: [flags(1)] [spo2(1)] [pulse_rate(2)] [reserved(2)] */
    plx_data[0] = 0x00;           /* Flags: normal status */
    plx_data[1] = spo2_current;   /* SpO2 percentage */
    plx_data[2] = pulse_current;  /* Pulse rate LSB */
    plx_data[3] = 0x00;           /* Pulse rate MSB */
    plx_data[4] = 0x00;           /* Reserved */
    plx_data[5] = 0x00;           /* Reserved */
    
    return bt_gatt_attr_read(conn, attr, buf, len, offset, plx_data, sizeof(plx_data));
}

/* Pulse Oximeter Service UUID (0x1822) */
static struct bt_uuid_128 plx_svc_uuid = BT_UUID_INIT_128(
    0x22, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* PLX Continuous Measurement UUID (0x2B04) */
static struct bt_uuid_128 plx_char_uuid = BT_UUID_INIT_128(
    0x04, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

BT_GATT_SERVICE_DEFINE(plx_svc,
    BT_GATT_PRIMARY_SERVICE(&plx_svc_uuid),
    BT_GATT_CHARACTERISTIC(&plx_char_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                          BT_GATT_PERM_READ,
                          plx_read_callback, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void PLX_SERVICE_Update(uint8_t spo2, uint8_t pulse_rate)
{
    spo2_current = spo2;
    pulse_current = pulse_rate;
    
    /* Send notification */
    uint8_t plx_data[6];
    plx_data[0] = 0x00;
    plx_data[1] = spo2;
    plx_data[2] = pulse_rate;
    plx_data[3] = 0x00;
    plx_data[4] = 0x00;
    plx_data[5] = 0x00;
    
    bt_gatt_notify(NULL, &plx_svc.attrs[1], plx_data, sizeof(plx_data));
}
