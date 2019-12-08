
#include <LKpcm_UserConfig.h>


#include <SD.h>              // need to include the SD library
#define SD_ChipSelectPin 4       
#include <Arduino.h>



#include <LKpcm.h>          
#include <SPI.h>


LKpcm lkpcm;   // create an object for use in this sketch

unsigned long time = 0;


// define what suffix all PCM/WAV files need to have on the SD-card.
// relevant for "normal" PCM files as well as message-files.
const char SUFFIX_PCM_FILES[5] = ".WAV";

// where on SDcard are all our files stored?
// we use hard-fixed "/", no user-definable variable!

// define how the subdir is named, that holds system-message-pcm-files
// relevant for "normal" PCM files as well as message-files.
const char SDC_SYSTEM_MESSAGES_DIR[7] = "SYSTEM";


void setup(){

    Serial.begin(9600);

  	// PWM pins pair MSB=9 LSB=10, together 16bit PWM :-)
    lkpcm.setupPlayer(9, 10); 


    pinMode(13,OUTPUT); //LED Connected to analog pin 0
    if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
        Serial.println(F("SD fail _ _ _"));  
        return;   // don't do anything more if not

    }
    else{   
        Serial.println(F("SD ok"));   
    }
    lkpcm.play("/1/1.WAV"); 

 
}






void loop(){  
  if( !lkpcm_isPlaying ) {      
          
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
    case '?': if(lkpcm_isPlaying){ Serial.println(F("A wav file is being played"));} break;
    case 'S': lkpcm.stopPlayback(); time = millis(); Serial.println(F("STOPPED manually"));  break;
    case '+': lkpcm.volume(1); break;
    case '-': lkpcm.volume(0); break;
    //sijsch debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
    //case 'm': lkpcm.sendRecordedMicros(); break;
    default: break;
    }
  }

}
