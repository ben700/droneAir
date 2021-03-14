namePrefix="droneAir"
droneRegion="europe-west1"
droneRegistry="droneDevice"
homeDir = "/home/benslittlebitsandbobs/"
 
import subprocess
import os
import sys
sys.path.append(homeDir + "/esptool/")  # after this line, esptool becomes importable

from esptool import ESP8266ROM, ESPLoader
   
chipId = ESP8266ROM.chip_id(ESPLoader.detect_chip())
deviceName = namePrefix + str(chipId)
devicePrivateKey = deviceName + "_private.pem"
devicePrivateKeyDer = deviceName + "_private.der"
devicePrivateKeyCsr = deviceName + "_private.csr"
devicePublicKey = deviceName + "_public.pem"



cmd = "rm " + homeDir + "/.ssh/droneKeys/" + deviceName + "*"
os.system(cmd)
cmd = "openssl ecparam -genkey -name prime256v1 -noout -out " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey
os.system(cmd)
cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -pubout -out " + homeDir + "/.ssh/droneKeys/" + devicePublicKey 
os.system(cmd)
cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -outform der -out " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer
os.system(cmd)
cmd = "openssl req -new -sha256 -key " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -out  " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyCsr + " -subj '/CN=unused'"
os.system(cmd)
cmd = "cp " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer + " data/private-key.der"
os.system(cmd)

cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices delete " + deviceName +" --region="+droneRegion+"  --registry="+droneRegistry+" -q"
print(cmd)
os.system(cmd)

cmd =  homeDir + "/google-cloud-sdk/bin/gcloud iot devices create " + deviceName +" --region="+droneRegion+" --registry="+droneRegistry+" --public-key path=" + homeDir + "/.ssh/droneKeys/" + devicePublicKey + ",type=es256"
print(cmd)
os.system(cmd)
