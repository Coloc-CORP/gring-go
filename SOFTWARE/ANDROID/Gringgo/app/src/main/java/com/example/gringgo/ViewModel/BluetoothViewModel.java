package com.example.gringgo.ViewModel;

import android.app.Application;
import android.bluetooth.BluetoothDevice;
import android.util.Log;
import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import java.util.ArrayList;
import java.util.List;

public class BluetoothViewModel extends AndroidViewModel implements Nrf5340Manager.OnGattDataListener {

    private final Nrf5340Manager manager;
    private final MutableLiveData<List<BluetoothDevice>> devices = new MutableLiveData<>(new ArrayList<>());
    private final MutableLiveData<BluetoothDevice> selectedDevice = new MutableLiveData<>();
    private final MutableLiveData<String> connectionState = new MutableLiveData<>("DISCONNECTED");

    // Données des capteurs
    private final MutableLiveData<String> heartRate = new MutableLiveData<>("--");
    private final MutableLiveData<String> batteryLevel = new MutableLiveData<>("--");
    private final MutableLiveData<String> stepsCount = new MutableLiveData<>("0"); // Nouveau

    public BluetoothViewModel(@NonNull Application application) {
        super(application);
        manager = new Nrf5340Manager(application);

        // On branche les listeners
        manager.setOnGattDataListener(this); // Cast supprimé car inutile
        manager.setOnConnectionStatusChangedListener(connectionState::postValue);
    }

    // --- Implémentation GATT (Callbacks directs du Manager) ---

    @Override
    public void onHeartRateReceived(int bpm) {
        heartRate.postValue(String.valueOf(bpm));
    }

    @Override
    public void onBatteryReceived(int percent) {
        // On peut ajouter le % directement ici pour l'affichage
        batteryLevel.postValue(percent + " %");
    }

    @Override
    public void onPulseOxReceived(int spo2) {
        Log.d("BLE_DATA", "SPO2 reçu : " + spo2);
    }

    @Override
    public void onStepsReceived(int steps) {
        stepsCount.postValue(String.valueOf(steps));
    }

    // --- Gestion de la liste d'appareils ---

    public void addDevice(BluetoothDevice device) {
        List<BluetoothDevice> current = devices.getValue();
        if (current != null && !current.contains(device)) {
            current.add(device);
            devices.setValue(current);
        }
    }

    // --- Actions ---

    public void connect(BluetoothDevice device) {
        selectedDevice.setValue(device);
        manager.connect(device)
                .retry(3, 100)
                .useAutoConnect(false)
                .enqueue();
    }

    public void disconnect() {
        if (manager != null) {
            manager.disconnect().enqueue();
            connectionState.postValue("DISCONNECTED");
        }
    }

    // --- Getters pour le Fragment (Menu) ---
    // Note : On expose en LiveData (lecture seule) pour plus de sécurité
    public LiveData<List<BluetoothDevice>> getDevices() { return devices; }
    public LiveData<String> getConnectionState() { return connectionState; }
    public LiveData<String> getHeartRate() { return heartRate; }
    public LiveData<String> getBatteryLevel() { return batteryLevel; }
    public LiveData<String> getStepsCount() { return stepsCount; }
}