sudo ln -s ~/.platformio/penv/bin/platformio /usr/local/bin/platformio
sudo ln -s ~/.platformio/penv/bin/pio /usr/local/bin/pio
sudo ln -s ~/.platformio/penv/bin/piodebuggdb /usr/local/bin/piodebuggdb


platformio run --target erase --environment esp8266
platformio run --target clean

platformio run --target buildfs --environment esp8266
platformio run --environment esp8266

platformio run --target uploadfs --environment esp8266
platformio run --target upload --environment esp8266