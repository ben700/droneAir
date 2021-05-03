#include "DroneWiFiConnect.h"

WiFiConnect::WiFiConnect() {
  //_params = (WiFiConnectParam**)malloc(WiFiConnect_MAX_PARAMS * sizeof(WiFiConnectParam*));
  _apName[0]='\0';
  _apPassword[0]='\0';
}

void WiFiConnect::setAPModeTimeoutMins(int mins) {
  if (mins > 0) {
    _apTimeoutMins = mins;
  }
}

void  WiFiConnect::setDeviceId(const char *deviceId){
    this->_deviceId = deviceId;
}

void WiFiConnect::setAPName(const char *apName) {
  if(strlen(apName)>32){return;}
  if (strlen(apName)==0||(apName == NULL)) {
    String ssid = String(this->_deviceId);
    //_apName = ssid.c_str();
    strcpy(_apName,ssid.c_str());

  } else if (apName != NULL &&  strlen(apName)>0) {
   // _apName = apName;
    strcpy(_apName,apName);
  }
}

const char* WiFiConnect::getAPName() {
  if ((_apName == NULL ) || strlen(_apName)==0) {
    setAPName(NULL);
  }
  return _apName;
}



boolean WiFiConnect::startConfigurationPortal() {
  return startConfigurationPortal(AP_NONE, _apName, _apPassword);
}
boolean WiFiConnect::startConfigurationPortal(AP_Continue apcontinue) {
  return startConfigurationPortal(apcontinue, _apName, _apPassword);
}
boolean WiFiConnect::startConfigurationPortal(AP_Continue apcontinue, const char  *apName, const char  *apPassword, bool paramsMode) {


  DEBUG_WC(F("WiFi AP STA - Configuration Portal"));

  _lastAPPage = millis();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  delay(50);

  if (WiFi.status() != WL_CONNECTED) {
  
      DEBUG_WC("startConfigurationPortal(): WiFi.mode(WIFI_AP)");
      WiFi.mode(WIFI_AP);

  } else {
    WiFi.mode(WIFI_AP_STA); // start an access point on the same channel we're already connected to.
  }
  dnsServer.reset(new DNSServer());
server.reset(new ESP8266WebServer(80));

  setAPName(apName);
  DEBUG_WC(_apName);

  if (strlen(apPassword)>0){
    if (strlen(apPassword) < 8 || strlen(apPassword) > 63) {
      // fail passphrase to short or long!
      DEBUG_WC(F("Invalid AccessPoint password. Ignoring"));
      apPassword = NULL;
    }
    //_apPassword = apPassword;
    strcpy(_apPassword,apPassword);
    DEBUG_WC(_apPassword);
  }

  //optional soft ip config
  if (_ap_static_ip) {
    DEBUG_WC(F("Custom AP IP/GW/Subnet"));
    WiFi.softAPConfig(_ap_static_ip, _ap_static_gw, _ap_static_sn);
  }

  //if (_apPassword != NULL && _apPassword!="") {
  if(strlen(apPassword)>0){
    WiFi.softAP(_apName, _apPassword);//password option
  } else {
    WiFi.softAP(_apName);
    DEBUG_WC("startConfigurationPortal(): WiFi.softAP(_apName)");
  }

  delay(500); // Without delay I've seen the IP address blank
  DEBUG_WC(F("AP IP address: "));
  DEBUG_WC(WiFi.softAPIP());
  




  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
  }


    /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
    server->on("/",       std::bind(&WiFiConnect::handleRoot, this));
    server->on("/wifi",     std::bind(&WiFiConnect::handleWifi, this, true));   // Auto Scan of APs
    server->on("/0wifi",    std::bind(&WiFiConnect::handleWifi, this, false)); // Manual entry form only
    server->on("/wifisave",   std::bind(&WiFiConnect::handleWifiSave, this));
    server->on("/i",      std::bind(&WiFiConnect::handleInfo, this)); // Not of interest - commented out in static HTML
    server->on("/r",      std::bind(&WiFiConnect::handleReset, this));
    server->serveStatic("/favicon.ico", LittleFS , "/data/favicon.ico");
    server->serveStatic("/style.css", LittleFS , "/data/style.css"); 
    server->serveStatic("/store_logo.png", LittleFS , "/data/store_logo.png"); 
    server->serveStatic("/bgGloss.png", LittleFS , "/data/bgGloss.png"); 
    server->serveStatic("/bgGlass.png", LittleFS , "/data/bgGlass.png"); 
    server->serveStatic("/qr.png", LittleFS , "/data/qr.png"); 
    
    

    server->onNotFound     (std::bind(&WiFiConnect::handleRoot, this));
    server->begin(); // Web server start
    DEBUG_WC(F("HTTP server started"));
    LittleFS.end();
  //notify we entered AP mode
  if ( _apcallback != NULL) {
    _apcallback(this);
  }
  
  if (apcontinue == AP_WAIT)
    DEBUG_WC(F("AP to stay alive indefinitely waiting for configuration."));
    

  _lastAPPage = millis();
  _readyToConnect = false;    
  while (true) {
  
  if (apcontinue != AP_WAIT)  // Stay here if the AP_LOOP enabled 
    if (millis() - (unsigned long int)_lastAPPage >= ( (unsigned int)_apTimeoutMins * 60 * 1000)) {
      break;
    }
    
    dnsServer->processNextRequest();
    server->handleClient();
    if (_readyToConnect) {
      _readyToConnect = false;

      DEBUG_WC(F("startConfigurationPortal(): Calling: autoConnect(_ssid.c_str(), _password.c_str(), WIFI_AP_STA)"));       

      if (autoConnect(_ssid.c_str(), _password.c_str(), WIFI_AP_STA)) {
      
        DEBUG_WC(F("Connection was a success, so changing to WIFI_STA mode only."));    
        WiFi.mode(WIFI_STA);
        delay(500);
        //WiFi.reconnect();
        //delay(500);
        if (_savecallback != NULL) {
          _savecallback();
        }
        break; // we connected!
      }
      else
      {
        DEBUG_WC(F("Connection was a failure. We keep waiting."));      
      }
    } // ready to connect
  
    yield();
  }
  
  //teardown??
  DEBUG_WC(F("Exiting AP (or AP Params) Mode"));
  server->close();
  server.reset();

  dnsServer.reset();
  boolean con = (WiFi.status() == WL_CONNECTED);
  if (!con) {
    switch (apcontinue) {
      case AP_WAIT:
        break;
      case AP_NONE:
        DEBUG_WC(F("No AP continue action"));
        break;
      case AP_LOOP:
        DEBUG_WC(F("AP to turn off and stay in loop lockup not being accessible."));
        WiFi.mode(WIFI_OFF);
        while (true) {
          delay(1000);
          yield();
        }
  
        break;
      case AP_RESTART:
       case AP_RESET:
        DEBUG_WC(F("AP restart chip"));
        delay(1000);
        ESP.reset();

        delay(2000);
    }
    WiFi.mode(WIFI_STA);
  }
  return con;
}
boolean WiFiConnect::autoConnect() {
  return autoConnect(NULL, NULL, WIFI_STA);
}
boolean WiFiConnect::autoConnect(char const *ssidName, char const *ssidPassword, WiFiMode_t acWiFiMode) {
  WiFi.hostname(this->_deviceId);

  DEBUG_WC(F("Auto Connect"));
  WiFi.mode(acWiFiMode);
  DEBUG_WC("autoConnect():WiFi.mode(acWiFiMode)");
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_WC(F("Already Connected"));
    DEBUG_WC(WiFi.localIP());
    return true;
  }
  if (_sta_static_ip) {
    DEBUG_WC(F("Custom STA IP/GW/Subnet"));
    WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn);
    DEBUG_WC(WiFi.localIP());
  }
  

  int c = 0;
  while (c < _retryAttempts) {
    long ms = millis();
    if (ssidName == NULL || strlen(ssidName)==0) {
    
       if (WiFi.SSID() == "")
        {
          DEBUG_WC(F("No ESP8266 WiFi SSID configuration stored. Bailing."));
          return false;
        }    
        DEBUG_WC(F("Stored SSID:"));
        DEBUG_WC(WiFi.SSID()); 
        DEBUG_WC(F("Stored Password:"));
        DEBUG_WC("[" +WiFi.psk()+"]"); 
     
      WiFi.begin(); // persistence is on by default, so if this WiFi connection should happen automatically
      DEBUG_WC("autoConnect(): WiFi.begin()");


    } else {
      DEBUG_WC(F("Connecting with SSID & Password"));
      DEBUG_WC(ssidName);
      DEBUG_WC(ssidPassword);
      WiFi.begin(ssidName, ssidPassword);
      DEBUG_WC("autoConnect(): WiFi.begin(ssidName, ssidPassword)");
    }


    while (millis() - (unsigned long int)ms < ( (unsigned int)_connectionTimeoutSecs * 1000)) {
      int ws = WiFi.status();
      if (ws == WL_CONNECTED) {
        DEBUG_WC(F("WIFI_CONNECTED"));
        delay(500);//wait for ip to refresh
        DEBUG_WC(WiFi.localIP());
        return true;
      } else if (ws == WL_CONNECT_FAILED) {
        DEBUG_WC(F("WIFI_CONNECT_FAILED"));
        delay(500);
        //return false;
      } else {
        delay(200);
        yield();
      }
    }
    DEBUG_WC(statusToString(WiFi.status()));
    c++;
  } // attemps
  DEBUG_WC(statusToString(WiFi.status()));
  return false;
}
void WiFiConnect::setRetryAttempts(int attempts) {
  if (attempts >= 1) {
    _retryAttempts = attempts;
  }
}
void WiFiConnect::setConnectionTimeoutSecs(int timeout) {
  if (timeout >= 1) {
    _connectionTimeoutSecs = timeout;
  }
}
void WiFiConnect::resetSettings() {
  DEBUG_WC(F("Clearing Settings for:"));
  DEBUG_WC(WiFi.SSID());
  WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
  delay(2000);
}
const char* WiFiConnect::statusToString(int state) {
  switch (state) {
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_NO_SHIELD:
      return "WL_NO_SHIELD";
    default:
      return "UNKNOWN CODE";
  }
}
/**************************************************************************/
/*!
    @brief  Outputs debug messages
    @param  text
            The text to be outputted
*/
/**************************************************************************/
template <typename Generic>
void WiFiConnect::DEBUG_WC(Generic text) {
  if (_debug) {
    Serial.print("DEBUG_WC: ");
    Serial.println(text);
  }
}


