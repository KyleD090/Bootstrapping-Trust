

#include <SPI.h>
#include <RH_NRF24.h>
#include "Hash.h"
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>


RH_NRF24 nrf24;
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini



int ledPin = 4;                
int buttonPin = 2; 
unsigned long StartTime = 0;  
boolean print = false; 
char temp[4]; 
byte hash[32] ;
SHA256 hasher = SHA256(); 





void setup() 
{
  Serial.begin(9600);
  
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  

  
 
}





void loop()
{
 

  
//look for messages
if (nrf24.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
//      NRF24::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }



//send a message
  

if(digitalRead(buttonPin) == HIGH){
    digitalWrite(ledPin, HIGH);
    print = true; 
    if(StartTime == 0){
    StartTime = millis();
    }
  }
  else{
    
    digitalWrite(ledPin, LOW); 
    
    if(print){
    unsigned long CurrentTime = millis();
    unsigned long ElapsedTime = CurrentTime - StartTime; 
    print = false; 
    Serial.print("Tap Time is ");
    Serial.println(ElapsedTime);

    itoa(ElapsedTime, temp, 10); 


    Serial.println("Sending to nrf24_server");
  // Send a message to nrf24_server

  
    hasher.clear(); 
    hasher.reset(); 
    hasher.update(temp, 4); 
    hasher.finalize(hash, 32);
    Serial.println("Calculated Hash ");
    Serial.println("");
    for(int i = 0; i <32; i++){
      Serial.print(hash[i]);
      }
      Serial.println(" "); 

  /*
   char storage[3]; 
   ltoa(ElapsedTime,storage,10);
   unsigned char a = hash[0]; 
   unsigned char b = hash[1];  
   unsigned char c = hash[2]; 
   unsigned char d = hash[3]; 
   unsigned char e = hash[4]; 
   unsigned char f = hash[5];  
   unsigned char g = hash[6]; 
   unsigned char h = hash[7];
   unsigned char i = hash[8]; 
   unsigned char j = hash[9];  
   unsigned char k = hash[10]; 
   unsigned char l = hash[11]; 
   unsigned char m = hash[12]; 
   unsigned char n = hash[13];  
   unsigned char o = hash[14]; 
   unsigned char p = hash[15];
   unsigned char q = hash[16]; 
   unsigned char r = hash[17];  
   unsigned char s = hash[18]; 
   unsigned char t = hash[19]; 
   unsigned char u = hash[20]; 
   unsigned char v = hash[21];  
   unsigned char w = hash[22]; 
   unsigned char x = hash[23];
   unsigned char y = hash[24]; 
   unsigned char z = hash[25];  
   unsigned char aa = hash[26]; 
   unsigned char ab = hash[27]; 
   unsigned char ac = hash[28]; 
   unsigned char ad = hash[29];  
   unsigned char ae = hash[30]; 
   unsigned char af = hash[31];
  uint8_t data[33] = {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae, af};  
  nrf24.send(data, sizeof(data));
  String myS(1, static_cast<char>(f));
  Serial.print("I sent "); 
  Serial.println(myS);
  */

  /*
   char storage[3]; 
  ltoa(ElapsedTime,storage,10);
   unsigned char a = storage[0]; 
   unsigned char b = storage[1];  
   unsigned char c = storage[2]; 
   unsigned char d = storage[3]; 
  uint8_t data[5] = {a, b, c, d};  
  nrf24.send(data, sizeof(data));

    uint8_t data[33]; 
  for(int i = 0; i <31; i++){
    data[i] = hash[i]; 
    }  
  */
  char storage[2]; 
  ltoa(hash[0],storage,10);
   unsigned char a =  storage[0]; 
   unsigned char b = storage[1]; 
   unsigned char c = storage[2]; 
  uint8_t data[4] = {a, b, c};  
  nrf24.send(data, sizeof(data));
  
  
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (nrf24.waitAvailableTimeout(500))
  { 
    // Should be a reply message for us now   
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is nrf24_server running?");
  }
  //delay(400);
    
    }
    StartTime = 0; 
    
    
    
  }



  
  
  }
