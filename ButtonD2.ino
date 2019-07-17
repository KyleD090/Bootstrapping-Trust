

#include <SPI.h>
#include <RH_NRF24.h>
#include "Hash.h"
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>


RH_NRF24 nrf24;
int mLedPin = 4;                
int mButtonPin = 2; 
unsigned long mStartTime = 0;  
boolean mPrint = false; 
char mTemp[4]; 
char mResponse[4];
byte mHash[1];
SHA256 mHasher = SHA256(); 
//long mRandomNum; 
unsigned long mElapsedTime = 0000; 
//boolean slow = true; 
int mEnd = 0; 
String mEnemyRandom; 
String mEnemyHash; 
unsigned long mOriginalTime = 0000; 
long mRandomNum = random(100, 3000);


void setup() 
{
  Serial.begin(9600);
  Serial.println("I'm D2");
  
  while (!Serial); 
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    

  pinMode(mLedPin, OUTPUT);
  pinMode(mButtonPin, INPUT);

  randomSeed(1);
  Serial.print("Intial nonce is " );
  Serial.println(mRandomNum);
  
 
}





void loop()
{
 

  
    //listen for message
    
    if (nrf24.available())
  {
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got request: ");
      Serial.println((char*)buf);
      mEnemyHash = (char*)buf; 
      
      // Send nonce back
      
      itoa(mRandomNum, mResponse, 10); 
      unsigned char t1 = mResponse[0]; 
      unsigned char t2 = mResponse[1]; 
      unsigned char t3 = mResponse[2]; 
      unsigned char t4 = mResponse[3]; 
      uint8_t data[5] = {t1, t2, t3, t4};
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      Serial.println("Sent a reply");
      Serial.println("");
      
      mEnd++; 
      if(mEnd == 2){
        finishProtocol();
        }
      
   
      //slow = false; 
    }
    else
    {
      Serial.println("recv failed");
    }
  }




  
//When the button is pressed 


if(digitalRead(mButtonPin) == HIGH){
    digitalWrite(mLedPin, HIGH);
    mPrint = true; 
    if(mStartTime == 0){
    mStartTime = millis();
    }
  }
  else{


    //calculate elapsed time
    
    digitalWrite(mLedPin, LOW); 
    
    if(mPrint){
    unsigned long CurrentTime = millis();
    mElapsedTime = CurrentTime - mStartTime;
    mOriginalTime = mElapsedTime; 
    Serial.print("Original Elapsed Time is ");
    Serial.println(mOriginalTime);
    mPrint = false; 
    Serial.print("Elapsed Time is ");
    Serial.println(mElapsedTime);
    
    //add nonce
   
   
    Serial.print("Nonce is ");
    Serial.println(mRandomNum);
    mElapsedTime = mElapsedTime + mRandomNum;  
    Serial.print("Elapsed Time with nonce is ");
    Serial.println(mElapsedTime);
    

    //calculate hash
 

    itoa(mElapsedTime, mTemp, 10); 
    mHasher.clear(); 
    mHasher.reset(); 
    mHasher.update(mTemp, 1); 
    mHasher.finalize(mHash, 1);
    Serial.print("Calculated Hash ");
    Serial.print("  ");
    Serial.println(mHash[0]);


    //send hash

  char vSender[2]; 
  ltoa(mHash[0],vSender,10);
  unsigned char a =  vSender[0]; 
  unsigned char b = vSender[1]; 
  unsigned char c = vSender[2]; 
  uint8_t data[4] = {a, b, c};
  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
  
  //get a response


  
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (nrf24.waitAvailableTimeout(500))
  {   
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.println("");
      mEnemyRandom = (char*)buf; 
      
      mEnd++; 
      if(mEnd == 2){
        finishProtocol();
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
    
    }
    mStartTime = 0; 
    
    
    
  }



  
  
  }

void finishProtocol(){
Serial.println("REACHED END");
Serial.print("My Original duration is "); 
Serial.println(mOriginalTime);
Serial.print("Enemy nonce is "); 
Serial.println(mEnemyRandom);
mOriginalTime = mOriginalTime + mEnemyRandom.toInt();
Serial.print("Calculated Elapsed Time is "); 
Serial.println(mOriginalTime); 
itoa(mOriginalTime, mTemp, 10);
mHasher.clear(); 
mHasher.reset(); 
mHasher.update(mTemp, 1); 
mHasher.finalize(mHash, 1);
Serial.print("Calculated enemy Hash ");
Serial.print("");  
String computedmHash1 = String(mHash[0]);
Serial.println(computedmHash1);
Serial.print("enemy Hash is ");
Serial.println(mEnemyHash);
if(mEnemyHash.equals(computedmHash1)){
  Serial.print("AUTHENTICATION PASS"); 
}
else{
 Serial.print("AUTHENTICATION FAIL"); 
}
}