/**************************************************************************/
/*!
    @brief  Handles the web request for displaying the menu in the
            Access Point portal.
*/
/**************************************************************************/
void WiFiConnect::handleRoot() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the page.
    return;
  }
  _lastAPPage = millis();
  
  
  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }



  DEBUG_WC("Sending Root");
  String page = FPSTR(AP_HTTP_HEAD);
  page += FPSTR(AP_HTTP_SCRIPT);
  page += FPSTR(AP_HTTP_HEAD_END);
  page.replace("{v}", "WiFi Connection Options");
  page += FPSTR(AP_HTTP_PORTAL_OPTIONS);
  page += FPSTR(AP_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);


}

/**************************************************************************/
/*!
    @brief  Handles the web request for entering the SSID and Password of
            the network you want to connect too.
            Optionally scans and shows available networks.
    @param  scan
            Whether to scan for networks or not.
*/
/**************************************************************************/
void WiFiConnect::handleWifi(boolean scan) {

  _lastAPPage = millis();
  DEBUG_WC(F("Sending WiFi"));

  String page = FPSTR(AP_HTTP_HEAD);
  page += FPSTR(AP_HTTP_SCRIPT);
  page += FPSTR(AP_HTTP_HEAD_END);
  
  if (scan) {
    page.replace("{v}", "Select WiFi");
    page += FPSTR(AP_HTTP_HEAD_TABLE);
    int n = WiFi.scanNetworks();
    DEBUG_WC(F("Scan done"));
    if (n == 0) {
      DEBUG_WC(F("No networks found"));
      page += F("No networks found. Refresh to scan again.");
    } else {

      //sort networks
      int indices[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

      // RSSI SORT

      // old sort
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }

      // remove duplicates ( must be RSSI sorted )
      if (_removeDuplicateAPs) {
        String cssid;
        for (int i = 0; i < n; i++) {
          if (indices[i] == -1) continue;
          cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) {
            if (cssid == WiFi.SSID(indices[j])) {
              DEBUG_WC("DUP AP: " + WiFi.SSID(indices[j]));
              indices[j] = -1; // set dup aps to index -1
            }
          }
        }
      }

      //display networks in page
      for (int i = 0; i < n; i++) {
        if (indices[i] == -1) continue; // skip dups
        DEBUG_WC(WiFi.SSID(indices[i]));
        DEBUG_WC(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

        if (_minimumQuality == -1 || _minimumQuality < quality) {
          String item = FPSTR(AP_HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {

            item.replace("{i}", "l");
            item.replace("{s}", "lock_icon");
            
          } else {
            item.replace("{i}", "");
            item.replace("{s}", "unlock_icon");
            
          }
          page += item;
          delay(0);
        } else {
          DEBUG_WC(F("Skipping due to quality"));
        }

      }
    }
  }
  else
  {
    page.replace("{v}", "Manually enter WiFi details");
    page += FPSTR(MAN_HTTP_HEAD_TABLE);
    
    
  }

  page += FPSTR(AP_HTTP_FORM_START);
  page += FPSTR(AP_HTTP_FORM_END);
  page += FPSTR(AP_HTTP_SCAN_LINK);
  page += FPSTR(AP_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}
void WiFiConnect::handleWifiSave() {
  _lastAPPage = millis();
  DEBUG_WC(F("Sending WiFi Save"));
  //SAVE/connect here
  if (_debug) {
    Serial.println(F("*WC Form Args"));
    for (int x = 0; x < server->args(); x++) {
      Serial.print(F("*WC "));
      Serial.print(server->argName(x));
      Serial.print(F("="));
      Serial.println(server->arg(x));
    }
  }
  _ssid = server->arg("s").c_str();
  _ssid.trim();
  DEBUG_WC(_ssid);
  _password = server->arg("p").c_str();
  _password.trim();
  DEBUG_WC(_password);
 
  if (server->arg("ip") != "") {
    DEBUG_WC(F("static ip"));
    DEBUG_WC(server->arg("ip"));
    //_sta_static_ip.fromString(server->arg("ip"));
    String ip = server->arg("ip");
    optionalIPFromString(&_sta_static_ip, ip.c_str());
  }
  if (server->arg("gw") != "") {
    DEBUG_WC(F("static gateway"));
    DEBUG_WC(server->arg("gw"));
    String gw = server->arg("gw");
    optionalIPFromString(&_sta_static_gw, gw.c_str());
  }
  if (server->arg("sn") != "") {
    DEBUG_WC(F("static netmask"));
    DEBUG_WC(server->arg("sn"));
    String sn = server->arg("sn");
    optionalIPFromString(&_sta_static_sn, sn.c_str());
  }

  String page = FPSTR(AP_HTTP_SAVED_FULL);
  page.replace("{v}", "Credentials Saved");
  page.replace("{ap}", _ssid);
  page.replace("{mac}", WiFi.macAddress());
    
 
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
  _readyToConnect = true;
}
/**************************************************************************/
/*!
    @brief  Handles the web request and shows basic information about the chip.
            TODO: ESP32 information still needs completing.
*/
/**************************************************************************/
void WiFiConnect::handleInfo() {
  _lastAPPage = millis();  DEBUG_WC(F("Sending Info"));

  String page = FPSTR(AP_HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(AP_HTTP_SCRIPT);
 // page += FPSTR(AP_HTTP_STYLE);
  page += FPSTR(AP_HTTP_HEAD_END);
  page += F("<dl>");
  page += F("<dt>Chip ID</dt><dd>");
  page += String(this->_deviceId);
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipId();
#else
  // TODO
  page += F("TODO");
#endif
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  page += F("<dt>Real Flash Size</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipRealSize();
#else
  // TODO
  page += F("TODO");
#endif
  page += F(" bytes</dd>");
  page += F("<dt>Soft AP IP</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  page += FPSTR(AP_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WC(F("Sent info page"));
}
/**************************************************************************/
/*!
    @brief  Handles the reset web request and and restarts the chip.
            Does not clear settings.
*/
/**************************************************************************/
void WiFiConnect::handleReset() {
  _lastAPPage = millis();
  DEBUG_WC(F("Sending Reset"));

  String page = FPSTR(AP_HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(AP_HTTP_SCRIPT);
  page += FPSTR(AP_HTTP_HEAD_END);
  page += F("Module will reset in a few seconds.");
  page += FPSTR(AP_HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WC(F("Sent reset page"));
  delay(5000);
#if defined(ESP8266)
  ESP.reset(); // causes boot loop 
#else
  ESP.restart();
#endif
  delay(2000);

}
/**************************************************************************/
/*!
    @brief  Handles the 204 web request from Android devices
*/
/**************************************************************************/
void WiFiConnect::handle204() {
  _lastAPPage = millis();
  DEBUG_WC(F("Sending 204"));
  handleRoot();
}
/**************************************************************************/
/*!
    @brief  Handles any web requests that are not found.
*/
/**************************************************************************/
void WiFiConnect::handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    DEBUG_WC(F("Webserver called handleNotFound();"));  
    return;
  }
  _lastAPPage = millis();
  DEBUG_WC(F("Sending Not Found"));
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Content-Length", String(message.length()));
  server->send ( 404, "text/plain", message );
}

/**************************************************************************/
/*!
    @brief  Redirect to captive portal if we get a request for another domain.
            Return true in that case so the page handler do not try to
            handle the request again.
    @return True if we have redirected to our portal, else false and contimue
            to handle request
*/
/**************************************************************************/
boolean WiFiConnect::captivePortal() {
  DEBUG_WC(server->hostHeader());
  
  if (!isIp(server->hostHeader()) ) {
    _lastAPPage = millis();
    DEBUG_WC(F("Request redirected to captive portal"));
    String msg = "redirect\n";
    server->sendHeader("Location", String("http://") + toStringIp(server->client().localIP()), true);
    server->sendHeader("Content-Length", String(msg.length()));
    server->send ( 302, "text/plain", msg); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    //server->client().stop(); // Stop is needed because we sent no content length

    return true;
  }
  return false;
}

/**************************************************************************/
/*!
    @brief  Sets the function to be called when we enter Access Point
            for configuration.
            Gets passed a the WiFiConnect for additional usage within function.
    @param  func
            The function to be called.

*/
/**************************************************************************/
void WiFiConnect::setAPCallback( void (*func)(WiFiConnect* myWiFiConnect) ) {
  _apcallback = func;
}
/**************************************************************************/
/*!
    @brief  Sets the function to be called when we need to save the
            custom parameters/configuration.
    @param  func
            The function to be called.
            This function should be short running and ideally just set.
            a flag for processing else where.
*/
/**************************************************************************/
void WiFiConnect::setSaveConfigCallback( void (*func)(void) ) {
  _savecallback = func;
}
/**************************************************************************/
/*!
    @brief  Function to convert RSSI into a usable range 0-100 so we can
            use it to exclude poor networks.
    @param  RSSI
            The quality/signal strength of the WiFi network.
    @return Value in the range of 0-100 to network indicate quality.

*/
/**************************************************************************/
int WiFiConnect::getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}
/**************************************************************************/
/*!
    @brief  Function to check if a given string is an IP Address.
    @param  str
            The string to process.
    @return True if inputted string is an IP Address.

*/
/**************************************************************************/
boolean WiFiConnect::isIp(String str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}
/**************************************************************************/
/*!
    @brief  Function to return a IPAddress as a string representation.
    @param  ip
            The IPAddress to process.
    @return String version of the IPAddress

*/
/**************************************************************************/
String WiFiConnect::toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
/**************************************************************************/
/*!
    @brief  Used to turn on or off messages to the serial port/monitor.
    @param  isDebug
            Set to true to enable messages.
*/
/**************************************************************************/
void WiFiConnect::setDebug(boolean isDebug) {
  _debug = isDebug;
}

/**************************************************************************/
/*!
    @brief  Used to set a static ip/gateway and subnet mask for the access point.
            IP and DNS Gateway are normally the same for a captive portal.
    @param  ip
            The IP address that you want to access the portal through.
    @param  gw
            The gateway address for the DNS server, usually the same as IP for
            captive portal.
    @param  sn
            The subnet mask for the access point, usually 255.255.255.0
*/
/**************************************************************************/
void          WiFiConnect::setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) {
  _ap_static_ip = ip;
  _ap_static_gw = gw;
  _ap_static_sn = sn;
}
/**************************************************************************/
/*!
    @brief  Used to set a static ip/gateway and subnet mask for the local
            network.
    @param  ip
            The IP address that you want to use on the network.
    @param  gw
            The gateway address for the DNS server or network router.
    @param  sn
            The subnet mask for the access point, usually 255.255.255.0
            for local networks.
*/
/**************************************************************************/
void          WiFiConnect::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) {
  _sta_static_ip = ip;
  _sta_static_gw = gw;
  _sta_static_sn = sn;
}
