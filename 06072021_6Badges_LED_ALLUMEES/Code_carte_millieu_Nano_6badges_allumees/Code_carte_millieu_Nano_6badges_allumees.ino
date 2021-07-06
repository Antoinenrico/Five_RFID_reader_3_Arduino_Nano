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

#define PIN_LED_VERTE_10 8
#define PIN_LED_VERTE_11 4
#define PIN_LED_VERTE_2 5
#define PIN_LED_VERTE_30 7
#define PIN_LED_VERTE_31 6
#define PIN_RELAIS_COMMANDE 2
#define SS_2_PIN 10

#define NR_OF_READERS   1

int state10=0;
int state11=0;
int state2=0;
int state30=0;
int state31=0;
int stateVictory=0;

byte ssPins[] = {SS_2_PIN};
byte v[] ={0,0};
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
String keyOk00="9 34 3d 99";
String keyOk01="19 c2 91 c2";
String keyOk02="21 21 e 3c";
String keyOk03="11 b e 3c";
String keyOk04="2a 27 6b 27"; // carte
String keyOk05="2a ad ae 27"; // carte
/**
 * Initialize.
 */
void setup() {
  pinMode(PIN_LED_VERTE_10, INPUT);
  pinMode(PIN_LED_VERTE_11, INPUT);
  pinMode(PIN_LED_VERTE_2, OUTPUT);
  pinMode(PIN_LED_VERTE_30, INPUT);
  pinMode(PIN_LED_VERTE_31, INPUT);
  pinMode(PIN_RELAIS_COMMANDE, OUTPUT);
  digitalWrite(PIN_LED_VERTE_2, LOW);
  digitalWrite(PIN_RELAIS_COMMANDE, HIGH);
  
  
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
    digitalWrite(PIN_LED_VERTE_2, LOW);
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
            Serial.println("Succes LED 2");
            state2=1;
            digitalWrite(PIN_LED_VERTE_2, HIGH);}
          else{
            digitalWrite(PIN_LED_VERTE_2, LOW);
            state2=0;
            Serial.println("Failed LED 2");
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
  statu_lect_open();
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


void statu_lect_open(){
  if(digitalRead(PIN_LED_VERTE_10) && state10==0){
    Serial.println("LED 10 OK");
    state10=1;
  }
  if(digitalRead(PIN_LED_VERTE_11) && state11==0){
    Serial.println("LED 11 OK");
    state11=1;
  }
  if(digitalRead(PIN_LED_VERTE_30) && state30==0 ){
    Serial.println("LED 30 OK");
    state30=1;
  }
  if(digitalRead(PIN_LED_VERTE_31) && state31==0 ){
    Serial.println("LED 31 OK");
    state31=1;
  }

  if(digitalRead(PIN_LED_VERTE_10)==LOW && state10==1){
    Serial.println("LED 10 NOK");
    state10=0;
  }
  if(digitalRead(PIN_LED_VERTE_11)==LOW && state11==1){
    Serial.println("LED 11 NOK");
    state11=0;
  }
  if(digitalRead(PIN_LED_VERTE_30)==LOW && state30==1){
    Serial.println("LED 30 NOK");
    state30=0;
  }
  if(digitalRead(PIN_LED_VERTE_31)==LOW && state31==1){
    Serial.println("LED 31 NOK");
    state31=0;
  }
  

  

  if(stateVictory==1){
      for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

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
            Serial.println("Succes LED 2");
            state2=1;
            digitalWrite(PIN_LED_VERTE_2, HIGH);}
          else{
            digitalWrite(PIN_LED_VERTE_2, LOW);
            digitalWrite(PIN_RELAIS_COMMANDE, HIGH);
            state2=0;
            Serial.println("Failed LED 2");
            Serial.println("");
            }
        }
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
      delay(100);
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
  } // end if victory =1

  if(state10==1 && state11==1 && state2==1 && state30==1 && state31==1){
    Serial.println("VICTOIRE");
    stateVictory=1;
    digitalWrite(PIN_RELAIS_COMMANDE, LOW);  
  }else
    {
      digitalWrite(PIN_RELAIS_COMMANDE, HIGH);
      stateVictory=0;
    }
}
