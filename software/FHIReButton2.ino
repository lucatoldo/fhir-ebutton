// Specify the various I/Opins
#define REDLED            1
#define STATUSLED         3
#define CONFIG3           4   //DIPSWITCH=3
#define CONFIG4           5   //DIPSWITCH=4
#define STATUSSWITCH     10   //MAIN
#define CONFIG1          12   //DISPWITCH=1
#define CONFIG2          14   //DIPSWITCH=2
#define BLUELED          15

#include <FS.h>               //File setup
#include <ArduinoJson.h>      //JSON parser
#include <StringSplitter.h>   //String splitter, used for A|B|C handling of hardcoded configs
// following 4 required by WiFiManager
#include <ESP8266WiFi.h>      //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

WiFiManager wifiManager;
// FHIR backend replaceable by configuration
String fhir_endpoint_url = "http://hapi.fhir.org/baseR4/Device";

// The FHIReButton has 8 possible resources (the DIPSWitch with 2*2*2, the last DIP  is used as general ON/OFF config)
// https://www.hl7.org/fhir/valueset-device-type.html
// DISPLAY                              = CODES
// -----------------------------------  = -----------------------
// Bed                                  = 229772003
// Hospital bed                         =  91537007
// Cardiac bed                          = 182590005 
// High air loss bed                    = 182592002
// Hospital bed      + Maske Beatmung   = 182592002 + 425447009
// High air loss bed + Tube Beatmung    = 182592002 + 467238001
// High air loss bed + Luftröhrenshnitt = 182592002 +   2248009
// High air loss bed + ECMO             = 182592002 +      3965

String chipID;                    // Unique identifier of the device. Exists even before WiFi.getMac(mac);
char   apID [30] ="FHIReButton-"; // Access point identifier when the button is started first time.
String uPmsg     ="";             // Message for UP status
String dowNmsg   ="";             // Message for DOWN status
String killeDmsg ="";             // Message for UNAVAILABLE status
bool newStatus;                   // boolean used only for determining when send message
bool oldStatus;                   // boolean used only for determining when send message


// Following is called at startup

void setup() {
// Identify the device, code already prepared for move to ESP32S2 (crypto)
#ifdef ESP8266
 chipID = ESP.getChipId();
#endif

#ifdef ESP32
uint32_t id = 0;
for(int i=0; i<17; i=i+8) {
  chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
}
#endif
// customise the access point ID with the CHIP ID . In this way the user can see to which device
// is going to connect, since the chipID is also engraved on the external enclosure, close to the status button.
  strcat(apID,chipID.c_str());

// init the output pins
  pinMode(STATUSLED, OUTPUT);
  pinMode(REDLED,    OUTPUT);
  pinMode(BLUELED,  OUTPUT);
//
  pinMode(STATUSSWITCH,INPUT);
//
  pinMode(CONFIG1,INPUT);
  pinMode(CONFIG2,INPUT);
  pinMode(CONFIG3,INPUT);
  pinMode(CONFIG4,INPUT);

// first turn off all the LED

  digitalWrite(REDLED,LOW);
  digitalWrite(BLUELED,LOW);
  digitalWrite(STATUSLED,LOW);

 // Turn on the status led of connected but config not yet read
  digitalWrite(STATUSLED,HIGH);

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  chipID+"FHIR-eButton"
  // and goes into a blocking loop awaiting configuration
  WiFiManagerParameter p_fhirendpoint("fhirendpoint", "FHIR Endpoint", fhir_endpoint_url.c_str(), 60);
  wifiManager.addParameter(&p_fhirendpoint);
  wifiManager.autoConnect(apID);

 // If it gets to this point it means that it did connected to the WIFI
 // determine the type of resource using the config switches

  String code = codeCheck();
  // decoding the code
  killeDmsg=FHIRgenerator(chipID,"000000","deactivated",false );
  if (code =="000") {
    uPmsg=""; dowNmsg="";
    } else if (code ="001") {
      uPmsg= FHIRgenerator(chipID,"182590005","Cardiac bed",true );
      dowNmsg= FHIRgenerator(chipID,"182590005","Cardiac bed",false );
    } else if (code ="010") {
      uPmsg==FHIRgenerator(chipID,"182592002","High air loss bed",true );
      dowNmsg==FHIRgenerator(chipID,"182592002","High air loss bed",false );
    } else if (code ="011") {
      uPmsg=FHIRgenerator(chipID,"182592002|425447009","High air loss bed|Ventilator",true );
      dowNmsg=FHIRgenerator(chipID,"182592002|425447009","High air loss bed|Ventilator",false );
    } else if (code ="100") {
      uPmsg=FHIRgenerator(chipID,"182592002|467238001","High air loss bed|Tube Respiration",true );
      dowNmsg=FHIRgenerator(chipID,"182592002|467238001","High air loss bed|Tube Respiration",false );
    } else if (code ="101") {
      uPmsg=FHIRgenerator(chipID,"182592002|248009","High air loss bed|Tracheotomy",true );
      dowNmsg=FHIRgenerator(chipID,"182592002|248009","High air loss bed|Tracheotomy",false );
    } else if (code ="110") {
      uPmsg=FHIRgenerator(chipID,"182592002|3965","High air loss bed|ECMO",true);
      dowNmsg=FHIRgenerator(chipID,"182592002|3965","High air loss bed|ECMO",false );
    } else if (code ="111") {
      uPmsg=FHIRgenerator(chipID,"229772003","Bed",true );
      dowNmsg=FHIRgenerator(chipID,"229772003","Bed",false );
   }
    if (digitalRead(CONFIG4)) {
      digitalWrite(STATUSLED,LOW);
      if (digitalRead(STATUSSWITCH)) {
        digitalWrite(REDLED,LOW);
        digitalWrite(BLUELED,HIGH);
      } else {
        digitalWrite(REDLED,HIGH);
        digitalWrite(BLUELED,LOW);
       }
    } else {
       digitalWrite(STATUSLED,LOW);
    }
}

