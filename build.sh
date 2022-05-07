sudo ln -s ~/.platformio/penv/bin/platformio /usr/local/bin/platformio
sudo ln -s ~/.platformio/penv/bin/pio /usr/local/bin/pio
sudo ln -s ~/.platformio/penv/bin/piodebuggdb /usr/local/bin/piodebuggdb


platformio run --target erase --environment droneAir
platformio run --target clean

platformio run --target buildfs --environment droneAir
platformio run --environment droneAir

platformio run --target uploadfs --environment droneAir
platformio run --target upload --environment droneAir