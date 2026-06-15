package com.example.gringgo.bdd;

import androidx.lifecycle.LiveData;
import androidx.room.Dao;
import androidx.room.Insert;
import androidx.room.Query;

import java.util.List;

@Dao
public interface HealthDao {

    // Insérer une nouvelle ligne
    @Insert
    void insert(HealthData healthData);

    // Récupérer tout l'historique, du plus récent au plus ancien
    @Query("SELECT * FROM health_data_table ORDER BY timestamp DESC")
    LiveData<List<HealthData>> getAllHistory();

    // Supprimer tout l'historique (utile pour un bouton "Vider la base")
    @Query("DELETE FROM health_data_table")
    void deleteAll();
}