# Gebruiksaanwijzing

### gebruiken van esp32 in arduino IDE
* esp library intalleren
* boards toevoegen in preferences
* CP2102 drivers installeren

### opladen / vervangen batterijen
Het opladen van de batterijen gebreurd met een speciale batterijoplader die gemaakt is op de 18650 batterijen veillig en correct op te laden. Vervang de batterijen door de lege batterijen uit de houder te halen en vervang deze met opgeladen batterijen

### draadloze communicatie
#### verbinding maken
zorg dat de robot aanstaat (rgb led is rood/groen). Verbind vervolgens uw pc met bluetooth device "ESP32_BT_DJ". Maak vervolgens een seriele verbinding in PuTTy:
* Serial line: Gebruik de 2de bluetooth COM poort in de device manager
* Speed: 115200
* Connection type: Serial

#### commando's
help
debug  
run true
run false
set cycle [µs]  
set power [0..255]  
set diff [0..1]  
set kp [0..]  
set ki [0..]  
set kd [0..]  
calibrate black  
calibrate white  

### kalibratie
plaats de robot op een volledig zwarte ondergrond
* commando: calibrate black

plaats de robot op een volledig witte ondergrond
* commando: calibrate white

controlleer of de waarden binnen het gewenste waarden geven
* commando: debug

### settings
De robot rijdt stabiel met volgende parameters:  
* cycle: 1000
* power: 200
* diff: 0.4
* kp: 18
* ki: 0
* kd: 0.2

### start/stop button
de robot kan gestart worden op twee manieren:
* fysieke button vooraan de robot gelegen
* commando: run true/false

de status van de robot wordt aangeduid met behulp van een rgb led
* rode led: run false
* groene led: run true
