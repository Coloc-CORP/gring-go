package com.example.gringgo;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioGroup;

public class Parametre extends Fragment {

    private RadioGroup radioGroupTheme;
    private SharedPreferences sharedPreferences;

    public Parametre() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_parametre, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        radioGroupTheme = view.findViewById(R.id.radioGroupTheme);
        sharedPreferences = requireActivity().getSharedPreferences("GringoPrefs", Context.MODE_PRIVATE);

        // 1. Récupérer le thème sauvegardé (par défaut : géré par le système)
        int savedTheme = sharedPreferences.getInt("app_theme", AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM);

        // 2. Cocher le bon bouton radio à l'ouverture de la page
        if (savedTheme == AppCompatDelegate.MODE_NIGHT_NO) {
            radioGroupTheme.check(R.id.radio_theme_light);
        } else if (savedTheme == AppCompatDelegate.MODE_NIGHT_YES) {
            radioGroupTheme.check(R.id.radio_theme_dark);
        } else {
            radioGroupTheme.check(R.id.radio_theme_system);
        }

        // 3. Écouter les changements de sélection
        radioGroupTheme.setOnCheckedChangeListener((group, checkedId) -> {
            int mode;
            if (checkedId == R.id.radio_theme_light) {
                mode = AppCompatDelegate.MODE_NIGHT_NO;
            } else if (checkedId == R.id.radio_theme_dark) {
                mode = AppCompatDelegate.MODE_NIGHT_YES;
            } else {
                mode = AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM;
            }

            // Sauvegarder le choix pour la prochaine ouverture de l'application
            sharedPreferences.edit().putInt("app_theme", mode).apply();

            // Appliquer le thème instantanément (va recharger l'activité visuellement)
            AppCompatDelegate.setDefaultNightMode(mode);
        });
    }
}