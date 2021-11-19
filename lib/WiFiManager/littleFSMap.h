#ifndef LittleFSMap_h
#define LittleFSMap_h

#include "Arduino.h"


String IDPath = "/data/id/";

String StylePath = "/data/style/";
String ImgPath = "/data/img/";
String IconPath = "/data/icons/";
String FontPath = "/data/fonts/";
String ScriptPath = "/data/script/";

String favicon = ImgPath + "favicon.ico";
String logo = ImgPath + "logo.jpeg";
String store_logo = ImgPath + "store_logo.png";

String qr = IDPath + "qr.png";
String style = StylePath + "style.css";

String variableFonts = ScriptPath + "variableFonts.js";
String jquery = ScriptPath + "jquery-3.5.1.min.js";

String PoppinsRegular = FontPath + "Poppins-Regular.ttf";
String MerriweatherSans = FontPath + "MerriweatherSans.ttf";

String wifi_lock_black_24dp = IconPath + "wifi_lock_black_24dp.svg";
String lock_open_black_24dp = IconPath + "lock_open_black_24dp.svg";
String wifi_black_24dp = IconPath + "wifi_black_24dp.svg";
String keyboard_black_24dp = IconPath + "keyboard_black_24dp.svg";
String save_black_24dp = IconPath + "save_black_24dp.svg";



#endif