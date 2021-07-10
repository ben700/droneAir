#ifndef WC_DRONE_HTML
#define WC_DRONE_HTML

///< Define to stop re-inclusion
const char HTTP_HEADER[] PROGMEM  =
 " <html lang='en'>"
 " <head>"
 "   <link rel='icon' href='/favicon.ico' type='image/x-icon' />"
 "   <link rel='shortcut icon' href='/favicon.ico' type='image/x-icon' />"
 "   <title>Device Manager WiFi Configuration</title>"
 "   <script src='/jquery-3.5.1.min.js'></script>"
 "   <script src='variableFonts.js' type='text/javascript'></script>"
 "   <link rel='stylesheet' media='all' href='/style.css' />";

/** Scripts for our page */
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
/** End of the header section and beginning of the body */
const char HTTP_HEADER_END[] PROGMEM        = 
"</head>"
"<body>"
"    <img class='image' src='/store_logo.png' alt='Droneponics' title='Droneponics'>"
"    <br>"
"    <td class='noWrapDiv'>"
"        <span style='animation-delay: 0s;'>D</span>"
"        <span style='animation-delay: 0.111111s;'>e</span>"
"        <span style='animation-delay: 0.222222s;'>v</span>"
"        <span style='animation-delay: 0.333333s;'>i</span>"
"        <span style='animation-delay: 0.444444s;'>c</span>"
"        <span style='animation-delay: 0.555556s;'>e</span>"
"        <span style='animation-delay: 1.33333s;'>&nbsp;</span>"
"        <span style='animation-delay: 1.44444s;'>W</span>"
"        <span style='animation-delay: 1.55556s;'>i</span>"
"        <span style='animation-delay: 1.66667s;'>F</span>"
"        <span style='animation-delay: 1.77778s;'>i</span>"
"        <span style='animation-delay: 1.88889s;'>&nbsp;</span>"
"        <span style='animation-delay: 2s;'>M</span>"
"        <span style='animation-delay: 2.11111s;'>a</span>"
"        <span style='animation-delay: 2.22222s;'>n</span>"
"        <span style='animation-delay: 2.33333s;'>a</span>"
"        <span style='animation-delay: 2.44444s;'>g</span>"
"        <span style='animation-delay: 2.55556s;'>e</span>"
"        <span style='animation-delay: 2.66667s;'>r</span>"
"        <span style='animation-delay: 2.88889s;'>!</span>"
"    </td>";

const char MAN_HTTP_HEAD_TABLE[] PROGMEM        = 
"    <br><p>Enter the details of WiFi network you want the device to connect to.</p>";


const char HTTP_HEAD_TABLE[] PROGMEM        = 
"    <p>Select the WiFi network you want the device"
"        to connect to.</p>"
"    <table id='networks'>"
"            <tr id='headers'>"
"                <th align='left'>"
"                    WiFi Name ('SSID')"
"                </th>"
"                <th align='right'>"
"                  WiFi signal strength"
"                </th>"
"            </tr>";

const char HTTP_END_AUTO_TABLE[] PROGMEM        = 
"    </table>";


/** Start of our HTMl configuration Form */
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = 
"  <p>"
"    Connect this device to a WiFi network."
"    <br>"
"    Select the option to find a WiFi network"
"  </p>"
"  <br />"
"  <form action='/autoWifi' method='get'>"
"    <button class='button buttonAuto'>"
"      Configure WiFi (Auto Scan)"
"    </button>"
"  </form>"
"  <br />"
"  <form action='/manWifi' method='get'>"
"    <button class='button buttonMan'>"
"      Configure WiFi (Manual)"
"    </button>"
"  </form>";


const char HTTP_STYLE[] PROGMEM            = "";
const char HTTP_FORM_PARAM[] PROGMEM            = "";


/** HTML snippet for wifi scanning */
const char HTTP_ITEM[] PROGMEM            = 
"            <tr>"
"                <td align='left'>"
"                    <a class='linkSave {rq}' href='#p' onclick='c(this)' role='button' aria-disabled='false'>"
"                        {v}"
"                    </a>"
"                </td>"
"                <td align='right'>"
"                    <h2 class='{i}'>{r}%</h2>"
"                </td>"
"            </tr>"; 
    
/** HTML form for saving wifi connection details */
const char HTTP_FORM_START[] PROGMEM      =
"    <form method='post' action='/wifisave'>"
"        <label>"
"            <h1>Enter WiFi Name ('SSID'):</h1>"
"            <input class='input' id='s' name='s' type='text' length=32 placeholder='Example: Home_Network_2002'"
"                autocomplete='off'>"
"        </label>"
"        <br />"
"        <label>"
"            <h1>Enter WiFi Password ('Password'):</h1>"
"            <input class='input' id='p' name='p' length=64 type='password' placeholder='Password123' autocomplete='off'>"
"        </label>"
"        <br />"
"        <br />";




/** The end of our HTML Form */
const char HTTP_FORM_END[] PROGMEM        = 
"        <label>"
"            <button class='button buttonSave' type='submit'>"
"                Save and Connect"
"            </button>"
"        </label>"
"        <br />"
"        <br />";

/** HTML snippet to recan for networks */
const char HTTP_SCAN_LINK[] PROGMEM       = 
"        <label>"
"            <a href='/autoWifi' class='linkAuto' role='button' aria-disabled='false'>"
"                Re-scan"
"            </a>"
"        </label>";


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
const char HTTP_END[] PROGMEM             = 
"    </form>"
"</body>"
"</html>";

/** HTML snippet for our custom parameters portal form */

const char AP_HTTP_SAVED_FULL[] PROGMEM           = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title><script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script><style type=\"text/css\">h1 { font-weight: normal; } .msgbox { font-size:1.2rem; line-height: 1.8em; padding: 0.5em; background-color: #ddffff; border-left: 6px solid #ccc; margin-bottom:1em; } .c{text-align:center}div,input{padding:5px;font-size:1em}input{width:95%;margin-top:5px;margin-bottom:10px}body{text-align:center;font-family:verdana;}button{border:0;border-radius:.3rem;background-color:#1fa3ec;color:#fff;line-height:2.6rem;font-size:1.2rem;width:100%}.q{float:right;width:64px;text-align:right}.l{background:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==) no-repeat left center;background-size:1em;}.cfail,.cok{text-align:center; font-size:1.2rem; line-height: 2em; margin-top: 1em; padding: 0.7em; display:none;} .cfail{color: #FFFFFF;background-color: #ff8433;} .cok{ background-color: #6aff33;}</style></head><body><div style='text-align:left;display:inline-block;min-width:260px;'><div>Credentials Saved.<br />Attempting to connect to  WiFi network. Please wait.... <br /><script type=\"text/javascript\">function doPing(){attempt_count>20&&(window.clearInterval(myPinger),document.getElementById(\"conn_ok\").style.display=\"block\");var t=new XMLHttpRequest;t.onload=function(){console.log(this.responseText),document.getElementById(\"conn_fail\").style.display=\"block\",window.clearInterval(myPinger)},t.ontimeout=function(t){console.log(\"Timeout Counter is: \"+timeout_count++)},t.open(\"GET\",\"/foo\"),t.timeout=1e3,t.send(null),console.log(\"Ping counter is: \"+attempt_count++)}attempt_count=0,timeout_count=0;var myPinger=window.setInterval(doPing,3e3);</script><div class=\"cok\" id=\"conn_ok\">Connected to {ap} !<br />You may now close this window.<br /></div><div class=\"cfail\" id=\"conn_fail\">Failed to connect to {ap}!<br /><a href=\"/\">Click here</a> to try again.</div></div></div></body></html>";

#endif
