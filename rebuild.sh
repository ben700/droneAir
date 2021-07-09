set -e
platformio run --target erase --environment esp8266

platformio run --target uploadfs --environment esp8266
platformio run --target upload --environment esp8266
