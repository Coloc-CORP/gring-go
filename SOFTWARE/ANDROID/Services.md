*Voici un documents mentionnant les différents services utilisés dans le cadre de la communication bluetooth. Pour l'instant, le choix a été fait de ne pas prendre de profil bluetooth mais directement les services GATT appropriés.*

| <center>Fonction</center>           | <center>GATT Services</center>    | <center>UUID</center> | <center>UUID Characteristics</center> | <center></center>Détails                           |
| ----------------------------------- | --------------------------------- | --------------------- | ------------------------------------- | -------------------------------------------------- |
| ==BPM==                             | ==Heart Rate Service==            | ==0x180D==            | ==0x2A37==                            |                                                    |
| ==Temperature==                     | ==Health Thermometer Service==    | ==0x1809==            | ==0x2B03==                            | ==Personnalisé==                                   |
| ==SP0²==                            | ==Pulse Oximeter Service==        | ==0x1822==            | ==0x2B04==                            | ==PLX Continuous Measurement<br><br>Personnalisé== |
| Step                                | Running Speed and Cadence Service | 0x1814                | 0x1068                                |                                                    |
| ==Health activation==                   | ==Device Information Service==        | ==0x180A==                | ==0x2b01==                                | ==Personnalisé==                                       |
| ==Low energy activation==               | ==Device Information Service==        | ==0x180A==                | ==0x2b02==                                | ==Personnalisé==                                       |
| ==Info system - authentification==      | ==Device Information Service==        | ==0x180A==                | ==0x2A29==                                | ==Manufacturer Name String==                           |
| Notification send for haptic return | Alert Notification Service        | 0x1811                | 0x2A46                                | 0x2A45 for the level                               |
| Battery                             | Battery Service                   | 0x180F                | 0x2A19                                |                                                    |




