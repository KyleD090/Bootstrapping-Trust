

#include <SPI.h>
#include <RH_NRF24.h>
#include "Hash.h"
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>


RH_NRF24 nrf24;
int ledPin = 4;                
int buttonPin = 2; 
unsigned long startTime = 0;  
boolean print = false; 
char temp[4]; 
char response[4];
byte hash[1];
SHA256 hasher = SHA256(); 
//long randomNum; 
unsigned long ElapsedTime = 0000; 
//boolean slow = true; 
int End = 0; 
String enemyRandom; 
String enemyHash; 
unsigned long ogTime = 0000; 
long randomNum = random(100, 3000);


void setup() 
{
  Serial.begin(9600);
  Serial.println("I'm D2");
  
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

  randomSeed(1);
  //randomNum = random(100, 3000);
  Serial.print("Intial nonce is " );
  Serial.println(randomNum);
  
 
}





void loop()
{
 

  
//look for messages
if (nrf24.available())
  {
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
//      NRF24::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
      enemyHash = (char*)buf; 
      
      // Send a reply
      //Serial.print("Random Num berfore sending is ");
      //Serial.println(randomNum);
      itoa(randomNum, response, 10); 
      unsigned char t1 = response[0]; 
      unsigned char t2 = response[1]; 
      unsigned char t3 = response[2]; 
      unsigned char t4 = response[3]; 
      uint8_t data[5] = {t1, t2, t3, t4};
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      End++; 
      //Serial.print("End is ");
      //Serial.println(End);
      if(End == 2){
        Serial.print("REACHED END");
        Serial.print("My Original duration is "); 
        Serial.println(ogTime);
        Serial.print("Enemy nonce is "); 
        Serial.println(enemyRandom);
        ogTime = ogTime + enemyRandom.toInt();
        Serial.print("Calculated Elapsed Time is "); 
        Serial.println(ogTime); 
        itoa(ogTime, temp, 10);
        hasher.clear(); 
        hasher.reset(); 
        hasher.update(temp, 1); 
        hasher.finalize(hash, 1);
        Serial.print("Calculated enemy Hash ");
        Serial.print("");
        //String computedHash1 = String(hash[0] + hash[1] + hash[2]+ hash[3]); 
        String computedHash1 = String(hash[0]);
        Serial.print(computedHash1);
        Serial.print("enemy hash is ");
        Serial.println(enemyHash);
        
        if(enemyHash.equals(computedHash1)){
          Serial.print("AUTHENTICATION PASS"); 
          }
        else{
          Serial.print("AUTHENTICATION FAIL"); 
          }
        }
      Serial.println("Sent a reply");
      Serial.println("");
   
      //slow = false; 
    }
    else
    {
      Serial.println("recv failed");
    }
  }




  
//This code is triggered if a button is pressed 
if(digitalRead(buttonPin) == HIGH){
    digitalWrite(ledPin, HIGH);
    print = true; 
    if(startTime == 0){
    startTime = millis();
    }
  }
  else{
    
    digitalWrite(ledPin, LOW); 
    
    if(print){
    unsigned long CurrentTime = millis();
    ElapsedTime = CurrentTime - startTime;
    ogTime = ElapsedTime; 
    Serial.print("OG TIME IS ");
    Serial.println(ogTime);
    print = false; 
    Serial.print("Elapsed Time is ");
    Serial.println(ElapsedTime);
    
 
   
   
    //slow = true; 
    Serial.print("Nonce is ");
    Serial.println(randomNum);
    ElapsedTime = ElapsedTime + randomNum;  
    Serial.print("Elapsed Time with nonce is ");
    Serial.println(ElapsedTime);
    itoa(ElapsedTime, temp, 10); 


    //Serial.println("Sending to nrf24_server");
 

  
    hasher.clear(); 
    hasher.reset(); 
    hasher.update(temp, 1); 
    hasher.finalize(hash, 1);
    Serial.print("Calculated Hash ");
    Serial.print("  ");
    for(int i = 0; i <1; i++){
      Serial.println(hash[i]);
      }
      //Serial.println(" "); 



  char b0[2]; 
  ltoa(hash[0],b0,10);
  unsigned char a =  b0[0]; 
  unsigned char b = b0[1]; 
  unsigned char c = b0[2]; 
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
      Serial.println("");
      enemyRandom = (char*)buf; 
      
      End++; 
     // Serial.print("End is ");
      //Serial.println(End);
      if(End == 2){
        Serial.print("REACHED END");
        Serial.print("My Original duration is "); 
        Serial.println(ogTime);
        Serial.print("Enemy nonce is "); 
        Serial.println(enemyRandom);
        ogTime = ogTime + enemyRandom.toInt(); 
        Serial.print("Calculated Elapsed Time is "); 
        Serial.println(ogTime); 
        itoa(ogTime, temp, 10);
        hasher.clear(); 
        hasher.reset(); 
        hasher.update(temp, 1); 
        hasher.finalize(hash, 1);
        Serial.print("Calculated enemy Hash ");
        Serial.print("");
          //String computedHash1 = String(hash[0] + hash[1] + hash[2]+ hash[3]); 
          String computedHash1 = String(hash[0]);
        Serial.print(computedHash1);
        Serial.print("enemy hash is ");
        Serial.println(enemyHash);
        if(enemyHash.equals(computedHash1)){
          Serial.print("AUTHENTICATION PASS"); 
          }
        else{
          Serial.print("AUTHENTICATION FAIL"); 
          }
        }

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
    startTime = 0; 
    
    
    
  }



  
  
  }
