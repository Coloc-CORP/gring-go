/*
 * @file
 * @brief Health Thermometer Service (0x1809)
 * Temperature characteristic (0x2B03) with custom format
 */

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(temp_service, CONFIG_LOG_DEFAULT_LEVEL);

static float current_temperature = 25.0f;
static struct bt_gatt_attr *temp_attr_ref = NULL;

/* Temperature characteristic read callback */
static ssize_t temp_read_callback(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset)
{
    uint8_t temp_data[5];
    int16_t temp_int = (int16_t)(current_temperature * 100.0f);
    
    /* Format: [flags(1)] [temp_int16_LSB] [temp_int16_MSB] [unit] [reserved] */
    temp_data[0] = 0x00;  /* Celsius, no timestamp */
    temp_data[1] = (uint8_t)(temp_int & 0xFF);
    temp_data[2] = (uint8_t)((temp_int >> 8) & 0xFF);
    temp_data[3] = 0x00;  /* Unit: Celsius */
    temp_data[4] = 0x00;  /* Reserved */
    
    return bt_gatt_attr_read(conn, attr, buf, len, offset, temp_data, sizeof(temp_data));
}

/* Health Thermometer Service definition */
static struct bt_uuid_128 hts_uuid = BT_UUID_INIT_128(
    0x09, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Temperature Measurement characteristic (0x2B03) */
static struct bt_uuid_128 temp_char_uuid = BT_UUID_INIT_128(
    0x03, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

BT_GATT_SERVICE_DEFINE(hts,
    BT_GATT_PRIMARY_SERVICE(&hts_uuid),
    BT_GATT_CHARACTERISTIC(&temp_char_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                          BT_GATT_PERM_READ,
                          temp_read_callback, NULL, &current_temperature),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void TEMP_SERVICE_Update(float temperature)
{
    current_temperature = temperature;
    
    /* Send notification to all connected clients */
    uint8_t temp_data[5];
    int16_t temp_int = (int16_t)(temperature * 100.0f);
    
    temp_data[0] = 0x00;
    temp_data[1] = (uint8_t)(temp_int & 0xFF);
    temp_data[2] = (uint8_t)((temp_int >> 8) & 0xFF);
    temp_data[3] = 0x00;
    temp_data[4] = 0x00;
    
    bt_gatt_notify(NULL, &hts.attrs[1], temp_data, sizeof(temp_data));
}