String codeCheck() {
  String code ="";
  if (digitalRead(CONFIG1)) {
    code =  "1";
  } else {
    code = "0";
  }
  if (digitalRead(CONFIG2)) {
    code = code +"1";
  } else {
    code = code +"0";
  }
  if (digitalRead(CONFIG3)) {
    code = code+"1";
  } else {
    code = code +"0";
  }
  return code;
}


 bool postMessageToFHIRdb(String msg) {
  int firstslash= fhir_endpoint_url.indexOf('/');
  int nextslash = fhir_endpoint_url.indexOf('/',firstslash+2);
  String host = fhir_endpoint_url.substring(firstslash,nextslash);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  int httpPort = 80;
  if (fhir_endpoint_url.startsWith("https:")) {
    httpPort=443;
  }
  if (!client.connect(host, httpPort)) {
    digitalWrite(STATUSLED,HIGH);
    return false;
  }
  digitalWrite(STATUSLED,LOW);
  // This will send the request to the server
  client.print(String("POST ") + fhir_endpoint_url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Length:" + msg.length() + "\r\n" +
               "\r\n" + msg);
  String line = client.readStringUntil('\n');
  if (line.startsWith("HTTP/1.1 2")) {
    return true;
  } else {
    return false;
  }
}


String FHIRgenerator(String chipiID,String deviceCode,String deviceText, bool buttonStatus ) {
  DynamicJsonDocument doc(2000);
  doc["resourceType"] = "Device";
  JsonArray identifier = doc.createNestedArray("identifier");
  JsonObject identifier_1 = identifier.createNestedObject();
  identifier_1["system"] = "urn:oid:1.2.840.10004.1.1.1.0.0.1.0.0.1.2680";
  identifier_1["value"] = chipiID;
  JsonObject type = doc.createNestedObject("type");
  JsonArray type_coding = type.createNestedArray("coding");
  StringSplitter *deviceCodeS = new StringSplitter(deviceCode, '|',10);
  StringSplitter *deviceTextS = new StringSplitter(deviceText, '|',10);
  int itemCount = deviceCodeS->getItemCount();
  for(int i = 0; i < itemCount; i++){
     String deviceCode = deviceCodeS -> getItemAtIndex(i);
     String deviceText = deviceTextS -> getItemAtIndex(i);
     JsonObject type_coding_0 = type_coding.createNestedObject();
     type_coding_0["system"] = "SNO";
     type_coding_0["code"] = deviceCode;
     type_coding_0["text"] = deviceText;
  }
//  doc["status"] = buttonStatus;
  String output="";
  serializeJson(doc,output);
  return output;
}



void loop() {
   delay(1000);
   newStatus = digitalRead(STATUSSWITCH);
   if (newStatus!=oldStatus)  {
    if (newStatus) {
      digitalWrite(REDLED,LOW);
      digitalWrite(BLUELED,HIGH);
      digitalWrite(STATUSLED,LOW);
      postMessageToFHIRdb(uPmsg);
    } else {
      digitalWrite(REDLED,HIGH);
      digitalWrite(BLUELED,LOW);
      digitalWrite(STATUSLED,LOW);
      postMessageToFHIRdb(dowNmsg);
    }
    oldStatus=newStatus;
   }
   if (digitalRead(CONFIG4)==false) {
      digitalWrite(REDLED,HIGH);
      digitalWrite(BLUELED,HIGH);
      digitalWrite(STATUSLED,HIGH);
      postMessageToFHIRdb(killeDmsg);
      wifiManager.resetSettings();
      ESP.deepSleep(0);
   }
}

///////////////////////////////////////////////
