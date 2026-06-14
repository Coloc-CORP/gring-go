package com.example.gringgo.ViewModel;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.util.Log;
import androidx.annotation.NonNull;
import no.nordicsemi.android.ble.BleManager;
import no.nordicsemi.android.ble.observer.ConnectionObserver;

public class Nrf5340Manager extends BleManager {

    private BluetoothGattCharacteristic hrsChar, basChar, posChar, stepChar, tempChar, healthActChar, lowEnergyActChar;

    public interface OnGattDataListener {
        void onHeartRateReceived(int bpm);
        void onBatteryReceived(int percent);
        void onTemperatureReceived(int tempC);
        void onPulseOxReceived(int spo2);
        void onStepsReceived(int steps);
    }

    public interface OnConnectionStatusChangedListener {
        void onStatusChanged(String state);
    }

    private OnConnectionStatusChangedListener connectionListener;
    private OnGattDataListener dataListener;

    public void setOnGattDataListener(OnGattDataListener listener) {
        this.dataListener = listener;
    }

    public void setOnConnectionStatusChangedListener(OnConnectionStatusChangedListener listener) {
        this.connectionListener = listener;
    }

    public Nrf5340Manager(@NonNull Context context) {
        super(context);

        setConnectionObserver(new ConnectionObserver() {
            @Override
            public void onDeviceConnecting(@NonNull BluetoothDevice device) {
                if (connectionListener != null) connectionListener.onStatusChanged("CONNECTING");
            }

            @Override
            public void onDeviceConnected(@NonNull BluetoothDevice device) { }

            @Override
            public void onDeviceFailedToConnect(@NonNull BluetoothDevice device, int reason) {
                if (connectionListener != null) connectionListener.onStatusChanged("DISCONNECTED");
            }

            @Override
            public void onDeviceReady(@NonNull BluetoothDevice device) {
                setupGattStreams();
                if (connectionListener != null) connectionListener.onStatusChanged("CONNECTED");
            }

            @Override
            public void onDeviceDisconnecting(@NonNull BluetoothDevice device) { }

            @Override
            public void onDeviceDisconnected(@NonNull BluetoothDevice device, int reason) {
                if (connectionListener != null) connectionListener.onStatusChanged("DISCONNECTED");
            }
        });
    }

    private void setupGattStreams() {
        // Heart Rate (BPM) - Format: [0x00, BPM_VALUE]
        if (hrsChar != null) {
            setNotificationCallback(hrsChar).with((dev, data) -> {
                int bpm = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 1);
                Log.d("BLE_DATA", "BPM reçu: " + bpm);
                if (dataListener != null) dataListener.onHeartRateReceived(bpm);
            });
            enableNotifications(hrsChar).enqueue();
        }

