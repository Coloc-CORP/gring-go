/*
 * @file
 * @brief Running Speed and Cadence Service (0x1814)
 * Characteristic 0x1068 for step count
 */

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rsc_service, CONFIG_LOG_DEFAULT_LEVEL);

static uint32_t steps_count_current = 0;

/* Step count read callback */
static ssize_t rsc_read_callback(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 void *buf, uint16_t len, uint16_t offset)
{
    uint8_t rsc_data[5];
    
    /* Format: [flags(1)] [steps(4 bytes, little-endian)] */
    rsc_data[0] = 0x00;  /* Flags */
    rsc_data[1] = (uint8_t)(steps_count_current & 0xFF);
    rsc_data[2] = (uint8_t)((steps_count_current >> 8) & 0xFF);
    rsc_data[3] = (uint8_t)((steps_count_current >> 16) & 0xFF);
    rsc_data[4] = (uint8_t)((steps_count_current >> 24) & 0xFF);
    
    return bt_gatt_attr_read(conn, attr, buf, len, offset, rsc_data, sizeof(rsc_data));
}

/* Running Speed and Cadence Service UUID (0x1814) */
static struct bt_uuid_128 rsc_svc_uuid = BT_UUID_INIT_128(
    0x14, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Step count characteristic UUID (0x1068) */
static struct bt_uuid_128 step_char_uuid = BT_UUID_INIT_128(
    0x68, 0x10, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

BT_GATT_SERVICE_DEFINE(rsc_svc,
    BT_GATT_PRIMARY_SERVICE(&rsc_svc_uuid),
    BT_GATT_CHARACTERISTIC(&step_char_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                          BT_GATT_PERM_READ,
                          rsc_read_callback, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void RSC_SERVICE_Update(uint32_t steps)
{
    steps_count_current = steps;
    
    /* Send notification */
    uint8_t rsc_data[5];
    rsc_data[0] = 0x00;
    rsc_data[1] = (uint8_t)(steps & 0xFF);
    rsc_data[2] = (uint8_t)((steps >> 8) & 0xFF);
    rsc_data[3] = (uint8_t)((steps >> 16) & 0xFF);
    rsc_data[4] = (uint8_t)((steps >> 24) & 0xFF);
    
    bt_gatt_notify(NULL, &rsc_svc.attrs[1], rsc_data, sizeof(rsc_data));
}
