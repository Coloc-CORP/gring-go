package com.example.gringgo;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.example.gringgo.ViewModel.BluetoothViewModel;

public class Distance extends Fragment {

    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    private String mParam1;
    private String mParam2;

    // Déclaration du ViewModel
    private BluetoothViewModel bluetoothViewModel;

    public Distance() {
        // Required empty public constructor
    }

    public static Distance newInstance(String param1, String param2) {
        Distance fragment = new Distance();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_distance, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        TextView tvDistance = view.findViewById(R.id.chiffre_distance);
        TextView tvStep = view.findViewById(R.id.chiffre_pas);

        // 1. CORRECTION DU CRASH : Initialisation indispensable du ViewModel
        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);

        // 2. CORRECTION LOGIQUE : On observe les pas, pas le rythme cardiaque
        bluetoothViewModel.getStepsCount().observe(getViewLifecycleOwner(), stepsStr -> {

            // Mise à jour de l'affichage des pas
            if (tvStep != null) {
                tvStep.setText(stepsStr);
            }

            // Calcul et mise à jour de la distance
            if (tvDistance != null) {
                if (!"--".equals(stepsStr) && !stepsStr.isEmpty()) {
                    try {
                        int steps = Integer.parseInt(stepsStr);
                        // On estime un pas moyen à 0.75 mètre
                        double distanceMetres = steps * 0.75;

                        // Affiche la distance avec 1 chiffre après la virgule (ex: "450.5")
                        tvDistance.setText(String.format(java.util.Locale.US, "%.1f", distanceMetres));
                    } catch (NumberFormatException e) {
                        tvDistance.setText("--");
                    }
                } else {
                    tvDistance.setText("--");
                }
            }
        });
    }
}