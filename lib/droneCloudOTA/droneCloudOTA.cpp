#include "droneCloudOTA.h"

DroneCloudOTA::DroneCloudOTA()
{
  //  this->droneDebug = new DroneDebug("droneCloudOTA");
    setURL();
}

String DroneCloudOTA::setURL(bool force)
{
    String _version = VERSION;
    if (force)
    {
        _version = "0";
    }
    this->url = CLOUD_FUNCTION_URL;
    this->url += String("?version=") + _version;
    this->url += String("&variant=") + DEVICE;
    return this->url;
}

bool DroneCloudOTA::reinstall()
{
    return update(true);
}

bool DroneCloudOTA::update(bool force)
{
    HTTPClient http;
    WiFiClient wiFiClient;

    setURL(force);
    Serial.println("Test URL = " + this->url);

    wiFiClient.flush();
    wiFiClient.stop();

    http.begin(wiFiClient, url);
    int httpReturnCode = http.GET();
    String payload = http.getString();

    
    this->downloadUrl = payload;
    http.end();

    
    if (httpReturnCode == HTTP_CODE_NO_CONTENT){
        Serial.println("[204] File Up to date, No new version.");
        return true;
    }else if (httpReturnCode == HTTP_CODE_OK){
        Serial.println("[200] Up to date, New version.");
        Serial.println(payload);

       
    }else if(httpReturnCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
        Serial.println("[500] Getting download URL.");
        return false;
    }else 
    {
        Serial.println("[Error:"+String(httpReturnCode)+"] Getting download URL. ");
        return false;
    }

    wiFiClient.flush();
    wiFiClient.stop();

    wiFiClient.connect(this->downloadUrl, 80);
    http.begin(wiFiClient, this->downloadUrl);

    // start connection and send HTTP header
    int httpCode = http.GET();

    // file found at server
    if (httpCode != HTTP_CODE_OK)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.println("[Download] GET... httpCode: " + httpCode);
        return false;
    }

    int contentLength = http.getSize();
    Serial.println("contentLength : " + String(contentLength));

    if (contentLength > 0)
    {
        bool canBegin = Update.begin(contentLength);
        if (canBegin)
        {
            WiFiClient stream = http.getStream();
            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            size_t written = Update.writeStream(stream);

            if ((int)written == (int)contentLength)
            {
                Serial.println("Written : " + String(written) + " successfully");
            }
            else
            {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
            }

            if (Update.end())
            {

                Serial.println("OTA done!");
                if (Update.isFinished())
                {
                    //     logStateChange(String("Rebooting : OTA successfully replaced ") + String(CURRENT_VERSION));
                    Serial.println("Update successfully completed.");
                    if (!force)
                    {
                        Serial.println("Rebooting.");
                        ESP.restart();
                    }
                    return true;
                }
            }
            else
            {
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        }
        else
        {
            Serial.println("Not enough space to begin OTA");
            wiFiClient.flush();
        }
    }
    else
    {
        Serial.println("There was no content in the response");
        wiFiClient.flush();
    }
    return false;
}
