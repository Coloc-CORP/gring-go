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

    // On ne garde que ce qu'on utilise réellement
    private BluetoothGattCharacteristic hrsChar, basChar, posChar, stepChar;

    // Interfaces pour la communication avec le ViewModel
    public interface OnGattDataListener {
        void onHeartRateReceived(int bpm);
        void onBatteryReceived(int percent);
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

    @Override
    protected boolean isRequiredServiceSupported(@NonNull BluetoothGatt gatt) {
        final BluetoothGattService hrs = gatt.getService(BluetoothUUIDs.HEART_RATE_SERVICE_UUID);
        final BluetoothGattService bas = gatt.getService(BluetoothUUIDs.BATTERY_SERVICE_UUID);
        final BluetoothGattService pos = gatt.getService(BluetoothUUIDs.PULSE_OXIMETER_SERVICE_UUID);
        final BluetoothGattService rsc = gatt.getService(BluetoothUUIDs.RUNNING_SPEED_AND_CADENCE_SERVICE_UUID);

        if (hrs != null) hrsChar = hrs.getCharacteristic(BluetoothUUIDs.HEART_RATE_MEASUREMENT_CHARACTERISTIC_UUID);
        if (bas != null) basChar = bas.getCharacteristic(BluetoothUUIDs.BATTERY_LEVEL_CHARACTERISTIC_UUID);
        if (pos != null) posChar = pos.getCharacteristic(BluetoothUUIDs.PULSE_OXIMETER_CHARACTERISTIC_UUID);
        if (rsc != null) stepChar = rsc.getCharacteristic(BluetoothUUIDs.STEP_COUNT_CHARACTERISTIC_UUID);

        Log.d("BLE_DEBUG", "HRS Service: " + (hrs != null));
        Log.d("BLE_DEBUG", "HRS Char: " + (hrsChar != null));
        Log.d("BLE_DEBUG", "BAS Service: " + (bas != null));

        //return true;
        return hrsChar != null || basChar != null; // On est prêt si on a au moins un des deux
    }

    @Override
    protected void onServicesInvalidated() {
        // Très important : on nettoie les références quand on déconnecte
        hrsChar = null;
        basChar = null;
        posChar = null;
        stepChar = null;
    }

    private void setupGattStreams() {
        if (hrsChar != null) {
            setNotificationCallback(hrsChar).with((dev, data) -> {
                int bpm = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 1);
                if (dataListener != null) dataListener.onHeartRateReceived(bpm);
            });
            enableNotifications(hrsChar).enqueue();
        }

        if (basChar != null) {
            setNotificationCallback(basChar).with((dev, data) -> {
                int bat = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT8, 0);
                if (dataListener != null) dataListener.onBatteryReceived(bat);
            });
            enableNotifications(basChar).enqueue();
        }

        // Flux PAS (Nouveau)
        if (stepChar != null) {
            setNotificationCallback(stepChar).with((dev, data) -> {
                // Pour les pas, on utilise souvent UINT16 ou UINT32 (selon le firmware)
                int steps = data.getIntValue(no.nordicsemi.android.ble.data.Data.FORMAT_UINT16, 1);
                if (dataListener != null) dataListener.onStepsReceived(steps);
            });
            enableNotifications(stepChar).enqueue();
        }


    }

    // Cette méthode est obligatoire dans les versions récentes de BleManager
    @NonNull
    @Override
    protected BleManagerGattCallback getGattCallback() {
        return new BleManagerGattCallback() {
            @Override
            protected boolean isRequiredServiceSupported(@NonNull BluetoothGatt gatt) {
                // On appelle la méthode qu'on a déjà écrite au dessus
                return Nrf5340Manager.this.isRequiredServiceSupported(gatt);
            }

            @Override
            protected void onServicesInvalidated() {
                Nrf5340Manager.this.onServicesInvalidated();
            }
        };
    }
}