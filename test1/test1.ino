
#define SERIAL_RX_BUFFER_SIZE 64
#define SERIAL_TX_BUFFER_SIZE 64


#include <LKpcmConfig.h>


#include <SD.h>              // need to include the SD library
#define SD_ChipSelectPin 4    
// pin 14 == A0    
#include <Arduino.h>



#include <LKpcm_atmega328_timer1.h>          
#include <SPI.h>

//#include <SdFat.h>


LKpcm lkpcm;   // create an object for use in this sketch

unsigned long time = 0;

void setup(){

	// PWM pins pair MSB=9 LSB=10, together 16bit PWM :-)
  lkpcm.definePWMPins(9, 10); 
  
  Serial.begin(9600);
  pinMode(13,OUTPUT); //LED Connected to analog pin 0
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println(F("SD fail _ _ _"));  
    return;   // don't do anything more if not

  }
  else{   
    Serial.println(F("SD ok"));   
  }
  lkpcm.play("/1/1.WAV"); //the sound file "music" will play each time the arduino powers up, or is reset 

 
}






void loop(){  
  if( !isPlaying() ) {      
          
      //lkpcm.stopPlayback();
      //Serial.println(F("STOPPED"));  
      //time = millis();   
  }
  

  
  

  if(Serial.available()){    
    switch(Serial.read()){
    case 'q': lkpcm.play("musik16.wav"); Serial.println(F("musik16 is being played"));   break;
    case 'w': lkpcm.play("/1/2.WAV"); Serial.println(F("2 is being played"));   break;
    case 'e': lkpcm.play("/1/3.WAV"); Serial.println(F("3 is being played"));   break;
    case 'r': lkpcm.play("/1/4.WAV"); Serial.println(F("4 is being played"));   break;
    case 't': lkpcm.play("/1/5.WAV"); Serial.println(F("5 is being played"));  break;
    case 'p': lkpcm.pause(); break;
    case '?': if(isPlaying()){ Serial.println(F("A wav file is being played"));} break;
    case 'S': lkpcm.stopPlayback(); time = millis(); Serial.println(F("STOPPED manually"));  break;
    case '+': lkpcm.volume(1); break;
    case '-': lkpcm.volume(0); break;
    //sijsch debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
    //case 'm': lkpcm.sendRecordedMicros(); break;
    default: break;
    }
  }

}
