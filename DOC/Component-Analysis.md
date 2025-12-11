# 📊 Component Comparison Tables — Smart Ring Project

---

## 1. Microcontroller (MCU BLE)

| Model            | Dimensions (mm) | Mass | Consumption (Active / Sleep) | Supply Voltage | Flash / RAM | Interfaces | Special Features | Eval Kit Availability | Price (€) | Notes |
|------------------|-----------------|------|------------------------------|----------------|-------------|------------|------------------|-----------------------|-----------|-------|
| **nRF5340** (Nordic) |                 |      |                              |                |             |            |                  |                       |           | Le plus utilisé sur le marché pour les memes usages donc aussi le plus de commu pour de l'aide potentielle dans forums, possède toutes les fonctions nécessaires + multi thread + RTOS |
|                  |                 |      |                              |                |             |            |                  |                       |           |       |
|                  |                 |      |                              |                |             |            |                  |                       |           |       |

---

## 2. Heart Rate Sensor (PPG + Sp0²)

| Model                     | Dimensions (mm)    | Mass | Consumption (Act / Sleep) | Supply Voltage    | Interface | Acc / Perf | Integrated LEDs | Module / Sensor | VIL / VIH        | Price (€) | Notes |
|---------------------------|--------------------|------|---------------------------|-------------------|-----------|------------|-----------------|-----------------|------------------|-----------|-------|
| **MAX32664** (Analog Devices) | 3.87 x 3.87 x 0.64 | 22mg |                           | 1.71 - 3.63V      | I²C / SPI |            | No              | Module          | 0.3 / 0.7 VDD    | 6.12      | Besoin d'un capteur optique avec données brutes en sortie |
| MAX30101 (Analog Devices) | 5.6  x 3.3  x 1.55 | 33mg | **0.6mA / 0.7µA**             | 1.7 - 2V (5V LED) | I²C       | 18-bits    | Yes             | Sensor          | 0.3 / 0.7 VDD    | 10.23     | **PIM438**| 
| NJL5313R (Nisshinbo)      | 3.15 x 6.00 x 0.65 | **17mg** | 15mA                      | 3.5V              | -         |            | Yes             | Sensor          | -                | 5.18      | Consomme trop et mauvaise disponibilité      |

---

## 3. IMU (Accelerometer + Gyroscope)

| Model            | Dimensions (mm)    | Mass | Consumption (Active / Sleep) | Supply Voltage | Interface | Axes | Accuracy | VIL / VIH        | Price (€) | Notes |
|------------------|--------------------|------|------------------------------|----------------|-----------|------|----------|------------------|-----------|-------|
| **BHI260AP (Bosch)** | 3.6 x 4.1 x 0.83   |      | 249µA / 8µA                  | 1.8V           | I²C / SPI | 6    |          | 0.3 / 0.7 VDD    | 6.05      | **MCU intégré qui traite localement les données : beaucoup plus simple à implémenter**, Dev board 13$ |
| LSM6DSOX (ST)    |**2.5 x 3   x 0.83**|      | **170-26µA  / 3µA**          | 1.7 - 3.6V     | I²C / SPI | 6    |          | 0.3 / 0.7 VDD    | **3.62**  | ABX00101 10$, STEVAL 15$      |
|                  |                    |      |                              |                |           |      |          |                  |           |       |

---

## 4. Temperature Sensor

| Model        | Dimensions (mm) | Mass | Consumption  | Supply Voltage | Interface   | Accuracy | Special Features | Price (€) | Notes |
|--------------|-----------------|------|--------------|----------------|-------------|----------|------------------|-----------|-------|
| **STTS22H (ST)** | 2.0 x 2.0 x 0.5 |      | **120 / 1.75 µA**| 1.5 - 3.6      | I²C / SMBus | 0.25°C   |                  | 1.45      | **SEN-21262 6$**|
| TMP117       | **1.5 x 0.95 x 0.5**|      | 135 / 3.5 µA | 1.7 - 5.5      | I²C / SMBus | **0.1°C**    |                  | 1.92      | SEN-15805 17$, ADA4821 14$      |

---

## 5. Battery

| Model | Capacity (mAh) | Mass | Dimensions (mm) | Thickness (mm) | Voltage (Nominal / Range) | Protection Circuit | Certification | Price (€) | Notes |
|-------|----------------|------|-----------------|----------------|---------------------------|--------------------|---------------|-----------|-------|
|       |                |      |                 |                |                           |                    |               |           |       |
|       |                |      |                 |                |                           |                    |               |           |       |

---

## 6. Wireless Charging IC

| Model | Dimensions (mm) | Mass | Supply Voltage | Output Voltage | Efficiency | Qi Standard Support | Features | Price (€) | Notes |
|-------|-----------------|------|----------------|----------------|------------|---------------------|----------|-----------|-------|
|       |                 |      |                |                |            |                     |          |           |       |
|       |                 |      |                |                |            |                     |          |           |       |

---

## 7. Wireless Charging Coil

