package com.example.gringgo.bdd;

import androidx.room.Entity;
import androidx.room.PrimaryKey;

@Entity(tableName = "health_data_table")
public class HealthData {

    @PrimaryKey(autoGenerate = true)
    public int id; // Identifiant unique généré automatiquement

    public long timestamp; // L'heure exacte de la sauvegarde
    public String heartRate;
    public String steps;
    public String temperature;
    public String spO2;
    public String battery;
}