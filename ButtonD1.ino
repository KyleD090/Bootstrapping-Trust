

#include <SPI.h>
#include <RH_NRF24.h>
#include <Hash.h>
#include <Crypto.h>
#include <SHA256.h>
#include <string.h>

RH_NRF24 nrf24;
int mLedPin = 4;                
int mButtonPin = 2; 
String mMessage; 
unsigned long mStartTime = 0;  
boolean mPrint = false; 
char mTemp[4]; 
char mResponse[4];
byte mHash[14];
byte mHash2[14];
SHA256 mHasher = SHA256(); 
unsigned long mElapsedTime = 0000;
int mEnd = 0; 
String mEnemyRandom; 
String mEnemyHash; 
unsigned long mOriginalTime = 0000;  
long mRandomNum = random(100, 3000);
int mStartCapture = 0;
unsigned long mCaptureTime = 0000;
unsigned long mUpdateTime = 0000;
unsigned long mOriginalDurations[20];
unsigned long mFinalVector[10];
int mArrayCounter = 0; 
unsigned long mBins[10];
boolean mStop = true;
int mRespond = 0;  
String mUnknownNonce[1];  
String mUnknownHash[14]; 

void setup() 
{
  
  Serial.begin(9600);
  Serial.println("I'm D1");
  while (!Serial); 
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    


  pinMode(mLedPin, OUTPUT);
  pinMode(mButtonPin, INPUT);

  randomSeed(2);
  mRandomNum = random(100, 3000);
  Serial.print("Intial nonce is " );
  Serial.println(mRandomNum);
  
  //String result = sha1("test string");
  

  
  mBins[0] = 500;
  mBins[1] = 1000;
  mBins[2] = 1500;
  mBins[3] = 2000;
  mBins[4] = 2500;
  mBins[5] = 3000;
  mBins[6] = 3500;
  mBins[7] = 4000;
  mBins[8] = 4500;
  mBins[9] = 5000;

  
}


void loop()
{
 
  
    // Listen for  Message    

    if (nrf24.available())
  {
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got request: ");
      Serial.println((char*)buf);
      if(mRespond <= 13){
        mUnknownHash[mRespond] = (char*)buf; 
      }
      
      
      if(mRespond == 13 ){
        delay(1000);
        sendNonce(); 
        }
        if(mRespond == 14 ){
          mUnknownNonce[0] = (char*)buf;  
          //Serial.print("FINISH");
        finishProtocol();
        }
        Serial.print(mRespond);
        mRespond++; 
      
      /*
      mEnemyHash = (char*)buf; 
      Serial.print("");  
         
      //Send nonce back 
      
      itoa(mRandomNum, mResponse, 10); 
      Serial.print(""); 
      unsigned char t1 = mResponse[0]; 
      unsigned char t2 = mResponse[1]; 
      unsigned char t3 = mResponse[2]; 
      unsigned char t4 = mResponse[3]; 
      uint8_t data[5] = {t1, t2, t3, t4};
      Serial.print("");
      //nrf24.send(data, sizeof(data));
      //nrf24.waitPacketSent();
     // Serial.println("Sent a reply");
      Serial.println("");

      
      mEnd++; 
     // if(mEnd == 2){
       // finishProtocol(); 
       // }
        */
    }
   
    else
    {
      Serial.println("recv failed");
    }
    }
  else{


  
 //When the button is pressed 

  if(mStartCapture == 1){
    mCaptureTime = millis(); 
    }
  else{
    if(mStartCapture > 1){
      mUpdateTime = millis();  
      if((mUpdateTime - mCaptureTime) >= 20000){
      
        if(mStop){
          Serial.println("STOP PRESSING");
        if(mArrayCounter < 20){
          for(int i = mArrayCounter; i < 20; i++){
            mOriginalDurations[i] = 0; 
            }
          }
        calculateVector(); 
        Serial.print("Vector is ");
        for(int i = 0; i < 10; i++){
          Serial.print(i);
          Serial.println(mFinalVector[i]);
          }
         delay(500);
        sendVector();
        mStop = false;
          }
        }
        else{
          Serial.print("Elapsed Time is"); 
          Serial.println(mUpdateTime - mCaptureTime);
          }
    }
    }
  
  if(digitalRead(mButtonPin) == HIGH){
    mStartCapture++; 
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
    mElapsedTime = roundDown(mElapsedTime); 
    Serial.print("Rounded time is ");
    Serial.println(mElapsedTime);
    mOriginalDurations[mArrayCounter] = mElapsedTime; 
    mArrayCounter++; 
    
   
       
    
  
    
    }
    mStartTime = 0; 
  }
}
}

