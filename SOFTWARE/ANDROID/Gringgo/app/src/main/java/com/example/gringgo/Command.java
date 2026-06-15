package com.example.gringgo;

import android.os.Bundle;
import android.view.MotionEvent;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Switch;
import android.util.Log;

import com.example.gringgo.ViewModel.BluetoothViewModel;
import com.example.gringgo.ViewModel.Nrf5340Manager;

public class Command extends Fragment {

    private Switch switchHealth;
    private Switch switchLowEnergy;
    private Button buttonVibration; // Changé ici
    private BluetoothViewModel bluetoothViewModel;

    public Command() {
        // Constructeur vide
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_command, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        switchHealth = view.findViewById(R.id.switch1);
        switchLowEnergy = view.findViewById(R.id.switch2);
        buttonVibration = view.findViewById(R.id.button_vibration); // Changé ici

        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);

        // 1. Initialisation visuelle basée sur la sauvegarde (uniquement pour les switchs)
        switchHealth.setChecked(bluetoothViewModel.getSavedState(requireContext(), BluetoothViewModel.KEY_HEALTH_STATE));
        switchLowEnergy.setChecked(bluetoothViewModel.getSavedState(requireContext(), BluetoothViewModel.KEY_LOW_ENERGY_STATE));

        // --- SYNCHRONISATION AUTOMATIQUE ---
        bluetoothViewModel.getConnectionState().observe(getViewLifecycleOwner(), state -> {
            if ("CONNECTED".equals(state)) {
                switchHealth.setChecked(false);
                bluetoothViewModel.setTrackingState(false, requireContext(), BluetoothViewModel.KEY_HEALTH_STATE);
            }
        });

        bluetoothViewModel.getHeartRate().observe(getViewLifecycleOwner(), bpm -> {
            if (!"--".equals(bpm)) {
                if (!switchHealth.isChecked()) {
                    Log.i("COMMAND_FRAG", "Données reçues : Sync du switch Santé sur ON");
                    switchHealth.setChecked(true);
                    bluetoothViewModel.setTrackingState(true, requireContext(), BluetoothViewModel.KEY_HEALTH_STATE);
                }
            }
        });

        // 2. Action Santé
        switchHealth.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (!buttonView.isPressed()) return;

            Nrf5340Manager manager = bluetoothViewModel.getBleManager();
            if (manager != null && manager.isConnected()) {
                manager.setHealthTrackingEnabled(isChecked);
                bluetoothViewModel.setTrackingState(isChecked, requireContext(), BluetoothViewModel.KEY_HEALTH_STATE);

                if (!isChecked) {
                    bluetoothViewModel.clearHealthData();
                }
            } else {
                buttonView.setChecked(!isChecked);
            }
        });

        // 3. Action Low Energy
        switchLowEnergy.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (!buttonView.isPressed()) return;

            Nrf5340Manager manager = bluetoothViewModel.getBleManager();
            if (manager != null && manager.isConnected()) {
                manager.setLowEnergyModeEnabled(isChecked);
                bluetoothViewModel.setTrackingState(isChecked, requireContext(), BluetoothViewModel.KEY_LOW_ENERGY_STATE);
            } else {
                buttonView.setChecked(!isChecked);
            }
        });

        // 4. NOUVEAU : Action Bouton Poussoir Vibration
        buttonVibration.setOnTouchListener((v, event) -> {
            Nrf5340Manager manager = bluetoothViewModel.getBleManager();

            if (manager != null && manager.isConnected()) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        // Le doigt appuie : on envoie 1
                        manager.setVibrationEnabled(true);
                        v.setPressed(true); // Effet visuel d'enfoncement
                        return true;

                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        // Le doigt relâche (ou glisse hors du bouton) : on envoie 0
                        manager.setVibrationEnabled(false);
                        v.setPressed(false); // Relâchement visuel
                        v.performClick(); // Bonne pratique Android
                        return true;
                }
            } else {
                // Si non connecté, on prévient juste quand on appuie
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    Log.e("COMMAND_FRAG", "BLE non connecté : impossible de vibrer");
                }
            }
            return false;
        });
    }
}