package com.example.gringgo;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.example.gringgo.ViewModel.BluetoothViewModel;
import com.example.gringgo.ViewModel.Nrf5340Manager;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.view.View;

import androidx.appcompat.app.AppCompatDelegate;
import androidx.appcompat.widget.PopupMenu;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.example.gringgo.databinding.ActivityMainBinding;

import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private AppBarConfiguration appBarConfiguration;
    private ActivityMainBinding binding;

    private BluetoothViewModel bluetoothViewModel;

    // À ajouter dans ton MainActivity.java si tu ne l'as pas déjà fait

    // La méthode qui permet aux Fragments d'y accéder :
    public Nrf5340Manager getBleManager() {
        return bluetoothViewModel.getBleManager();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        SharedPreferences sharedPreferences = getSharedPreferences("GringoPrefs", Context.MODE_PRIVATE);
        int savedTheme = sharedPreferences.getInt("app_theme", AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM);
        AppCompatDelegate.setDefaultNightMode(savedTheme);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.toolbar);

        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        appBarConfiguration = new AppBarConfiguration.Builder(navController.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);

        binding.fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAnchorView(R.id.fab)
                        .setAction("Action", null).show();
            }
        });

        //FOR THE POP UP MENU

        // 1. Récupérer la référence du FAB défini dans activity_main.xml
        FloatingActionButton fab = findViewById(R.id.fab);

        // 2. Ajouter l'écouteur de clic
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showPopupMenu(view);
            }
        });

        setSupportActionBar(binding.toolbar);

        // --- AJOUT : INITIALISATION DU VIEWMODEL ---
        bluetoothViewModel = new ViewModelProvider(this).get(BluetoothViewModel.class);

        // --- AJOUT : OBSERVATION DE LA BATTERIE ---
        // On récupère le TextView que nous avons ajouté dans le XML de la toolbar précédemment
        TextView tvBattery = findViewById(R.id.tv_toolbar_battery);

        if (tvBattery != null) {
            bluetoothViewModel.getBatteryLevel().observe(this, battery -> {
                tvBattery.setText(battery); // "battery" contient déjà le "%"
            });
        }

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        NavController navController = Navigation.findNavController(MainActivity.this, R.id.nav_host_fragment_content_main);

        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            navController.navigate(R.id.parametre);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }

    // Méthode pour afficher le PopupMenu
    private void showPopupMenu(View view) {
        // Création du PopupMenu attaché au bouton (view)
        PopupMenu popup = new PopupMenu(this, view);

        // On charge le fichier XML créé à l'étape 1
        popup.getMenuInflater().inflate(R.menu.menu_navigation, popup.getMenu());

        // Gestion des clics sur les items du menu
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {

                NavController navController = Navigation.findNavController(MainActivity.this, R.id.nav_host_fragment_content_main);
                int id = item.getItemId();

                // On vérifie quel item a été cliqué
                if (id == R.id.nav_menu) {
                    // Naviguer vers Fragment 1
                    navController.navigate(R.id.menu);
                    return true;
                }
                else if (id == R.id.nav_command) {
                    // Naviguer vers Fragment 2
                    navController.navigate(R.id.command);
                    return true;
                }
                else if (id == R.id.nav_distance) {
                    // Naviguer vers Fragment 3
                    navController.navigate(R.id.distance);
                    return true;
                }
                else if (id == R.id.nav_bdd) {
                    // Naviguer vers Fragment 4
                    navController.navigate(R.id.bdd);
                    return true;
                }
                return false;
            }
        });

        // Afficher le menu
        popup.show();
    }
}


