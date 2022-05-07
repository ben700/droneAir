set -e
platformio run --target erase --environment droneAir

platformio run --target uploadfs --environment droneAir
platformio run --target upload --environment droneAir
