/*
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing how to read new NUID from a PICC to serial.
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

   Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
   Reader on the Arduino SPI interface.

   When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
   then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
   you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
   will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
   when removing the PICC from reading distance too early.

   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN_1  7
#define SS_PIN_2  10
#define NB_RFIDS  2
#define PIN_LED_VERTE_10 3
#define PIN_LED_VERTE_11 4

byte SS_PINS[] = {SS_PIN_1, SS_PIN_2};
byte v[] ={0,0};

// Lecteur Blanc
String keyOk10="37 96 e3 60";
String keyOk11="6a 16 e6 80";
String keyOk12="9a 59 e 3c";
String keyOk13="d8 43 e 3c";
String keyOk14="2a f8 b0 27"; // carte
String keyOk15="2a 7e 1f 27"; // carte


// Lecteur Transparant 
String keyOk00="ca 57 f4 81";
String keyOk01="7b 9a b 3c";
String keyOk02="5b a7 e 3c";
String keyOk03="2a 61 d9 27"; // Carte 
String keyOk04="2a 7e 34 27"; // Carte 

MFRC522 rfid[NB_RFIDS];

int succes10=0;
int succes11=0;

void setup() {

  pinMode(PIN_LED_VERTE_10, OUTPUT);
  pinMode(PIN_LED_VERTE_11, OUTPUT);
  digitalWrite(PIN_LED_VERTE_10, LOW);
  digitalWrite(PIN_LED_VERTE_11, LOW);
  
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Init...");

  SPI.begin(); // Init SPI bus

  for (int i = 0; i < NB_RFIDS; i++) {
    rfid[i].PCD_Init(SS_PINS[i], MFRC522::UNUSED_PIN);
    rfid[i].PCD_SetAntennaGain(rfid[i].RxGain_max);
  }
  Serial.println("Init OK");
}

void loop() {
  for (int i = 0; i < NB_RFIDS; i++) {
    lect2(rfid[i],i);
  }
  if(succes10==1 && succes11==1){
    digitalWrite(PIN_LED_VERTE_10, HIGH);
    Serial.println("YA");
    digitalWrite(PIN_LED_VERTE_11, HIGH);
    delay(10000);
    succes10=0;
    succes11=0;
  }
}


void lect2(MFRC522 rfid, int i) {
  // Si on ne detecte pas une carte on reboucle au debut
    digitalWrite(PIN_LED_VERTE_10, LOW);
    digitalWrite(PIN_LED_VERTE_11, LOW);
  
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // as t'on lu la carte
  if (!rfid.PICC_ReadCardSerial())
  
    return;

  // recuperation de l'UID et concatÃ©nation de sa valeur en hex dans un string
  String myKey = String(rfid.uid.uidByte[0], HEX) + " " + String(rfid.uid.uidByte[1], HEX) + " " + String(rfid.uid.uidByte[2], HEX) + " " + String(rfid.uid.uidByte[3], HEX);

  // j'affiche la valeur de la carte sur le serial
  Serial.println(myKey);
  //Serial.println(i);
  if(i==0){
    Serial.println("Lecteur 10");
    if(keyOk00 == myKey || keyOk01 == myKey || keyOk02 == myKey || keyOk03 == myKey || keyOk04 == myKey){
    digitalWrite(PIN_LED_VERTE_10, HIGH);
    succes10=1;
  }else{digitalWrite(PIN_LED_VERTE_10, LOW);
        succes10=0;
        }
  }
  if(i==1 ){
    Serial.println("Lecteur 11");
    if(keyOk10 == myKey || keyOk11 == myKey || keyOk12 == myKey || keyOk13 == myKey || keyOk14 == myKey || keyOk15 == myKey){
    digitalWrite(PIN_LED_VERTE_11, HIGH);
    succes11=1;
  }else{digitalWrite(PIN_LED_VERTE_11, LOW);
        succes11=0;
        }
  }
  // Halt PICC, fin de lecture de carte
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
}
  
