# 💍 Projet Bague Connectée BLE - Santé & Bien-être

## 🎯 Objectif

Créer une **bague connectée** ultra-compacte, élégante et rechargeable sans fil, capable de :
- Suivre la fréquence cardiaque
- Mesurer la SpO2 (oxygène dans le sang)
- Évaluer le stress
- Compter les pas et suivre l’activité physique
- Suivre les phases de sommeil
- Mesurer la température corporelle
- Se connecter à Samsung Health via Bluetooth Low Energy (BLE)

---

## 🧩 Architecture générale

| Composant                | Fonction                                                             |
|--------------------------|----------------------------------------------------------------------|
| **nRF5340 (Nordic)**     | Microcontrôleur principal avec BLE                                  |
| **MAX86150 / MAX30102**  | Capteur PPG (fréquence cardiaque, SpO2, stress)                     |
| **BHI260AP / LSM6DSOX**  | IMU pour mouvement, podomètre, détection du sommeil                 |
| **STTS22H**              | Capteur de température corporelle                                   |
| **Batterie LiPo 10-20mAh** | Alimentation, rechargeable sans fil                                |
| **STWLC38**              | Récepteur de recharge sans fil (Qi) avec protection intégrée        |
| **PCB flexible (FPC)**   | Support du circuit miniature                                        |
| **Boîtier titane**       | Enveloppe solide, légère et hypoallergénique                       |

---

## 🔋 Alimentation & Recharge

- **Recharge sans fil Qi** via un chargeur personnalisé
- Basé sur **STWLC38**, compatible Qi 1.2, avec protection intégrée
- Antenne bobinée sur FPC autour de la bague
- Batterie : **LiPo 3.7V** entre 10 et 20 mAh, charge lente pour préserver la durée de vie

---

## 📱 Fonctionnalités principales

| Fonction                      | Capteurs nécessaires                     | Détail technique                                   |
|-------------------------------|------------------------------------------|---------------------------------------------------|
| Fréquence cardiaque           | MAX86150 / MAX30102                      | PPG infrarouge, données envoyées en BLE           |
| SpO2                          | MAX86150 / MAX30102                      | Analyse des variations d’intensité lumineuse      |
| Stress / HRV                  | PPG                                      | Analyse de la variabilité cardiaque (HRV)         |
| Suivi des pas (podomètre)     | BHI260AP / LSM6DSOX                      | Accéléromètre et algorithmes de step counting     |
| Suivi de sommeil (phases)     | PPG + IMU                                | Fusion des données pour estimer les cycles        |
| Température corporelle        | STTS22H                                  | Mesure en contact avec la peau (intérieur bague)  |

---

## 📶 Connexion à Samsung Health

- **BLE Standard HRM (Heart Rate Monitor Profile)** reconnu par Samsung Health
- Via **nRF Connect SDK** (Zephyr RTOS) ou Nordic SDK
- Possibilité de développer une **application Android compagnon** pour collecter, afficher et éventuellement **transférer via Health Connect** les données vers Samsung Health

---

## 🛠️ Logiciels & Développement

- **Firmware :**
  - **nRF Connect SDK** avec Zephyr RTOS (Langage C)
  - IDE recommandé : **nRF Connect for VS Code** ou **Segger Embedded Studio**
- **Conception PCB :**
  - Outils : **KiCad**, **Altium Designer**
  - Type : **Flexible PCB (FPC)** ultra-fin
- **Modélisation 3D du boîtier :**
  - Fusion 360 / SolidWorks
  - Impression SLA ou CNC pour prototypage
  - Matériau : **Titane** (durable, hypoallergénique)
