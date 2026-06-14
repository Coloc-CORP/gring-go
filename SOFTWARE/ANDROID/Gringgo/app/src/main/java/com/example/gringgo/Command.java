package com.example.gringgo;

import android.content.Context;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModelProvider;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Switch;
import android.util.Log;

import com.example.gringgo.ViewModel.BluetoothViewModel;
import com.example.gringgo.ViewModel.Nrf5340Manager;

public class Command extends Fragment {

    private Switch switchHealth;
    private Switch switchLowEnergy;
    private BluetoothViewModel bluetoothViewModel;

    private final MutableLiveData<Boolean> healthTrackingState = new MutableLiveData<>(false);

    public Command() {
        // Constructeur public vide requis
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_command, container, false);
    }

    public void setHealthTracking(boolean enabled, Context context) {
        // 1. Sauvegarde persistante
        context.getSharedPreferences("GringoPrefs", Context.MODE_PRIVATE)
                .edit().putBoolean("health_state", enabled).apply();

        // 2. Mise à jour du LiveData
        healthTrackingState.setValue(enabled);
    }

    public boolean getSavedHealthState(Context context) {
        return context.getSharedPreferences("GringoPrefs", Context.MODE_PRIVATE)
                .getBoolean("health_state", false);
    }

    public LiveData<Boolean> getHealthTrackingState() {
        return healthTrackingState;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        switchHealth = view.findViewById(R.id.switch1);
        switchLowEnergy = view.findViewById(R.id.switch2);


        switchHealth.setChecked(bluetoothViewModel.getSavedState(requireContext(), true));
        // 1. Récupération du ViewModel partagé avec l'Activité
        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);

        // 2. Action lors de l'appui sur l'interrupteur "Santé"
        switchHealth.setOnCheckedChangeListener((buttonView, isChecked) -> {
            Nrf5340Manager manager = bluetoothViewModel.getBleManager();
            Log.d("COMMAND_FRAG", "Manager: " + (manager != null ? "OK" : "NULL"));
            Log.d("COMMAND_FRAG", "Connected: " + (manager != null && manager.isConnected()));

            if (manager != null && manager.isConnected()) {
                manager.setHealthTrackingEnabled(isChecked);
                bluetoothViewModel.setTrackingState(isChecked, requireContext(), true);
            } else {
                Log.e("COMMAND_FRAG", "BLE non connecté ou manager null");
                buttonView.setChecked(!isChecked);
            }
        });
        switchLowEnergy.setChecked(bluetoothViewModel.getSavedState(requireContext(), false));
        // 3. Action lors de l'appui sur l'interrupteur "Low Energy"
        switchLowEnergy.setOnCheckedChangeListener((buttonView, isChecked) -> {
            Nrf5340Manager manager = bluetoothViewModel.getBleManager();
            Log.d("COMMAND_FRAG", "Manager: " + (manager != null ? "OK" : "NULL"));
            Log.d("COMMAND_FRAG", "Connected: " + (manager != null && manager.isConnected()));

            if (manager != null && manager.isConnected()) {
                manager.setHealthTrackingEnabled(isChecked);
                bluetoothViewModel.setTrackingState(isChecked, requireContext(), false);
            } else {
                Log.e("COMMAND_FRAG", "BLE non connecté ou manager null");
                buttonView.setChecked(!isChecked);
            }
        });
    }
}