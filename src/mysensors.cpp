#include <SD.h>
#include <SPI.h>
#include <DHT.h>
#include <RTClib.h>
#include "autorange.h"
#include "mysensors.h"

/*DS3231 parameters*/
RTC_DS3231 rtc; //defining object

/*TCS34725 parameters*/
// Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


/*DHT22 parameters*/
#ifdef __AVR
    #define DHTPIN A1 // Pin 2 on Arduino Uno
#elif defined(ESP8266)
    #define DHTPIN 2 // GPIO 2 | D4 on Nodemcu
#endif

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

/*SD card parameters*/
#ifdef __AVR
    const int pinCS = 4; // Pin 4 on Arduino Uno
#elif defined(ESP8266)
    const int pinCS = 15; // GPIO 15 | D8 on Nodemcu
#endif


tcs34725 rgb_sensor; 


void initializeSDCard(void)
{

    /*initializing SD card*/
    Serial.print("Initializing SD card...");
    if (!SD.begin(pinCS)) 
    {
        Serial.println("Card failed, or not present");
        while (1) { delay(0); }; //if there is no SD card present, the program does nothing
    }
    Serial.println("SD Card initialized.");

}


void initializeRTC(void)
{
    /*initializing DS3231(rtc)*/
    Serial.println("DS3231 initialized");
    rtc.begin();
    
        /*setting time*/
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    Serial.println("RTC was set");
}


// void initializeTCS34725(void)
// {
//     /*initializing TCS34725*/
//     if (tcs.begin()) 
//     {
//         Serial.println("TCS34725 initialized");
//     } 
//     else 
//     {
//         Serial.println("No TCS34725 found ... check your connections");
//         // while (1);
//     }

// }


void initializeDHT22(void)
{
    /*initializing DHT22*/
    Serial.println("DHT22 initialized");
    dht.begin();
}


String getFilenameTimestamp(void)
{
    // OBS: FAT filesystem do not allow more than 8 char for the filename (without extension)
    String filename;
    DateTime now = rtc.now();
    filename = "";
    filename += String(now.month(), DEC);
    filename += String(now.day(), DEC);
    filename += String(now.hour(), DEC);
    filename += String(now.minute(), DEC);
    filename += ".txt";
    return filename;
}


String getDateTime(char sep, char date_sep, char hour_sep)
{
    DateTime now = rtc.now();

    String raw="";
    raw += String(now.year(), DEC) + date_sep;
    raw += String(now.month(), DEC) + date_sep;
    raw += String(now.day(), DEC);
    raw += String(' ');
    raw += String(now.hour(), DEC) + hour_sep;
    raw += String(now.minute(), DEC) + hour_sep;
    raw += String(now.second(), DEC);

    return raw;
}


String getTemperatureData(char sep)
{
    float h = dht.readHumidity();
    // float h=+6.66;
    // float t =-6.66;
    float t = dht.readTemperature(); //read temperature as Celcius, for Fahrenheit put argument as "true"

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) 
    {
        Serial.println("Failed to read from DHT sensor!");
        return String("<error_dt_read>");
    }

    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);


    String s="";
    s += String(h) + sep;
    s += String(t) + sep;
    s += String(hic);
    return s;
}


// String getLightData(char sep)
// { 
//     tcs.setInterrupt(true);  //turn off LED

//     uint16_t r, g, b, c, colorTemp, lux;

//     tcs.getRawData(&r, &g, &b, &c);


//     colorTemp = tcs.calculateColorTemperature(r, g, b);
//     lux = tcs.calculateLux(r, g, b);


//     String s="";
//     s += String(colorTemp) + sep;
//     s += String(lux) + sep;
//     s += String(r) + sep;
//     s += String(g) + sep;
//     s += String(b) + sep;
//     s += String(c) + sep;

//     return s;
// }


void initializeTCS34725(void)
{
    /*initializing TCS34725*/
    if (rgb_sensor.begin()) 
    {
        Serial.println("TCS34725 initialized");
    } 
    else 
    {
        Serial.println("No TCS34725 found ... check your connections");
        // while (1);
    }

}

String getLightData(char sep)
{ 

    rgb_sensor.getData();

    Serial.print(F("Gain:")); 
    Serial.print(rgb_sensor.againx); 
    Serial.print(F("x "));
    Serial.print(F("Time:")); 
    Serial.print(rgb_sensor.atime_ms); 
    Serial.print(F("ms (0x")); 
    Serial.print(rgb_sensor.atime, HEX);   
    Serial.println(F(")"));
      
    Serial.print(F("Raw R:")); 
    Serial.print(rgb_sensor.r); 
    Serial.print(F(" G:")); 
    Serial.print(rgb_sensor.g); 
    Serial.print(F(" B:")); 
    Serial.print(rgb_sensor.b); 
    Serial.print(F(" C:")); 
    Serial.println(rgb_sensor.c); 

    Serial.print(F("IR:")); 
    Serial.print(rgb_sensor.ir); 
    Serial.print(F(" CRATIO:"));
    Serial.print(rgb_sensor.cratio); 
    Serial.print(F(" Sat:"));
    Serial.print(rgb_sensor.saturation); 
    Serial.print(F(" Sat75:"));
    Serial.print(rgb_sensor.saturation75); 
    Serial.print(F(" "));
    Serial.println(rgb_sensor.isSaturated ? "*SATURATED*" : "");

    Serial.print(F("CPL:")); 
    Serial.print(rgb_sensor.cpl);
    Serial.print(F(" Max lux:")); 
    Serial.println(rgb_sensor.maxlux);

    Serial.print(F("Compensated R:")); 
    Serial.print(rgb_sensor.r_comp); 
    Serial.print(F(" G:")); 
    Serial.print(rgb_sensor.g_comp); 
    Serial.print(F(" B:")); 
    Serial.print(rgb_sensor.b_comp); 
    Serial.print(F(" C:")); 
    Serial.println(rgb_sensor.c_comp); 
      
    Serial.print(F("Lux:")); 
    Serial.print(rgb_sensor.lux);
    Serial.print(F(" CT:")); 
    Serial.print(rgb_sensor.ct);
    Serial.println(F("K")); 
      
    Serial.println();

    String s="";
    s += String(rgb_sensor.lux) + sep;
    s += String(rgb_sensor.ct) + sep;
    s += String(rgb_sensor.r_comp) + sep;
    s += String(rgb_sensor.g_comp) + sep;
    s += String(rgb_sensor.b_comp) + sep;
    s += String(rgb_sensor.c_comp) + sep;

    return s;
}
