package com.example.gringgo.ViewModel;
import java.util.UUID;

public class BluetoothUUIDs {
    // Services
    public static final UUID HEART_RATE_SERVICE_UUID =
            UUID.fromString("0000180d-0000-1000-8000-00805f9b34fb");
    public static final UUID HEALTH_THERMOMETER_SERVICE_UUID =
            UUID.fromString("00001809-0000-1000-8000-00805f9b34fb");
    public static final UUID PULSE_OXIMETER_SERVICE_UUID =
            UUID.fromString("00001822-0000-1000-8000-00805f9b34fb");
    public static final UUID RUNNING_SPEED_AND_CADENCE_SERVICE_UUID =
            UUID.fromString("00001814-0000-1000-8000-00805f9b34fb");
    public static final UUID BATTERY_SERVICE_UUID =
            UUID.fromString("0000180f-0000-1000-8000-00805f9b34fb");
    public static final UUID DEVICE_INFORMATION_SERVICE_UUID =
            UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static final UUID ALERT_NOTIFICATION_SERVICE_UUID =
            UUID.fromString("00001811-0000-1000-8000-00805f9b34fb");

    // Caractéristiques - CORRIGER LES UUIDs DUPLIQUÉS
    public static final UUID HEART_RATE_MEASUREMENT_CHARACTERISTIC_UUID =
            UUID.fromString("00002a37-0000-1000-8000-00805f9b34fb");
    public static final UUID HEALTH_THERMOMETER_MEASUREMENT_CHARACTERISTIC_UUID =
            UUID.fromString("00002b03-0000-1000-8000-00805f9b34fb"); // ← NOUVEAU
    public static final UUID PULSE_OXIMETER_CHARACTERISTIC_UUID =
            UUID.fromString("00002b04-0000-1000-8000-00805f9b34fb"); // ← CHANGÉ de 00002a5f
    public static final UUID RUNNING_SPEED_AND_CADENCE_CHARACTERISTIC_UUID =
            UUID.fromString("00001068-0000-1000-8000-00805f9b34fb"); // ← NOUVEAU
    public static final UUID BATTERY_LEVEL_CHARACTERISTIC_UUID =
            UUID.fromString("00002a19-0000-1000-8000-00805f9b34fb");
    public static final UUID ALERT_LEVEL_CHARACTERISTIC_UUID =
            UUID.fromString("00002a45-0000-1000-8000-00805f9b34fb"); // ← NOUVEAU

    // Perso (cf. ton code C)
    public static final UUID HEALTH_ACTIVATION_CHARACTERISTIC_UUID =
            UUID.fromString("00002b01-0000-1000-8000-00805f9b34fb");
    public static final UUID LE_ACTIVATION_CHARACTERISTIC_UUID =
            UUID.fromString("00002b02-0000-1000-8000-00805f9b34fb");

    // Service DIS (Device Information Service) où tu as placé tes caractéristiques
    public static final UUID DIS_SERVICE_UUID = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");

    // Tes caractéristiques custom (0x2B01 et 0x2B02)
    public static final UUID HEALTH_ACT_CHARACTERISTIC_UUID = UUID.fromString("00002b01-0000-1000-8000-00805f9b34fb");
    public static final UUID LOW_ENERGY_CHARACTERISTIC_UUID = UUID.fromString("00002b02-0000-1000-8000-00805f9b34fb");
}