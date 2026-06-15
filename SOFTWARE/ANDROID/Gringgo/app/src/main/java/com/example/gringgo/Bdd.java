package com.example.gringgo;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import com.example.gringgo.bdd.HealthData; // Assurez-vous que cet import correspond à votre package
import com.example.gringgo.ViewModel.BluetoothViewModel;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

public class Bdd extends Fragment {

    private BluetoothViewModel bluetoothViewModel;
    private ArrayAdapter<String> listAdapter;
    private ArrayList<String> historyList;

    public Bdd() {
        // Constructeur public vide requis
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_bdd, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);

        // --- 1. CONFIGURATION DU BOUTON SAUVEGARDER ---
        Button btnSauvegarder = view.findViewById(R.id.btn_sauvegarder);
        btnSauvegarder.setOnClickListener(v -> {
            String currentState = bluetoothViewModel.getConnectionState().getValue();
            if ("CONNECTED".equals(currentState)) {
                bluetoothViewModel.saveCurrentDataToDatabase();
                Toast.makeText(requireContext(), "Données enregistrées !", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(requireContext(), "Erreur : Connectez-vous d'abord à la carte.", Toast.LENGTH_SHORT).show();
            }
        });

        // --- 2. CONFIGURATION DE LA LISTE (AFFICHAGE) ---
        ListView listView = view.findViewById(R.id.listView);
        historyList = new ArrayList<>();

        // On utilise un affichage simple fourni par Android
        listAdapter = new ArrayAdapter<>(requireContext(), android.R.layout.simple_list_item_1, historyList);
        listView.setAdapter(listAdapter);

        // Formateur pour transformer le "timestamp" (chiffres) en date lisible
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy à HH:mm:ss", Locale.getDefault());

        // --- 3. OBSERVATION DE LA BASE DE DONNÉES ---
        // Dès qu'une donnée est ajoutée, Room prévient cette méthode qui met à jour la liste
        bluetoothViewModel.getAllHistory().observe(getViewLifecycleOwner(), healthDataList -> {

            // On vide l'ancienne liste affichée
            historyList.clear();

            // On boucle sur toutes les lignes de la base de données
            for (HealthData data : healthDataList) {

                // On formate la date
                String dateStr = dateFormat.format(new Date(data.timestamp));

                // On crée une jolie chaîne de caractères pour chaque ligne
                String affichage = dateStr + "\n" +
                        "BPM: " + data.heartRate +
                        " | Pas: " + data.steps + "\n" +
                        " | Temp: " + data.temperature +
                        " | SpO2: " + data.spO2 +
                        " | Bat: " + data.battery;

                // On ajoute ce texte à notre liste
                historyList.add(affichage);
            }

            // On prévient la ListView que les données ont changé pour qu'elle se redessine
            listAdapter.notifyDataSetChanged();
        });
    }
}