        // Temperature - Format: [0x00, TEMP_VALUE]
        if (tempChar != null) {
            setNotificationCallback(tempChar).with((dev, data) -> {
                int temp = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 1);
                Log.d("BLE_DATA", "Temperature reçue: " + temp + "°C");
                if (dataListener != null) dataListener.onTemperatureReceived(temp);
            });
            enableNotifications(tempChar).enqueue();
        }

        // Pulse Oximeter (SpO2) - Format: [0x00, SPO2_VALUE]
        if (posChar != null) {
            setNotificationCallback(posChar).with((dev, data) -> {
                int spo2 = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 1);
                Log.d("BLE_DATA", "SpO2 reçu: " + spo2 + "%");
                if (dataListener != null) dataListener.onPulseOxReceived(spo2);
            });
            enableNotifications(posChar).enqueue();
        }

        // Steps - Format: UINT32 little-endian [STEP0, STEP1, STEP2, STEP3]
        if (stepChar != null) {
            setNotificationCallback(stepChar).with((dev, data) -> {
                int steps = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT32, 0);
                Log.d("BLE_DATA", "Steps reçus: " + steps);
                if (dataListener != null) dataListener.onStepsReceived(steps);
            });
            enableNotifications(stepChar).enqueue();
        }

        // Battery - Format: [BATTERY_PERCENT]
        if (basChar != null) {
            setNotificationCallback(basChar).with((dev, data) -> {
                int bat = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 0);
                Log.d("BLE_DATA", "Battery reçue: " + bat + "%");
                if (dataListener != null) dataListener.onBatteryReceived(bat);
            });
            enableNotifications(basChar).enqueue();
        }
    }

    @Override
    protected boolean isRequiredServiceSupported(@NonNull BluetoothGatt gatt) {
        final BluetoothGattService hrs = gatt.getService(BluetoothUUIDs.HEART_RATE_SERVICE_UUID);
        final BluetoothGattService hts = gatt.getService(BluetoothUUIDs.HEALTH_THERMOMETER_SERVICE_UUID);
        final BluetoothGattService bas = gatt.getService(BluetoothUUIDs.BATTERY_SERVICE_UUID);
        final BluetoothGattService pos = gatt.getService(BluetoothUUIDs.PULSE_OXIMETER_SERVICE_UUID);
        final BluetoothGattService rsc = gatt.getService(BluetoothUUIDs.RUNNING_SPEED_AND_CADENCE_SERVICE_UUID);
        final BluetoothGattService dis = gatt.getService(BluetoothUUIDs.DIS_SERVICE_UUID);



        if (hrs != null) hrsChar = hrs.getCharacteristic(BluetoothUUIDs.HEART_RATE_MEASUREMENT_CHARACTERISTIC_UUID);
        if (hts != null) tempChar = hts.getCharacteristic(BluetoothUUIDs.HEALTH_THERMOMETER_MEASUREMENT_CHARACTERISTIC_UUID);
        if (bas != null) basChar = bas.getCharacteristic(BluetoothUUIDs.BATTERY_LEVEL_CHARACTERISTIC_UUID);
        if (pos != null) posChar = pos.getCharacteristic(BluetoothUUIDs.PULSE_OXIMETER_CHARACTERISTIC_UUID);
        if (rsc != null) stepChar = rsc.getCharacteristic(BluetoothUUIDs.RUNNING_SPEED_AND_CADENCE_CHARACTERISTIC_UUID);
        if (dis != null) {
            healthActChar = dis.getCharacteristic(BluetoothUUIDs.HEALTH_ACT_CHARACTERISTIC_UUID);
            lowEnergyActChar = dis.getCharacteristic(BluetoothUUIDs.LOW_ENERGY_CHARACTERISTIC_UUID);
        }
        // DEBUG
        Log.d("BLE_DEBUG", "HRS Service: " + (hrs != null) + " | Char: " + (hrsChar != null));
        Log.d("BLE_DEBUG", "HTS Service: " + (hts != null) + " | Char: " + (tempChar != null));  // ← À VÉRIFIER
        Log.d("BLE_DEBUG", "BAS Service: " + (bas != null) + " | Char: " + (basChar != null));  // ← À VÉRIFIER
        Log.d("BLE_DEBUG", "PLX Service: " + (pos != null) + " | Char: " + (posChar != null));
        Log.d("BLE_DEBUG", "RSC Service: " + (rsc != null) + " | Char: " + (stepChar != null));

        return hrsChar != null || basChar != null || tempChar != null;
    }

    @Override
    protected void onServicesInvalidated() {
        hrsChar = null;
        basChar = null;
        posChar = null;
        stepChar = null;
        tempChar = null;
        healthActChar = null;
        lowEnergyActChar = null;
    }


    @NonNull
    @Override
    protected BleManagerGattCallback getGattCallback() {
        return new BleManagerGattCallback() {
            @Override
            protected boolean isRequiredServiceSupported(@NonNull BluetoothGatt gatt) {
                return Nrf5340Manager.this.isRequiredServiceSupported(gatt);
            }

            @Override
            protected void onServicesInvalidated() {
                Nrf5340Manager.this.onServicesInvalidated();
            }
        };
    }

    public void setHealthTrackingEnabled(boolean isEnabled) {
        if (healthActChar != null) {
            // Création d'un tableau de 1 octet (1 si activé, 0 si désactivé)
            byte[] payload = new byte[] { (byte) (isEnabled ? 1 : 0) };

            // Envoi de la donnée via la file d'attente du BleManager
            writeCharacteristic(healthActChar, payload, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
                    .enqueue();

            Log.d("BLE_WRITE", "Commande Health Tracking envoyée : " + isEnabled);
        } else {
            Log.e("BLE_WRITE", "Caractéristique Health Act introuvable");
        }
    }

    public void setLowEnergyModeEnabled(boolean isEnabled) {
        if (lowEnergyActChar != null) {
            byte[] payload = new byte[] { (byte) (isEnabled ? 1 : 0) };

            writeCharacteristic(lowEnergyActChar, payload, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
                    .enqueue();

            Log.d("BLE_WRITE", "Commande Low Energy envoyée : " + isEnabled);
        } else {
            Log.e("BLE_WRITE", "Caractéristique Low Energy introuvable");
        }
    }


}