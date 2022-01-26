homeDir = "/home/benslittlebitsandbobs"
import sys
# after this line, esptool becomes importable
sys.path.append(homeDir + "/esptool/")


from esptool import ESP8266ROM, ESPLoader
import filecmp
import urllib.request
import html

import os
import subprocess

namePrefix="droneAir"
gcloudProject = "drone-302200"
droneRegion="europe-west1"
droneRegistry="droneDeviceProduction"



chipId = ESP8266ROM.chip_id(ESPLoader.detect_chip(port="/dev/ttyUSB1"))
deviceNamePostfix = str(chipId)
deviceName = namePrefix + "_" + deviceNamePostfix
devicePrivateKeyPem = deviceNamePostfix + "_private.pem"
devicePrivateKeyDer = deviceNamePostfix + "_private.der"
devicePrivateKeyCsr = deviceNamePostfix + "_private.csr"
devicePublicKeyPem = deviceNamePostfix + "_public.pem"
deviceQRFilePath = deviceNamePostfix + "_qr.png"
deviceFilePath = "device_LitttleFS/data/id/"
privateKeyFilePath = deviceFilePath + devicePrivateKeyDer
publicKeyFilePath = deviceFilePath + devicePublicKeyPem
qRFilePath = deviceFilePath + deviceQRFilePath


def areKeyFilesSame():
    p = os.popen(publicKeyFilePath)
    deviceExistTest = p.read()
    if(deviceExistTest):
        print(publicKeyFilePath + " Exists")
    else:
        print(publicKeyFilePath + " doesn't Exists")
        return False

    checkPublic = filecmp.cmp(
        publicKeyFilePath, homeDir + "/.ssh/droneKeys/" + devicePublicKeyPem)
    checkPrivate = filecmp.cmp(
        privateKeyFilePath, homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer)
    if(checkPublic and checkPrivate):
        return True
    else:
        return False


def setProject():
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud config set project " + gcloudProject
    os.system(cmd)


def removeDevice():
    print("Remove device from Google IOT")
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices delete " + \
        deviceName + " --region="+droneRegion+"  --registry="+droneRegistry+" -q"
    os.system(cmd)
    print("Remove keys")
    cmd = "rm " + homeDir + "/.ssh/droneKeys/*" + deviceNamePostfix + "*"
    os.system(cmd)
    cmd = "rm *" + deviceNamePostfix+"*"
    os.system(cmd)


def testIfDeviceExists():
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices credentials describe --region=" + \
        droneRegion+"  --registry="+droneRegistry + " --device " + deviceName + " 0"
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
    cmd = "openssl ecparam -genkey -name prime256v1 -noout -out " + \
        homeDir + "/.ssh/droneKeys/" + devicePrivateKeyPem
    os.system(cmd)
    cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyPem + \
        " -pubout -out " + homeDir + "/.ssh/droneKeys/" + devicePublicKeyPem
    os.system(cmd)
    cmd = "openssl ec -in " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyPem + \
        " -outform der -out " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyDer
    os.system(cmd)
    cmd = "openssl req -new -sha256 -key " + homeDir + "/.ssh/droneKeys/" + devicePrivateKeyPem + \
        " -out  " + homeDir + "/.ssh/droneKeys/" + \
        devicePrivateKeyCsr + " -subj '/CN=unused'"
    os.system(cmd)
    cmd = "cp " + homeDir + "/.ssh/droneKeys/" + \
        devicePrivateKeyDer + " " + privateKeyFilePath
    os.system(cmd)
    cmd = "cp " + homeDir + "/.ssh/droneKeys/" + \
        devicePublicKeyPem + " " + publicKeyFilePath
    os.system(cmd)
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud config set project " + gcloudProject
    os.system(cmd)
    cmd = homeDir + "/google-cloud-sdk/bin/gcloud iot devices create " + deviceName + " --region=" + \
        droneRegion+" --registry="+droneRegistry + \
        " --public-key path=" + publicKeyFilePath + ",type=es256"
    os.system(cmd)
    count = len(open(publicKeyFilePath).readlines())
    f = open(publicKeyFilePath, "r")
    head = f.readline()
    key = ""
    i = 2
    while i < count:
        key += f.readline().strip().replace("+", "%2B")
        i = i + 1
    url = "https://chart.googleapis.com/chart?cht=qr&chs=400x400&chl={\"serial_number\":\"" + deviceName
    url += "\",\"type\":\"" + namePrefix
    url += "\",\"public_key\":\"" + key + "\"}"
    urllib.request.urlretrieve(url,  qRFilePath)
    cmd = "cp " + qRFilePath + " ~/.ssh/droneKeys/" + deviceQRFilePath
    os.system(cmd)


setProject()
if(testIfDeviceExists()):
    createDevice()