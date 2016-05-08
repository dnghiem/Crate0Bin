/**************************************************************************/
/*
 * This program gets the RFID data, and then sends it via curl to firebase
 * Easy fast solution using Vince's suggestion to send the RFID data and circumvent
 * Temboo and Parse.com
 * 
*/
/**************************************************************************/

// Setup the Hardware RFID reader

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// Setup the process for using curl on the linux side of the Yun
#include <Process.h>

//Set up the process to convert time to miliseconds
#include <Time.h>
#include <TimeLib.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SS   (10)

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
 Adafruit_PN532 nfc(PN532_SS);

// Setup a set of three indicator LED lights to show that the board is bin is processing
// Use digital pins 7, 8, and 9 for the indicators.
// digital Pin 7 is yellow
// digital Pin 8 is green
// digital Pin 9 is red
//

int Pin7Yellow = 7; // The wifi indicator pin
int Pin8Green = 8;  // The ready to scan pin
int Pin9Red = 9;    // The scanning pin

String timeString;

void setup(void) {

  // Initialize Bridge
  Bridge.begin();
  Serial.begin(115200);

  // Setup the output pins

  pinMode(Pin7Yellow, OUTPUT);  digitalWrite(Pin7Yellow, LOW);
  pinMode(Pin8Green, OUTPUT);   digitalWrite(Pin8Green, HIGH);
  pinMode(Pin9Red, OUTPUT);     digitalWrite(Pin9Red, LOW);
  
  Serial.println("Hello! This program reads in the RFID data and then sends it out via Linux curl program");

 
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(void) {

  wifiStatus();
  
  Serial.println(timeString);
 
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UUID (normally 7)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength); 

    String uid_string = "";
    
    for(int i = 0;i<uidLength;i++) {
      uid_string += uid[i];
    }
          digitalWrite(Pin8Green, LOW);  
          digitalWrite(Pin9Red, HIGH);    
          Serial.println(uid_string);
   
          String payload = "curl -k -X POST https://cratezero-proto.firebaseio.com/logs.json -d '{ \"timestamp\" : \"" + getTimeStamp() + "\", \"RFID_UUID\" : \""+ uid_string +"\"}'";

//          String payload = "curl -k -X POST https://cratezero-proto.firebaseio.com/logs.json -d '{ \"timestamp\" : " + String(time_int) + ", \"RFID_UUID\" : \""+ uid_string +"\"}'";

          
          Serial.println(payload);
          Process p;
          p.runShellCommand(payload);
          digitalWrite(Pin9Red, LOW);
          digitalWrite(Pin8Green, HIGH);
                      
    Serial.println("\n\nSent!");
    delay(500);
    Serial.println("\n\nScanning!");
    Serial.flush();
  }
}
