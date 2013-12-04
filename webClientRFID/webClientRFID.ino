/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <SoftwareSerial.h>
#include <SD.h>
//#include <SimpleSDAudio.h>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "D's Nets"           // cannot be longer than 32 characters!
#define WLAN_PASS       "gEt 0n iT!"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

// What page to grab!
#define PORT 8080
#define LOCALSERVER 3232236038 //"192.168.2.6"
#define WEBSITE      "192.168.2.6"
#define WEBPAGE      "/hello"


/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

SoftwareSerial mySerial(4, 6); // RX, TX
int RFIDResetPin = 7;
uint32_t ip;
Adafruit_CC3000_Client www;
char tagString[13];
int sdSelect = 8;

void setup(void)
{
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  pinMode(2, OUTPUT);
  /*
  pinMode(SS, OUTPUT);
  pinMode(sdSelect, OUTPUT);
  pinMode(ADAFRUIT_CC3000_CS, OUTPUT);
  digitalWrite(sdSelect, HIGH);
  digitalWrite(ADAFRUIT_CC3000_CS, OUTPUT);
  */
  
  Serial.begin(115200); 
  
  Serial.println(F("Hello, CC3000!\n")); 

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  // Optional SSID scan
  // listSSIDResults();
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  ip = LOCALSERVER;//0;
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
  
  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");  
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */  
     // If your SD card CS-Pin is not at Pin 4, enable and adapt the following line:
  //SdPlay.setSDCSPin(sdSelect);
  
  //cc3000.stop();
  
  if (!SD.begin(sdSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  /*
  // Init SdPlay and set audio mode and activate autoworker
  if (!SdPlay.init(SSDA_MODE_HALFRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)) {
    while(1){
      Serial.println("Could not initialize SD card for playback");  
      return;
    }; // Error while initialization of SD card -> stop.
  } 
  
  // Select file to play
  if(!SdPlay.setFile("C_SOUND.AFM")) {
    while(1){
        Serial.println("Could not set file to C_SOUND.AFM");
        return;
    }; // Error file not found -> stop.
  }
  */
  www = cc3000.connectTCP(ip, PORT);
  mySerial.begin(9600);
  Serial.println("Start reading cards...");
}

void loop(void)
{
 
 readRFID(tagString);
 //Serial.println(tagString);
 //httpGetSound();
 //rfidCleanup(); 
 
 delay(1000);
}

void readRFID(char tagString[]){
  int index = 0;
  boolean reading = false;
  
  
  while(mySerial.available()){
    
    byte readByte = mySerial.read(); //read next available byte
    
    if(readByte == 2) 
    { 
      reading = true; //begining of tag
      lightLED(2);
    }
    
    if(readByte == 3){
      reading = false; //end of tag
      httpGetSound();
      playSample();
    }
    
    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
      //Serial.println(readByte);
    }
  }
  
  clearTag(tagString); //Clear the char of all value
  resetReader(); //eset the RFID reader
}

/*void rfidCleanup(void){

}*/

void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}

void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

void playSample(void)
{
  
  //digitalWrite(ADAFRUIT_CC3000_CS, HIGH);
  //digitalWrite(sdSelect, LOW);
  //cc3000.stop();
  /*
  ccspi.SpiPauseSpi();
  if (!SD.begin(sdSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  digitalWrite(sdSelect, HIGH);
  */
  //digitalWrite(ADAFRUIT_CC3000_CS, LOW);
  //SD.stop()
  //cc3000.begin();
  //ccspi.SpiResumeSpi();
}

void httpGetSound(void)
{
    //Adafruit_CC3000_Client 
   /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  
  
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(WEBPAGE);
    www.fastrprint("/?rfid=");
    www.fastrprint(tagString);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));
    Serial.println(F("Disconnecting..."));
    cc3000.disconnect();    
    return;
  }

  Serial.println(F("-------------------------------------"));
  
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
  byte lastCR = 0;
  boolean storeRes = false;
  int index = 0;
  char response[14];
  
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      if (c == '\n' || c == '\r') {
         //Serial.println("Carriage return"); 
         if (lastCR > 2) {
           //Serial.println("Beginning of HTTP Response body");
           storeRes = true;  
       }else {
           lastCR += 1;
         }
      }
      else {
         lastCR = 0;
      }
      
      //Serial.print(c);
      if(storeRes && index < 37){
        response[index] = c;
        index += 1;
      }
      lastRead = millis();
    }
  }
  //www.close();
  //Serial.println(F("-------------------------------------"));
  Serial.println(response);
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  
  //Serial.println(F("\n\nDisconnecting"));
  //cc3000.disconnect();

}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
