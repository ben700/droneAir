
Import("env")
build_tag = env['PIOENV']
env.Replace(PROGNAME="droneAir_firmware")

dataDir = env['PROJECT_DIR'] + "/device_LitttleFS/"
env.Replace(PROJECTDATA_DIR= dataDir)

