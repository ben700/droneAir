platformio run --target erase --environment esp8266
platformio run --target clean

platformio run --target buildfs --environment esp8266
platformio run --environment esp8266

platformio run --target uploadfs --environment esp8266
platformio run --target upload --environment esp8266