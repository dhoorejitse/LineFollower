# Blink - PoC
Deze PoC vererifieert de werking van volgende zaken:
- compilen en uploaden van een programma naar de microcontroller lukt
- de klok frequentie van de microcontroller klopt

Bij een correcte werking knippert de RGB-led achtereenvolgens rood, groen en blauw, telkens 1 seconde aan en 1 seconde uit, waarna de cyclus zich herhaalt. Bij een opstart of reset van de microcontroller knippert de led 3 keer kortstondig wit.

https://github.com/user-attachments/assets/0f00047d-495a-4bb4-b6ac-76eb2669470c

## Schema
<img src="../../../media/images/blinkpocschema.png" alt="Schema" width="400"/>

## Stappenplan
- Sluit RGB led samen met weerstanden correct aan op de microcontroller volgens bovenstaande schema
- Verbind de microcontroller met de pc
- Verify/Upload de code naar de mictrocontroller
- Verifieer de werking door te vergelijken met bovenstaande video

