package com.example.gringgo.ViewModel;

import android.app.Application;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.util.Log;
import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.example.gringgo.bdd.AppDatabase;
import com.example.gringgo.bdd.HealthDao;
import com.example.gringgo.bdd.HealthData;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

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
    public static final String KEY_HEALTH_STATE = "health_state";
    public static final String KEY_LOW_ENERGY_STATE = "low_energy_state";

    // Variables pour la base de données
    private final HealthDao healthDao;
    private final ExecutorService databaseWriteExecutor = Executors.newSingleThreadExecutor();


    public BluetoothViewModel(@NonNull Application application) {
        super(application);
        manager = new Nrf5340Manager(application);
        manager.setOnGattDataListener(this);
        manager.setOnConnectionStatusChangedListener(connectionState::postValue);

        // Initialisation de la base de données
        AppDatabase db = AppDatabase.getDatabase(application);
        healthDao = db.healthDao();
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
        clearHealthData();
    }

    public Nrf5340Manager getBleManager() {
        return manager;
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
     * @param key La clé (ex: KEY_HEALTH_STATE)
     */
    public boolean getSavedState(Context context, String key) {
        return context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
                .getBoolean(key, false);
    }

    /**
     * Sauvegarde l'état d'un switch.
     * @param isChecked État à sauvegarder
     * @param context Nécessaire pour accéder aux SharedPreferences
     * @param key La clé (ex: KEY_HEALTH_STATE)
     */
    public void setTrackingState(boolean isChecked, Context context, String key) {
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
                .edit()
                .putBoolean(key, isChecked)
                .apply();
    }

    public void clearHealthData() {
        heartRate.postValue("--");
        temperature.postValue("--");
        spO2.postValue("--");
        stepsCount.postValue("--");
        // On peut laisser stepsCount à sa dernière valeur ou le remettre à "0" selon votre préférence
    }

    public void saveCurrentDataToDatabase() {
        HealthData snapshot = new HealthData();
        snapshot.timestamp = System.currentTimeMillis();

        // On récupère les valeurs actuelles (si null, on met "--")
        snapshot.heartRate = heartRate.getValue() != null ? heartRate.getValue() : "--";
        snapshot.steps = stepsCount.getValue() != null ? stepsCount.getValue() : "--";
        snapshot.temperature = temperature.getValue() != null ? temperature.getValue() : "--";
        snapshot.spO2 = spO2.getValue() != null ? spO2.getValue() : "--";
        snapshot.battery = batteryLevel.getValue() != null ? batteryLevel.getValue() : "--";

        // L'insertion DOIT se faire en arrière-plan pour ne pas bloquer l'application
        databaseWriteExecutor.execute(() -> {
            healthDao.insert(snapshot);
            Log.d("DATABASE", "Données sauvegardées avec succès !");
        });
    }

    public LiveData<List<HealthData>> getAllHistory() {
        return healthDao.getAllHistory();
    }
}