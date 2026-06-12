/*
 * @file
 * @brief Alert Notification Service (0x1811)
 * Send notifications for haptic feedback
 */

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ans_service, CONFIG_LOG_DEFAULT_LEVEL);

static uint8_t alert_level = 0;

/* Alert Level write callback */
static ssize_t alert_level_write(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                const void *buf, uint16_t len, uint16_t offset,
                                uint8_t flags)
{
    if (len == 1) {
        alert_level = *(uint8_t *)buf;
        LOG_INF("Alert Level: %d", alert_level);
        return len;
    }
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
}

static ssize_t alert_level_read(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &alert_level, sizeof(alert_level));
}

/* Alert Notification Service UUID (0x1811) */
static struct bt_uuid_128 ans_uuid = BT_UUID_INIT_128(
    0x11, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* New Alert UUID (0x2A46) */
static struct bt_uuid_128 new_alert_uuid = BT_UUID_INIT_128(
    0x46, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Alert Level UUID (0x2A45) */
static struct bt_uuid_128 alert_level_uuid = BT_UUID_INIT_128(
    0x45, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

BT_GATT_SERVICE_DEFINE(ans,
    BT_GATT_PRIMARY_SERVICE(&ans_uuid),
    
    BT_GATT_CHARACTERISTIC(&new_alert_uuid,
                          BT_GATT_CHRC_NOTIFY,
                          BT_GATT_PERM_NONE,
                          NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    
    BT_GATT_CHARACTERISTIC(&alert_level_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                          BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                          alert_level_read, alert_level_write, &alert_level),
);

void ANS_SendAlert(uint8_t alert_type, uint8_t alert_level_val)
{
    uint8_t alert_data[2];
    alert_data[0] = alert_type;   /* Type of alert */
    alert_data[1] = alert_level_val; /* Level intensity */
    
    bt_gatt_notify(NULL, &ans.attrs[1], alert_data, sizeof(alert_data));
}

uint8_t ANS_GetAlertLevel(void)
{
    return alert_level;
}
