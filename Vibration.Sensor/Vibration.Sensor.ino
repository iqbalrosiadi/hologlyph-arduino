#include <Wire.h>
#include <FileIO.h>
#include <Digital_Light_TSL2561.h>
#include "DHT.h"
#include "ThingSpeak.h"
#include <Ethernet.h>
#include <SPI.h>
#include <HttpClient.h>
#include <Bridge.h>
String webserver_name = "hologlyph-hub.herokuapp.com";

// Temp & Humidity sensor
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//EthernetClient aclient; 

//#define USE_WIFI101_SHIELD
//#define USE_ETHERNET_SHIELD

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  #error "EPS8266 and ESP32 are not compatible with this example."
#endif

#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) 
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#if defined(ARDUINO_AVR_YUN)
    #include "YunClient.h"
    YunClient client;
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000)
    // Use WiFi
    #include <SPI.h>
    #include <WiFi101.h>
    char ssid[] = "<YOURNETWORK>";    //  your network SSID (name) 
    char pass[] = "<YOURPASSWORD>";   // your network password
    int status = WL_IDLE_STATUS;
    WiFiClient  client;
  #elif defined(USE_ETHERNET_SHIELD)
    // Use wired ethernet shield
    #include <SPI.h>
    #include <Ethernet.h>
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    EthernetClient client;
  #endif
#endif

#ifdef ARDUINO_ARCH_AVR
  // On Arduino:  0 - 1023 maps to 0 - 5 volts
  #define VOLTAGE_MAX 5.0
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAMD_MKR1000
  // On MKR1000:  0 - 1023 maps to 0 - 3.3 volts
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAM_DUE
  // On Due:  0 - 1023 maps to 0 - 3.3 volts
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 1023.0  
#endif



/*************************************************************************/ 
/**************************** SETUP **************************************/ 
/*************************************************************************/ 
const int pinAdc = A0;

void setup()
{
  // Initialize the Bridge and the file system
  Bridge.begin();
  ThingSpeak.begin(client);
  
  FileSystem.begin();

  // Light - it uses I2C
  Wire.begin();
  Serial.begin(9600);
  TSL2561.init();


 
}

/*************************************************************************/ 
/**************************** LOOP ***************************************/ 
/*************************************************************************/ 

//
// This function return a string with the time stamp
//
String getTimeStamp() 
{
  String result;
  
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%d/%m/%y-%T");  // mm/dd/yy and %T for the time in hh:mm:ss format
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) 
  {
    char c = time.read();
    if (c != '\n')
      result += c;
  }

  Serial.println("Time " + result);
  return result;
}


void posting_data(String sensor, String sensor_value)
{
    // Initialize the client library
  HttpClient aclient;

  // Make a HTTP request:
  aclient.get("http://"+webserver_name+"/detail/data/create/"+sensor+"/"+sensor_value);
  
}
//
// Main loop
//
void loop()
{


  boolean motion = false;

  int piezoADC = analogRead(pinAdc);
  float piezoV = piezoADC / 1023.0 * 5.0;
  Serial.println(piezoV); // Print the voltage.
  
  //posting_data("5ab2896c4bed30243324c60f", String(temp));
  //posting_data("5ab27516cef85e23a330a151", String(LDR));
  posting_data("5ab273a3d53b8f2348b0851e", String(piezoV));
 
  
  
  // Sleep for a while
   delay(200);
}


