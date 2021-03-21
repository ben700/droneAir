Import("env")

# access to global construction environment
#print env

build_tag = env['PIOENV']
env.Replace(PROGNAME="droneAir_firmware")

# Dump construction environments (for debug purpose)
#print env.Dump()