void finishProtocol(){
  Serial.println("REACHED END");
  Serial.println("Unknown Hash is ");
  String vUnknownHash = ""; 
  for(int i = 0; i < 14; i++){
    Serial.print(i);
    Serial.println(mUnknownHash[i]);
    vUnknownHash = vUnknownHash + mUnknownHash[i]; 
    }

    //mUnknownNonce[0].remove(2, 4); 
   Serial.println("Unknown Nonce is ");
   Serial.print(mUnknownNonce[0]);

  String vCalc = ""; 
  for(int i = 0; i < 10; i++){
    vCalc = vCalc + String(mFinalVector[i]); 
    }
    vCalc = vCalc + mUnknownNonce[0]; 
    Serial.print("h3 input is ");
    Serial.println(vCalc);
    char vTemp[vCalc.length()];
    vCalc.toCharArray(vTemp, vCalc.length() );
    //itoa(mElapsedTime, mTemp, 10);
    mHasher.clear(); 
    mHasher.reset(); 
    mHasher.update(vTemp, vCalc.length()); 
    mHasher.finalize(mHash2, vCalc.length());

    String vCalculatedHash = ""; 
    Serial.println("Calculated hash is ");
    for(int i = 0; i < 14; i++){
    Serial.print(i);
    Serial.println(mHash2[i]);
    vCalculatedHash = vCalculatedHash + mHash2[i]; 
    }

    if(vCalculatedHash.equals(vUnknownHash)){
      Serial.println("PAIRING SUCCESS");
      }
    else{
      Serial.println("PAIRING FAILURE");
      }
}

unsigned long roundDown(unsigned long X){
  if(X > 0 && X<1000){return 500;}
  if(X >=1000  && X<1500){return 1000;}
  if(X >= 1500 && X<2000){return 1500;}
  if(X >= 2000 && X<2500){return 2000;}
  if(X >= 2500 && X<3000){return 2500;}
  if(X >= 3000 && X<3500){return 3000;}
  if(X >= 3500 && X<4000){return 3500;}
  if(X >= 4000 && X<4500){return 4000;}
  if(X >= 4500 && X<5000){return 4500;}
  if(X >= 5000){return 5000;}
  else{return 0;}
  }

void calculateVector(){
  unsigned long vBin; 
  for(int i = 0; i < 10; i++){
    vBin = mBins[i];
    unsigned long vCounter = 0;
    for(int j = 0; j < 20; j++){
      if(vBin == mOriginalDurations[j]){
        vCounter++;
        }
      }
      mFinalVector[i] = vCounter; 
    }
  }

void sendVector(){
     //calculate hash
     String vVectorString = "";  
     for(int i = 0; i < 10; i++){
      vVectorString = vVectorString + String(mFinalVector[i]);
      } 
    vVectorString = vVectorString + String(mRandomNum);
    Serial.print("h1 input is ");
    Serial.println(vVectorString);
    char vTemp[vVectorString.length()];
    vVectorString.toCharArray(vTemp, vVectorString.length() );
    //itoa(mElapsedTime, mTemp, 10);
    mHasher.clear(); 
    mHasher.reset(); 
    mHasher.update(vTemp, vVectorString.length()); 
    mHasher.finalize(mHash, vVectorString.length());
    Serial.print("Calculated Hash ");
    Serial.print("");
    for(int i = 0; i < vVectorString.length(); i++){
      Serial.print(i);
      Serial.println(mHash[i]);
      }
  



    //send hash 

    for(int i = 0; i < vVectorString.length(); i++ ){
    char vSender[2]; 
    ltoa(mHash[i],vSender,10);
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
      
     
      }
      else
      {
      Serial.println("recv failed");
      }
    }
    else
    {
    //Serial.println("No reply, is nrf24_server running?");
    }
    }
  }

  void sendNonce(){
   Serial.println("SENDING NONCE");
    char vSender[4]; 
    
    ltoa(mRandomNum,vSender,10);
    unsigned char a =  vSender[0]; 
    unsigned char b = vSender[1]; 
    unsigned char c = vSender[2]; 
    unsigned char d = vSender[3]; 
    uint8_t data[5] = {a, b, c, d}; 
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();


    //get a response 
    /*
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.waitAvailableTimeout(500))
    {   
      if (nrf24.recv(buf, &len))
      {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.println("");
      //mEnemyRandom = (char*)buf; 
      
     
      }
      else
      {
      Serial.println("recv failed");
      }
    }
    else
    {
    //Serial.println("No reply, is nrf24_server running?");
    }
    */
    }
