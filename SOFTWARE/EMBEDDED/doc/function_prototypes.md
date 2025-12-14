# 📋 API Logicielle du Projet Gring-Go (Zephyr RTOS) - Convention Concise

## 1. ⚙️ Couche Drivers Hôtes (`drivers/`)

| Fichier | Nom de la Fonction | Entrées | Sorties | Utilité |
| :--- | :--- | :--- | :--- | :--- |
| **`haptic_driver.c`** | `HAPTIC_Init()` | `void` | `int` | Initialise et configure le DRV2605L. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| | `HAPTIC_PlayEffect()` | `enum alert_effect effect_id` | `int` | Déclenche un effet de vibration. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| **`imu_driver.c`** | `IMU_Init()` | `void` | `int` | Charge le Firmware ARC (Flash). Retourne `STATUS_OK` ou `STATUS_ERR_FW_FLASH`. |
| | `IMU_Read()` | `struct imu_data *data` | `int` | Lit le *buffer* du BHI260AP. Retourne `STATUS_OK`, `STATUS_ERR_I2C_COM` ou `STATUS_ERR_NOT_READY`. |
| | `IMU_SleepWake()` | `bool enable` | `int` | Gère l'état de sommeil/réveil. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| **`bio_driver.c`** | `BIO_Init()` | `void` | `int` | Charge le Firmware WHRM (Flash). Retourne `STATUS_OK` ou `STATUS_ERR_FW_FLASH`. |
| | `BIO_Read()` | `struct bio_data *data` | `int` | Lit le *buffer* du MAX32664. Retourne `STATUS_OK`, `STATUS_ERR_I2C_COM` ou `STATUS_ERR_NOT_READY`. |
| | `BIO_SleepMode()` | `bool enable` | `int` | Gère l'état *shutdown*/réveil. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| | `BIO_InjectMotion()` | `struct imu_data *motion_data` | `int` | Transmet les données de mouvement pour correction. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| **`temp_driver.c`** | `TEMP_Init()` | `void` | `int` | Initialise la puce STTS22H. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| | `TEMP_ReadCelsius()` | `float *temperature` | `int` | Effectue une lecture I²C. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| **`charge_driver.c`** | `CHARGE_Init()` | `void` | `int` | Initialise le pilote de charge I²C. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |
| | `CHARGE_GetStatus()` | `struct charge_status *status` | `int` | Récupère l'état de charge. Retourne `STATUS_OK` ou `STATUS_ERR_I2C_COM`. |

## 2. 🎛️ Couche Logique Application & Gestion (`src/`)

| Fichier | Nom de la Fonction | Entrées | Sorties | Utilité |
| :--- | :--- | :--- | :--- | :--- |
| **`sensors_wrapper.c`** | `WRAPPER_StartPoll()` | `void` | `int` | Démarre le thread de *polling*. Retourne `STATUS_OK` ou code Zephyr (`-ENOMEM`). |
| | `WRAPPER_StopPoll()` | `void` | `int` | Arrête le thread de *polling*. Retourne `STATUS_OK`. |
| | `WRAPPER_GetLatestData()` | `void` | `struct aggregated_data` | Fournit les dernières données lues (pas, BPM, temp). |
| **`power_monitor.c`** | `PWR_Init()` | `void` | `int` | Démarre un thread pour la surveillance de la batterie. Retourne `STATUS_OK` ou code Zephyr (`-ENOMEM`). |
| | `PWR_GetLevel()` | `void` | `uint8_t` (Pourcentage) | Retourne le pourcentage de batterie actuel. |
| | `PWR_CheckChargeStatus()` | `void` | `void` | Vérifie l'état de charge et met à jour l'état système. |
| **`main.c`** | `main()` | `void` | `int` | Initialise tout. Retourne `0` si succès, ou code d'erreur Zephyr si échec critique. |

## 3. 🌐 Couche Services GATT (`src/`)

