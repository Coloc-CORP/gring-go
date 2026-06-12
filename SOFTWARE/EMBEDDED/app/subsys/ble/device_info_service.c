/*
 * @file
 * @brief Device Information Service (0x180A)
 * Multiple characteristics for system info, health activation, and low energy mode
 */

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(dis_service, CONFIG_LOG_DEFAULT_LEVEL);

/* Service state */
static uint8_t health_activation = 0;    /* 0x2b01 - Health activation status */
static uint8_t low_energy_activation = 0; /* 0x2b02 - Low energy mode status */
static const char *manufacturer = "Gringgo";

/* Manufacturer Name String read callback */
static ssize_t manufacturer_read(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           (void *)manufacturer, strlen(manufacturer));
}

/* Health Activation characteristic read/write */
static ssize_t health_activation_read(struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr,
                                     void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &health_activation, sizeof(health_activation));
}

static ssize_t health_activation_write(struct bt_conn *conn,
                                      const struct bt_gatt_attr *attr,
                                      const void *buf, uint16_t len, uint16_t offset,
                                      uint8_t flags)
{
    if (len == 1) {
        health_activation = *(uint8_t *)buf;
        LOG_INF("Health Activation: %d", health_activation);
        return len;
    }
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTR_LEN);
}

/* Low Energy Activation characteristic read/write */
static ssize_t low_energy_read(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &low_energy_activation, sizeof(low_energy_activation));
}

static ssize_t low_energy_write(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len, uint16_t offset,
                               uint8_t flags)
{
    if (len == 1) {
        low_energy_activation = *(uint8_t *)buf;
        LOG_INF("Low Energy Activation: %d", low_energy_activation);
        return len;
    }
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTR_LEN);
}

/* Device Information Service UUID (0x180A) */
static struct bt_uuid_128 dis_uuid = BT_UUID_INIT_128(
    0x0A, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Manufacturer Name String UUID (0x2A29) */
static struct bt_uuid_128 mfr_name_uuid = BT_UUID_INIT_128(
    0x29, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Health Activation UUID (0x2b01) */
static struct bt_uuid_128 health_act_uuid = BT_UUID_INIT_128(
    0x01, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

/* Low Energy Activation UUID (0x2b02) */
static struct bt_uuid_128 low_energy_uuid = BT_UUID_INIT_128(
    0x02, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb);

BT_GATT_SERVICE_DEFINE(dis,
    BT_GATT_PRIMARY_SERVICE(&dis_uuid),
    
    BT_GATT_CHARACTERISTIC(&mfr_name_uuid,
                          BT_GATT_CHRC_READ,
                          BT_GATT_PERM_READ,
                          manufacturer_read, NULL, NULL),
    
    BT_GATT_CHARACTERISTIC(&health_act_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                          BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                          health_activation_read, health_activation_write, &health_activation),
    
    BT_GATT_CHARACTERISTIC(&low_energy_uuid,
                          BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                          BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                          low_energy_read, low_energy_write, &low_energy_activation),
);

uint8_t DIS_GetHealthActivation(void)
{
    return health_activation;
}

uint8_t DIS_GetLowEnergyMode(void)
{
    return low_energy_activation;
}

void DIS_SetHealthActivation(uint8_t active)
{
    health_activation = active;
}

void DIS_SetLowEnergyMode(uint8_t active)
{
    low_energy_activation = active;
}
