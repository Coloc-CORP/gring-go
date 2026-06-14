package com.example.gringgo.ViewModel;

import android.app.Application;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
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
    private final MutableLiveData<String> stepsCount = new MutableLiveData<>("0");
    private final MutableLiveData<String> temperature = new MutableLiveData<>("--");
    private final MutableLiveData<String> spO2 = new MutableLiveData<>("--");

    private static final String PREFS_NAME = "GringoPrefs";
    private static final String KEY_HEALTH_STATE = "health_state";
    private static final String KEY_LOW_ENERGY_STATE = "low_energy_state";


    public BluetoothViewModel(@NonNull Application application) {
        super(application);
        manager = new Nrf5340Manager(application);
        manager.setOnGattDataListener(this);
        manager.setOnConnectionStatusChangedListener(connectionState::postValue);
    }

    @Override
    public void onHeartRateReceived(int bpm) {
        heartRate.postValue(String.valueOf(bpm));
    }

    @Override
    public void onBatteryReceived(int percent) {
        batteryLevel.postValue(percent + " %");
    }

    @Override
    public void onTemperatureReceived(int tempC) {
        temperature.postValue(tempC + " °C");
    }

    @Override
    public void onPulseOxReceived(int spo2) {
        spO2.postValue(spo2 + " %");
    }

    @Override
    public void onStepsReceived(int steps) {
        stepsCount.postValue(String.valueOf(steps));
    }

    public void addDevice(BluetoothDevice device) {
        List<BluetoothDevice> current = devices.getValue();
        if (current != null && !current.contains(device)) {
            current.add(device);
            devices.setValue(current);
        }
    }

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

    // À l'intérieur de ta classe BluetoothViewModel
    public Nrf5340Manager getBleManager() {
        return manager; // ou le nom que tu as donné à ta variable Nrf5340Manager dans le ViewModel
    }

    // Getters
    public LiveData<List<BluetoothDevice>> getDevices() { return devices; }
    public LiveData<String> getConnectionState() { return connectionState; }
    public LiveData<String> getHeartRate() { return heartRate; }
    public LiveData<String> getBatteryLevel() { return batteryLevel; }
    public LiveData<String> getStepsCount() { return stepsCount; }
    public LiveData<String> getTemperature() { return temperature; }
    public LiveData<String> getSpO2() { return spO2; }


    /**
     * Récupère l'état sauvegardé pour un switch spécifique.
     * @param context Nécessaire pour accéder aux SharedPreferences
     * @param isHealthSwitch Si true, récupère l'état de la santé, sinon du Low Energy
     */
    public boolean getSavedState(Context context, boolean isHealthSwitch) {
        return context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
                .getBoolean(isHealthSwitch ? KEY_HEALTH_STATE : KEY_LOW_ENERGY_STATE, false);
    }

    /**
     * Sauvegarde l'état d'un switch.
     * @param isChecked État à sauvegarder
     * @param context Nécessaire pour accéder aux SharedPreferences
     * @param isHealthSwitch Si true, sauvegarde la santé, sinon le Low Energy
     */
    public void setTrackingState(boolean isChecked, Context context, boolean isHealthSwitch) {
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
                .edit()
                .putBoolean(isHealthSwitch ? KEY_HEALTH_STATE : KEY_LOW_ENERGY_STATE, isChecked)
                .apply(); // .apply() est asynchrone, ce qui est idéal pour ne pas bloquer l'UI
    }
}