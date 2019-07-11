// nrf24_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_NRF24 class. RH_NRF24 class does not provide for addressing or
// reliability, so you should only use RH_NRF24 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example nrf24_server.
// Tested on Uno with Sparkfun NRF25L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <SPI.h>
#include <RH_NRF24.h>
#include <Hash.h>
// Singleton instance of the radio driver
RH_NRF24 nrf24;

// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini



int ledPin = 4;                // choose the pin for the LED
int buttonPin = 2; 
String message; 
unsigned long StartTime = 0;  
boolean print = false; 
char temp[4]; 
void setup() 
{
  Serial.begin(9600);
  Serial.println("Begin");
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
  char storage[3]; 
  ltoa(ElapsedTime,storage,10);
   unsigned char a = storage[0]; 
   unsigned char b = storage[1];  
   unsigned char c = storage[2]; 
   unsigned char d = storage[3]; 
  uint8_t data[5] = {a, b, c, d};  
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
