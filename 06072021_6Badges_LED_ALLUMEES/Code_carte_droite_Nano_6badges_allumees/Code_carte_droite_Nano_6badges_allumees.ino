/**
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from more than one PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to read data from more than one PICC (that is: a RFID Tag or Card) using a
 * MFRC522 based RFID Reader on the Arduino SPI interface.
 *
 * Warning: This may not work! Multiple devices at one SPI are difficult and cause many trouble!! Engineering skill
 *          and knowledge are required!
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         8          // Configurable, see typical pin layout above
#define SS_1_PIN        9         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        10          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1
#define PIN_LED_VERTE_31 6
#define PIN_LED_VERTE_30 7


#define NR_OF_READERS   2

byte ssPins[] = {SS_1_PIN, SS_2_PIN};
byte v[] ={0,0};
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
// Lecteur 31 Vert et Jaune 
String keyOk10="79 f7 fe b9";
String keyOk11="89 73 58 a2";
String keyOk12="2a 4e 22 27"; // Carte 
String keyOk13="2a 9d a9 27"; // Carte
String keyOk14="41 55 e 3c";
String keyOk15="cf 55 e 3c";

// Lecteur 30 ROUGE
String keyOk00="89 5d 2d 99";
String keyOk01="39 4b b3 b1";
String keyOk02="2a 2e d2 27"; // Carte
String keyOk03="2a bc 65 27"; // Carte 
String keyOk04="2 95 e 3c";
String keyOk05="90 90 e 3c";
/**
 * Initialize.
 */
void setup() {
  pinMode(PIN_LED_VERTE_31, OUTPUT);
  pinMode(PIN_LED_VERTE_30, OUTPUT);  
  digitalWrite(PIN_LED_VERTE_31, LOW);
  digitalWrite(PIN_LED_VERTE_30, LOW);
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  
  SPI.begin();        // Init SPI bus
  //do{
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    
    mfrc522[reader].PCD_Init(ssPins[reader],  MFRC522::UNUSED_PIN); // Init each MFRC522 card
    delay(1000);
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
    mfrc522[reader].PCD_SetAntennaGain(mfrc522[reader].RxGain_max);
    v[reader] = mfrc522[reader].PCD_ReadRegister(mfrc522[reader].VersionReg);
    Serial.println(v[reader],HEX);
    }
  
  //}while(v[0]!=0x92 && v[1]!=0x92);//v != 146
  //Serial.println(v[0],HEX);
  //Serial.println(v[1],HEX);
  Serial.println("init fait");
}

/**
 * Main loop.
 */
void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    digitalWrite(PIN_LED_VERTE_31, LOW);
    digitalWrite(PIN_LED_VERTE_30, LOW);
  
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));
      
        if(reader==0){
          String myKey=String(mfrc522[reader].uid.uidByte[0],HEX) + " " + String(mfrc522[reader].uid.uidByte[1],HEX) + " " + String(mfrc522[reader].uid.uidByte[2],HEX) + " " + String(mfrc522[reader].uid.uidByte[3],HEX);
          Serial.println(myKey);
          Serial.println(keyOk00);
          Serial.println(keyOk01);
          if(keyOk00 == myKey || keyOk01 == myKey || keyOk02 == myKey || keyOk03 == myKey || keyOk04 == myKey || keyOk05 == myKey){
            Serial.println("Succes LED 31");
            digitalWrite(PIN_LED_VERTE_31, HIGH);}
          else{
            digitalWrite(PIN_LED_VERTE_31, LOW);
            Serial.println("Failed LED 31");
            Serial.println("");
            }
        }
        if(reader==1){
          String myKey=String(mfrc522[reader].uid.uidByte[0],HEX) + " " + String(mfrc522[reader].uid.uidByte[1],HEX) + " " + String(mfrc522[reader].uid.uidByte[2],HEX) + " " + String(mfrc522[reader].uid.uidByte[3],HEX);
          Serial.println(keyOk10);
          Serial.println(keyOk11);
          Serial.println(myKey);
          if(keyOk10 == myKey || keyOk11 == myKey || keyOk12 == myKey || keyOk13 == myKey || keyOk14 == myKey || keyOk15 == myKey){
            Serial.println("Succes LED 30");
            digitalWrite(PIN_LED_VERTE_30, HIGH);}
          else{
            digitalWrite(PIN_LED_VERTE_30, LOW);
            Serial.println("Failed LED 30");
            Serial.println("");
            }
        }
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
      delay(500);
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
