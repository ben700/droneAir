#ifndef WC_DRONE_HTML
#define WC_DRONE_HTML

///< Define to stop re-inclusion
const char HTTP_HEADER[] PROGMEM  =
"<!DOCTYPE html>"
"<html lang='en'>"
    "<head>"
        "<title>Drone Configuration</title>"
        "<link rel='icon' href='/favicon.ico' type='image/x-icon' />"
        "<link rel='shortcut icon' href='/favicon.ico' type='image/x-icon' />"
        "<link rel='stylesheet' href='/style.css' />";
/** Scripts for our page */
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
/** End of the header section and beginning of the body */
const char HTTP_HEADER_END[] PROGMEM        = "</head>"
"    <body>"
"    <div id='bodyWrapper' class='container_24'>"
"      <div id='header' class='grid_24'>"
"        <div id='storeLogo'>"
"            <img src='/store_logo.png' alt='Droneponics' title='Droneponics'>"
"        </div>"
"      </div>"

"      <div class='grid_24 ui-widget infoBoxContainer'>"
"        <div class='ui-widget-header infoBoxHeading'>&nbsp;&nbsp;"  
"            <a href='.' class='headerNavigation'>Config Landing</a>"
"            >>"
"            <a href='./wifi?' class='headerNavigation'>Auto Scan</a>"
"            >>"
"            <a href='./0wifi?' class='headerNavigation'>Manual Config</a>"
"          </div>"
"      </div>"
"      <div id='bodyContent' class='grid_16 push_4'>"
"        <h1 class='header'>{v}</h1>"
"            <div class='holder'>"
"               <div class='contentContainer'>"
"                  <div class='contentText'>"
"                      <div class='ui-widget infoBoxContainer'>";

const char MAN_HTTP_HEAD_TABLE[] PROGMEM        = 
"<div class='ui-widget-content infoBoxContents' style='text-align: left;'>Enter the details of WiFi network you want the device to connect to.</div>";

const char HTTP_HEAD_TABLE[] PROGMEM        = 
"<div class='ui-widget-content infoBoxContents' style='text-align: left;'>Select the WiFi network you want the device to connect to.</div>"
"  <div class='ui-widget-header ui-corner-top infoBoxHeading'>"
"    <table border='0' width='100%' cellspacing='0' cellpadding='2' class='productListingHeader'>"
"    <tbody>"    
"    <tr>"
"    <td align='center'></td>"
"    <td align='left'><div class='productListing-heading'>WiFi Name ('SSID')</div></td>"
"    <td align='right'>WiFi signal strength</td>"
"    <td align='center'></td>"
"    </tr>"
"    </tbody>"    
"    </table>"
"                       </div>"
"                       <div class='ui-widget-content ui-corner-bottom productListTable'>"
"    <table border='0' width='100%' cellspacing='0' cellpadding='2' class='productListingData'> "
"    <tbody>";

/** Start of our HTMl configuration Form */
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = 
"<div class='ui-widget-content infoBoxContents' style='text-align: left;'>Connect this device to a WiFi network. Select the option to find a WiFi network</div>"
"<form class='main-form' action='/wifi' method='get'>"
    "<label class='field-wrap'>"
"<button class='ui-button ui-widget ui-state-default ui-corner-all ui-priority-primary'><span class='scan_icon'></span> Configure WiFi (Auto Scan)</button>"
    "</label>"
    "</form>"
    "<br/>"
    "<form class='main-form' action='/0wifi' method='get'>"
    "<label class='field-wrap'>"
    "<button class='ui-button ui-widget ui-state-default ui-priority-secondary'><span class='man_icon'></span> Configure WiFi (Manual)</button>"
    "</label>"
    "</form>";


const char HTTP_STYLE[] PROGMEM            = "";
const char HTTP_FORM_PARAM[] PROGMEM            = "";


/** HTML snippet for wifi scanning */
const char HTTP_ITEM[] PROGMEM            = 
    "<tr>"
    "    <td align='left'> "
    "          <a href='#p' onclick='c(this)' class='a-button ui-button ui-widget ui-state-default ui-corner-all ui-button-text-icon-primary ui-priority-primary' role='button' aria-disabled='false'>"
    "               <span class='{s}'></span>"
    "               <span class='ui-button-text'>{v}</span>"
    "          </a>"
    "    </td>"
    "    <td align='right'>"    
    "          <span class='q {i}'>{r}%</span>"
    "    </td>"          
    "</tr>"; 
    
/** HTML form for saving wifi connection details */
const char HTTP_FORM_START[] PROGMEM      =
"    </tbody>"    
"    </table>"
"    </div>"

    "<form class='main-form' method='post' action='wifisave'>"
        
     "   &nbsp;"
     "   <label class='field-wrap'>"
     "      <span for='ssid'>Enter WiFi Name ('SSID'):</span>"
     "      <input id='s' name='s' type='text' length=32 placeholder='Example: Home_Network_2002' autocomplete='off'>"
     "   </label>"
 
     "   &nbsp;"
     "   <label class='field-wrap'>"
     "      <span for='ssid'>Enter WiFi Password ('Password'):</span>"
     "      <input id='p' name='p' length=64 type='password' placeholder='Password123' autocomplete='off'>"
     "   </label>";




