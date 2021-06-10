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

#define SS_PIN_1  6
#define SS_PIN_2  7
#define NB_RFIDS  2
#define PIN_LED_VERTE_0 A5
#define PIN_LED_VERTE_1 A4
#define PIN_LED_VERTE_2 A3
#define PIN_LED_VERTE_4 2
#define PIN_LED_VERTE_5 3
#define RELAIS_ELECTRO_AIMANT 4
int state0=0;
int state1=0;
int state2=0;
int state3=0;
int state4=0;

byte SS_PINS[] = {SS_PIN_1, SS_PIN_2};
byte v[] ={0,0};
String keyOk10="37 96 e3 60";
String keyOk11="6a 16 e6 80";
String keyOk00="ca 57 f4 81";
String keyOk01="ca 57 f4 81";

MFRC522 rfid[NB_RFIDS];

void setup() {

  pinMode(PIN_LED_VERTE_0, INPUT);
  pinMode(PIN_LED_VERTE_1, INPUT);
  pinMode(PIN_LED_VERTE_2, INPUT);
  pinMode(PIN_LED_VERTE_4, OUTPUT);
  pinMode(PIN_LED_VERTE_5, OUTPUT);
  pinMode(RELAIS_ELECTRO_AIMANT, OUTPUT);
  digitalWrite(PIN_LED_VERTE_4, LOW);
  digitalWrite(PIN_LED_VERTE_5, LOW);
  digitalWrite(RELAIS_ELECTRO_AIMANT, HIGH);
  
  Serial.begin(115200);
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
  statu_lect_open();

}

void lect(MFRC522 rfid) {
  // Si on ne detecte pas une carte on reboucle au debut
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // as t'on lu la carte
  if (!rfid.PICC_ReadCardSerial())
    return;

  // recuperation de l'UID et concatÃ©nation de sa valeur en hex dans un string
  String myKey = String(rfid.uid.uidByte[0], HEX) + " " + String(rfid.uid.uidByte[1], HEX) + " " + String(rfid.uid.uidByte[2], HEX) + " " + String(rfid.uid.uidByte[3], HEX);

  // j'affiche la valeur de la carte sur le serial
  Serial.println(myKey);

  // Halt PICC, fin de lecture de carte
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
}

void lect2(MFRC522 rfid, int i) {
  // Si on ne detecte pas une carte on reboucle au debut
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // as t'on lu la carte
  if (!rfid.PICC_ReadCardSerial())
    return;

  // recuperation de l'UID et concatÃ©nation de sa valeur en hex dans un string
  String myKey = String(rfid.uid.uidByte[0], HEX) + " " + String(rfid.uid.uidByte[1], HEX) + " " + String(rfid.uid.uidByte[2], HEX) + " " + String(rfid.uid.uidByte[3], HEX);

  // j'affiche la valeur de la carte sur le serial
  Serial.println(myKey);
  Serial.println(i);
  if(i==0){
    if(keyOk00 == myKey || keyOk01 == myKey){
  
    state3=1;
    digitalWrite(PIN_LED_VERTE_4, HIGH);
  }else{digitalWrite(PIN_LED_VERTE_4, LOW);}
  }
  if(i==1 ){
    if(keyOk10 == myKey || keyOk11 == myKey){
    state4=1;
    digitalWrite(PIN_LED_VERTE_5, HIGH);
  }else{digitalWrite(PIN_LED_VERTE_5, LOW);}
  }
  // Halt PICC, fin de lecture de carte
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
}

void statu_lect_open(){
  if(digitalRead(PIN_LED_VERTE_0) && state0==0){
    Serial.println("LED 0 OK");
    state0=1;
  }
  if(digitalRead(PIN_LED_VERTE_1) && state1==0){
    Serial.println("LED 1 OK");
    state1=1;
  }
  if(digitalRead(PIN_LED_VERTE_2) && state2==0 ){
    Serial.println("LED 2 OK");
    state2=1;
  }  

  if(state0==1 && state1==1 && state2==1 && state3==1 && state4==1){
    Serial.println("VICTOIRE");
    digitalWrite(RELAIS_ELECTRO_AIMANT, LOW);  
  }
}
  
