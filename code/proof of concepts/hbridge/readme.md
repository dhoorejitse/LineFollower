# H-Bridge - PoC
Deze PoC vererifieert de werking van volgende zaken:
- 2 motoren kunnen onafhankelijk van elkaar kunnen draaien
- motoren zijn (traploos) regelbaar in snelheid en draairichting

Bij een correcte werking zal eerst motor 1 traploos versnellen van stilstand tot zijn maximum snelheid, en vervolgens terug vertragen in de andere richting tot stilstand. Motor 2 herhaald deze werking.

https://github.com/user-attachments/assets/9430dffd-f219-440d-9be8-ca7bf7144678

## Schema
<img src="../../../media/images/hbridgepocschema.png" alt="Schema" width="800"/>

## Stappenplan
- Sluit de componenten correct aan volgens bovenstaande schema
- Verbind de microcontroller met de pc
- Verify/Upload de code naar de mictrocontroller
- Verifieer de werking door te vergelijken met bovenstaande video