| Fichier | Nom de la Fonction | Entrées | Sorties | Utilité |
| :--- | :--- | :--- | :--- | :--- |
| **`alert_service.c`** | `ALERT_SVC_Init()` | `void` | `int` | Enregistre le service GATT. Retourne `0` ou code d'erreur BLE Zephyr. |
| | `ALERT_SVC_WriteCb()` | *Paramètres Zephyr* | `ssize_t` | **Callback GATT Write.** Réagit à la commande de vibration. Retourne la taille des données écrites (ou code d'erreur). |
| **`health_service.c`** | `HEALTH_SVC_Init()` | `void` | `int` | Enregistre le service GATT de santé. Retourne `0` ou code d'erreur BLE Zephyr. |
| | `HEALTH_SVC_NotifyData()` | `struct aggregated_data *data` | `void` | Envoie une notification (NOTIFY). |
| **`battery_service.c`** | `BAS_Init()` | `void` | `int` | Enregistre le service GATT BAS. Retourne `0` ou code d'erreur BLE Zephyr. |
| | `BAS_NotifyLevel()` | `uint8_t level` | `void` | Envoie une notification (NOTIFY) du niveau de batterie actuel. |

# 📋 Définitions des Types et Status Clés

## 1. Structures de Données (Définies dans `include/gringgo_types.h`)

| Type | Contenu | Utilisé par | Utilité |
| :--- | :--- | :--- | :--- |
| `struct imu_data` | `uint32_t steps_count`, `enum activity_mode mode` | `IMU_Read`, `BIO_InjectMotion` | Représente les données de mouvement traitées par le BHI260AP. |
| `struct bio_data` | `uint16_t bpm`, `uint8_t spo2_pct`, `uint16_t hrv` | `BIO_Read` | Représente les métriques biométriques traitées par le MAX32664. |
| `struct charge_status` | `uint8_t battery_level`, `bool is_charging`, `uint16_t voltage_mv` | `CHARGE_GetStatus` | État de la batterie et du récepteur de charge sans fil. |
| `struct aggregated_data` | `struct bio_data bio`, `uint32_t steps`, `float temp_celsius`, `uint8_t batt_level` | `WRAPPER_GetLatestData`, `HEALTH_SVC_NotifyData` | Ensemble complet des données de santé du dernier cycle de *polling*. |

## 2. Énumérations (Définies dans `include/gringgo_types.h`)

| Type | Valeurs Clés | Utilisé par | Utilité |
| :--- | :--- | :--- | :--- |
| `enum alert_effect` | `ALERT_VIB_INCOMING_CALL`, `ALERT_VIB_NOTIFICATION`, `ALERT_VIB_ALARM` | `HAPTIC_PlayEffect`, `ALERT_SVC_WriteCb` | Identifiants pour sélectionner un effet de vibration ROM prédéfini sur le DRV2605L. |
| `enum activity_mode` | `MODE_STILL`, `MODE_WALKING`, `MODE_RUNNING`, `MODE_SLEEPING` | `IMU_Read` | État d'activité détecté par l'algorithme de fusion de capteurs BHI260AP. |

## 3. Codes de Statut Personnalisés (Définis dans `include/gringgo_status.h`)

| Code (Valeur) | Nom de la Macro | Utilisé par | Utilité |
| :--- | :--- | :--- | :--- |
| `0` | `STATUS_OK` | Toutes | Succès de l'opération (Standard Zephyr/C). |
| `-1001` | `STATUS_ERR_I2C_COM` | Drivers | Erreur générique de communication I²C/SPI. |
| `-1002` | `STATUS_ERR_FW_FLASH` | `IMU_Init`, `BIO_Init` | Échec du chargement du firmware dans le Sensor Hub. |
| `-1003` | `STATUS_ERR_NOT_READY` | `IMU_Read`, `BIO_Read` | Le capteur n'a pas encore de donnée valide à retourner. |
| `-1004` | `STATUS_ERR_NO_CONN` | Services GATT | Tentative d'envoyer une notification sans connexion BLE active. |