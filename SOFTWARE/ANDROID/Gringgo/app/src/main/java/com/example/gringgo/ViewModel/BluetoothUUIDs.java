package com.example.gringgo.ViewModel;
import java.util.UUID;

public class BluetoothUUIDs {
    // Services standard
    public static final UUID HEART_RATE_SERVICE_UUID =
            UUID.fromString("0000180d-0000-1000-8000-00805f9b34fb");
    public static final UUID RUNNING_SPEED_AND_CADENCE_SERVICE_UUID =
            UUID.fromString("00001814-0000-1000-8000-00805f9b34fb");
    public static final UUID PULSE_OXIMETER_SERVICE_UUID =
            UUID.fromString("00001822-0000-1000-8000-00805f9b34fb");
    public static final UUID BATTERY_SERVICE_UUID =
            UUID.fromString("0000180f-0000-1000-8000-00805f9b34fb");
    public static final UUID DEVICE_INFORMATION_SERVICE_UUID =
            UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static final UUID ALERT_NOTIFICATION_SERVICE_UUID =
            UUID.fromString("00001811-0000-1000-8000-00805f9b34fb");
    public static final UUID HEALTH_THERMOMETER_SERVICE_UUID =
            UUID.fromString("00001809-0000-1000-8000-00805f9b34fb");

    // Caractéristiques standard
    public static final UUID HEART_RATE_MEASUREMENT_CHARACTERISTIC_UUID =
            UUID.fromString("00002a37-0000-1000-8000-00805f9b34fb");
    public static final UUID STEP_COUNT_CHARACTERISTIC_UUID =
            UUID.fromString("00002a53-0000-1000-8000-00805f9b34fb");
    public static final UUID BATTERY_LEVEL_CHARACTERISTIC_UUID =
            UUID.fromString("00002A19-0000-1000-8000-00805f9b34fb");
    public static final UUID NOTIFICATION_CHARACTERISTIC_UUID =
            UUID.fromString("00002A19-0000-1000-8000-00805f9b34fb");
    public static final UUID TEMPERATURE_CHARACTERISTIC_UUID =
            UUID.fromString("00002A19-0000-1000-8000-00805f9b34fb");
    public static final UUID PULSE_OXIMETER_CHARACTERISTIC_UUID =
            UUID.fromString("00002a5f-0000-1000-8000-00805f9b34fb");
    public static final UUID MANUFACTURER_NAME_CHARACTERISTIC_UUID =
            UUID.fromString("00002A19-0000-1000-8000-00805f9b34fb");

    // Caractéristiques personnalisées
    public static final UUID HEALTH_ACTIVATION_CHARACTERISTIC_UUID =
            UUID.fromString("00002b01-0000-1000-8000-00805f9b34fb");
    public static final UUID LE_ACTIVATION_CHARACTERISTIC_UUID =
            UUID.fromString("00002b02-0000-1000-8000-00805f9b34fb");
}