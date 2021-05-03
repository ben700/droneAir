namePrefix="droneAir"
gcloudProject = "drone-302200"
droneRegion="europe-west1"
droneRegistry="droneDeviceProduction"
homeDir = "/home/benslittlebitsandbobs"
 
import subprocess
import os
import sys
import html
import urllib.request
import filecmp

sys.path.append(homeDir + "/esptool/")  # after this line, esptool becomes importable

from esptool import ESP8266ROM, ESPLoader
   
chipId = ESP8266ROM.chip_id(ESPLoader.detect_chip())
deviceName = namePrefix +"_"+ str(chipId)
devicePrivateKey = deviceName + "_private.pem"
devicePrivateKeyDer = deviceName + "_private.der"
devicePrivateKeyCsr = deviceName + "_private.csr"
devicePublicKey = deviceName + "_public.pem"

privateKeyFilePath = "device_LitttleFS/data/private-key.der"
publicKeyFilePath = "device_LitttleFS/data/public-key.pem"
qRFilePath = "device_LitttleFS/data/qr.png"

def areKeyFilesSame():
    checkPublic = filecmp.cmp(publicKeyFilePath, homeDir + "/.ssh/droneKeys/" + devicePublicKey)
    checkPrivate = filecmp.cmp(privateKeyFilePath, homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer)
    if(checkPublic and  checkPrivate):
        return True
    else:
        return False
    
def setProject():
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud config set project " + gcloudProject
    os.system(cmd)

def removeDevice():
    print("Remove device from Google IOT")
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices delete " + deviceName +" --region="+droneRegion+"  --registry="+droneRegistry+" -q"
    os.system(cmd)
    print("Remove keys")
    cmd = "rm " + homeDir + "/.ssh/droneKeys/" + deviceName + "*"
    os.system(cmd)
    cmd = "rm " + privateKeyFilePath 
    os.system(cmd)
    cmd = "rm " + publicKeyFilePath
    os.system(cmd)
    cmd = "rm " + qRFilePath
    os.system(cmd)

def testIfDeviceExists():
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices credentials describe --region="+droneRegion+"  --registry="+droneRegistry + " --device " + deviceName +" 0"
    p = os.popen(cmd)
    deviceExistTest = p.read()
    if(deviceExistTest):
        print("device Exist")
        if(areKeyFilesSame()):
            print("Key Files are the same")
            return False
        else:
            print("Key Files don't match so remove device and recreate")            
            removeDevice()
            return True
#        yes = {'yes','y', 'ye', ''}
#        no = {'no','n'}
#        print("Device exist, do you want to delete and recreate? Otherwise will use existing [y,n]")
#        choice = input().lower()
#        if choice in yes:
#            removeDevice()
#            return True
#        elif choice in no:
#            return False
    else:
        return True

def createDevice():
    cmd = "openssl ecparam -genkey -name prime256v1 -noout -out " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey
    os.system(cmd)
    cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -pubout -out " + homeDir + "/.ssh/droneKeys/" + devicePublicKey 
    os.system(cmd)
    cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -outform der -out " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer
    os.system(cmd)
    cmd = "openssl req -new -sha256 -key " + homeDir + "/.ssh/droneKeys/" + devicePrivateKey + " -out  " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyCsr + " -subj '/CN=unused'"
    os.system(cmd)
    cmd = "cp " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer + " " + privateKeyFilePath
    os.system(cmd)
    cmd = "cp " + homeDir + "/.ssh/droneKeys/" + devicePublicKey + " " + publicKeyFilePath
    os.system(cmd)
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud config set project " + gcloudProject
    os.system(cmd)
    cmd =  homeDir + "/google-cloud-sdk/bin/gcloud iot devices create " + deviceName +" --region="+droneRegion+" --registry="+droneRegistry+" --public-key path=" + publicKeyFilePath + ",type=es256"
    os.system(cmd)
    count = len(open(publicKeyFilePath).readlines(  ))
    f = open(publicKeyFilePath, "r")
    head = f.readline()
    key =""
    i=2
    while i<count:
        key += f.readline().strip().replace("+","%2B")
        i = i +1
    url = "https://chart.googleapis.com/chart?cht=qr&chs=400x400&chl={\"serial_number\":\"" + deviceName
    url += "\",\"type\":\"" + namePrefix
    url += "\",\"public_key\":\"" + key + "\"}"
    urllib.request.urlretrieve(url,  qRFilePath )

setProject()
if(testIfDeviceExists()):
    createDevice()
print(deviceName)