/** The end of our HTML Form */
const char HTTP_FORM_END[] PROGMEM        = 
     "&nbsp;"
     "<label class='field-wrap'>"
     "    <button class='a-button ui-widget ui-state-default ui-corner-all ui-priority-primary' type='submit'>"
     "        <span class='save_icon'></span>Save and Connect"
     "    </button>"
     " </label>";

/** HTML snippet to recan for networks */
const char HTTP_SCAN_LINK[] PROGMEM       = 
 "<br />&nbsp;"
    "<label class='field-wrap'>"
    "    <a href='/wifi' class='a-button ui-button ui-widget ui-state-default ui-corner-all ui-button-text-icon-primary ui-priority-secondary' role='button' aria-disabled='false'>"
    "        <span class='scan_icon'></span>"
    "        <span class='ui-button-text'>Re-scan</span>"
    "    </a>"
    "</label>"
    "<br />";


const char HTTP_SAVED[] PROGMEM           = 
"<div class='ui-widget-content infoBoxContents' style='text-align: left;'>Credentials Saved.<br />"
"Attempting to connect to  WiFi network. Please wait.... <br /></div>"
"<script type=\"text/javascript\">function doPing(){attempt_count>20&&(window.clearInterval(myPinger),"
"document.getElementById(\"conn_ok\").style.display=\"block\");var t=new XMLHttpRequest;t.onload="
"function(){console.log(this.responseText),document.getElementById(\"conn_fail\").style.display=\"block\",window.clearInterval(myPinger)}"
",t.ontimeout=function(t){console.log(\"Timeout Counter is: \"+timeout_count++)},t.open(\"GET\",\"/foo\"),"
"t.timeout=1e3,t.send(null),console.log(\"Ping counter is: \"+attempt_count++)}attempt_count=0,timeout_count=0;"
"var myPinger=window.setInterval(doPing,3e3);</script>"
"<div class=\"cok\" id=\"conn_ok\">Connected to {ap} !<br />You may now close this window.<br /><img src='/qr.png'></div>"
"<div class=\"cfail\" id=\"conn_fail\">Failed to connect to {ap}!<br />"
"<a href=\"/\"><span class='retry_icon'>&nbsp</span>Click here</a> to try again.</div></div>";

/** End of the HTML page */
const char HTTP_END[] PROGMEM             = "</form></div></div></div></div></div></body></html>";
/** HTML snippet for our custom parameters portal form */

const char AP_HTTP_SAVED_FULL[] PROGMEM           = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title><script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script><style type=\"text/css\">h1 { font-weight: normal; } .msgbox { font-size:1.2rem; line-height: 1.8em; padding: 0.5em; background-color: #ddffff; border-left: 6px solid #ccc; margin-bottom:1em; } .c{text-align:center}div,input{padding:5px;font-size:1em}input{width:95%;margin-top:5px;margin-bottom:10px}body{text-align:center;font-family:verdana;}button{border:0;border-radius:.3rem;background-color:#1fa3ec;color:#fff;line-height:2.6rem;font-size:1.2rem;width:100%}.q{float:right;width:64px;text-align:right}.l{background:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==) no-repeat left center;background-size:1em;}.cfail,.cok{text-align:center; font-size:1.2rem; line-height: 2em; margin-top: 1em; padding: 0.7em; display:none;} .cfail{color: #FFFFFF;background-color: #ff8433;} .cok{ background-color: #6aff33;}</style></head><body><div style='text-align:left;display:inline-block;min-width:260px;'><div>Credentials Saved.<br />Attempting to connect to  WiFi network. Please wait.... <br /><script type=\"text/javascript\">function doPing(){attempt_count>20&&(window.clearInterval(myPinger),document.getElementById(\"conn_ok\").style.display=\"block\");var t=new XMLHttpRequest;t.onload=function(){console.log(this.responseText),document.getElementById(\"conn_fail\").style.display=\"block\",window.clearInterval(myPinger)},t.ontimeout=function(t){console.log(\"Timeout Counter is: \"+timeout_count++)},t.open(\"GET\",\"/foo\"),t.timeout=1e3,t.send(null),console.log(\"Ping counter is: \"+attempt_count++)}attempt_count=0,timeout_count=0;var myPinger=window.setInterval(doPing,3e3);</script><div class=\"cok\" id=\"conn_ok\">Connected to {ap} !<br />You may now close this window.<br /></div><div class=\"cfail\" id=\"conn_fail\">Failed to connect to {ap}!<br /><a href=\"/\">Click here</a> to try again.</div></div></div></body></html>";

#endif
