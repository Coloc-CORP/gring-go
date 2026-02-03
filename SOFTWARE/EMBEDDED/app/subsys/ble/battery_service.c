#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/bluetooth.h>

static uint8_t battery_level = 100;

/* Callback : Appelé quand David veut lire le niveau de batterie (READ) */
static ssize_t BAS_ReadLevel(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	uint8_t lvl = battery_level;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl, sizeof(lvl));
}

/* Définition du service standard BAS (Battery Service) */
BT_GATT_SERVICE_DEFINE(bas,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_BAS),
	BT_GATT_CHARACTERISTIC(BT_UUID_BAS_BATTERY_LEVEL,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, BAS_ReadLevel, NULL, &battery_level),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void BAS_NotifyLevel(uint8_t level)
{
    battery_level = level;
    /* Envoie la notification à tous les clients connectés */
    bt_gatt_notify(NULL, &bas.attrs[1], &battery_level, sizeof(battery_level));
}
