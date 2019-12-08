// PWM pins pair MSB=9 LSB=10, together 16bit PWM :-)
  lkpcm.definePWMPins(9, 10); 
  
  Serial.begin(115200);
  pinMode(13,OUTPUT); //LED Connected to analog pin 0
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println(F("SD fail _ _ _"));  
    return;   // don't do anything more if not

  }
  else{   
    Serial.println(F("SD ok"));   
  }
  lkpcm.play("musik32.wav"); //the sound file "music" will play each time the arduino powers up, or is reset 
