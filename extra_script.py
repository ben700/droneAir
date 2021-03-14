Import("env")

# access to global construction environment
#print env

build_tag = env['PIOENV']
env.Replace(PROGNAME="droneair_firmware")

# Dump construction environments (for debug purpose)
#print env.Dump()