| Model | Diameter (mm) | Mass | Thickness (mm) | Inductance (µH) | Resistance (Ω) | Efficiency | Compatible ICs | Price (€) | Notes |
|-------|---------------|------|----------------|-----------------|----------------|------------|----------------|-----------|-------|
|       |               |      |                |                 |                |            |                |           |       |
|       |               |      |                |                 |                |            |                |           |       |

---

## 8. Regulator / LDO

| Model | Dimensions (mm) | Output Voltage | Quiescent Current | Dropout Voltage | Max Current | Noise Level | Price (€) | Notes |
|-------|-----------------|----------------|-------------------|-----------------|-------------|-------------|-----------|-------|
|       |                 |                |                   |                 |             |             |           |       |
|       |                 |                |                   |                 |             |             |           |       |

---

## 9. BLE Antenna

| Model | Type (Ceramic / PCB) | Dimensions (mm) | Mass | Gain (dBi) | Frequency | Matching Required | Price (€) | Notes |
|-------|----------------------|-----------------|------|------------|-----------|-------------------|-----------|-------|
|       |                      |                 |      |            |           |                   |           |       |
|       |                      |                 |      |            |           |                   |           |       |

## 10. Actionneur (Comparaison)

| Actionneur | Mode de Fonctionnement | Avantages pour une Bague | Inconvénients / Impact Physique | Choix |
| :--- | :--- | :--- | :--- | :--- |
| **Moteur Haptique (LRA/ERM)** | Vibration discrète | **Retour personnel et silencieux.** Faible consommation cumulée (pics courts). | Nécessite un volume minimal (épaisseur) et un driver (DRV2605L). | **CHOISI** |
| **Haut-Parleur / Buzzer** | Son / Alerte audible | Peut attirer l'attention de l'utilisateur sans contact. | **Nécessite une chambre acoustique.** Retour non-discret (entendue par l'entourage). Haute consommation continue. | REJETÉ |
| **Microphone** | Acquisition audio | Permet la commande vocale ou l'enregistrement (si requis). | **Non un actionneur.** Nécessite un ADC / DSP pour le traitement et une forte consommation. | REJETÉ |

---

## 11. Composants Haptiques 

| Model | Dimensions (mm) | Mass | Consumption (Active / Sleep) | Supply Voltage | Interface | Special Features | Eval Kit Availability | Price (€) | Notes |
|---|---|---|---|---|---|---|---|---|---|
| **Pilote DRV2605L (TI)** | 1.5 x 1.5 x 0.6 | | **0.5 mA / 4.1 µA** | 1.8 - 3.3V | I²C | Bibliothèque de 123 effets ROM intégrée. | DRV2605LEVM (Abordable) | 2.15 | Utilisation du mode Bibliothèque pour décharger le nRF5340. |
| **Moteur LRA** (Ex: HD-EMC0503-LW27) | $5 \text{ x } 1.8$ (épaisseur) | < 1g | **85 mA** (pic) | 2.7 - 3.3V | PWM | Le seul LRA compatible avec l'épaisseur d'une bague. | N/A | 1.50 | Courant de pointe important (85 mA) mais très courte durée (Duty Cycle faible). |

---

## Final choices

| **Component** | **Dimensions (mm)** | **Consumption (Active / Sleep)** | **Supply Voltage** | **Interface** | **Accuracy / Performance** | **Special Features** | **Price (€)** | **Notes** |
|---------------|----------------------|----------------------------------|---------------------|---------------|-----------------------------|-----------------------|---------------|-----------|
| MCU           | nRF5340              |                                  |                     |               |                             | Multi-thread, RTOS    |               | Le plus utilisé sur le marché, grande communauté |
| Heart Sensor (PPG) | MAX32664          |                                  | 1.71 - 3.63V        | I²C / SPI     |                            | Besoin d'un capteur optique, données brutes | 6.12          | Module, nécessite capteur optique externe |
| IMU           | BHI260AP             | 249µA / 8µA                    | 1.8V         | I²C / SPI     | 6 axes                     |                      | 6.05          | **MCU intégré qui traite localement les données : beaucoup plus simple à implémenter**, Dev board 13$ |
| Temp Sensor   | STTS22H              | 120 / 1.75 µA                     | 1.5 - 3.6V         | I²C / SMBus   | 0.25°C                      |                      | 1.45          | SEN-21262 6€ |
| **Haptic Driver** | **DRV2605L** | **0.5 mA / 4.1 µA** | **1.8 - 3.3V** | **I²C** | **N/A** | **Bibliothèque ROM intégrée** | **2.15** | Pilote basse consommation pour décharger le MCU. |
| **Haptic Motor** | **LRA** | **85 mA / 0 mA** | **2.7 - 3.3V** | **PWM** | **Discret** | **Ultra-plat (1.8mm)** | **1.50** | Courant de pointe important mais durée d'activation très courte. | Battery       |                      |                                  |                     |               |                             |                       |               |           |
| Wireless Charging IC |                |                                  |                     |               |                             |                       |               |           |
| Charging Coil |                      |                                  |                     |               |                             |                       |               |           |
| Regulator / LDO |                     |                                  |                     |               |                             |                       |               |           |
| BLE Antenna   |                      |                                  |                     |               |                             |                       |               |           |
| Other         |                      |                                  |                     |               |                             |                       |               |           |
