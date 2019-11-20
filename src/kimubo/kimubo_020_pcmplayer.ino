/* 
 * KIMUBO
 * KInderMUsikBOx
 * 
 * Ein minimalistischer WAV/PCM Spieler für Kinder - und mehr.
 * 
 * (c) 2019 ludgerknorps <56650955+ludgerknorps@users.noreply.github.com>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. See file "LICENSE". 
 *   If not, see <https://www.gnu.org/licenses/>.
 * 
 */


// ####################################################################################
bool isPaused(){
	return bitRead(playerOptions,7);
}

// ####################################################################################
bool isStopped(){
	return bitRead(playerOptions,6);
}

// ####################################################################################
bool isInterupted(){
	return bitRead(playerOptions,7);
}

// ####################################################################################
bool isPlaying(){
	return bitRead(playerOptions,4);
}

// ####################################################################################
bool isTwoByteSamples() {
	return bitRead(playerOptions,2);
}

// ####################################################################################
bool is16bit() {
	return bitRead(playerOptions,1);
}

// ####################################################################################
void setPaused(bool bParam) {
	bitWrite(playerOptions,7,bParam);
}

// ####################################################################################
void setStopped(bool bParam) {
	bitWrite(playerOptions,6,bParam);
}

// ####################################################################################
void setInterupted(bool bParam) {
	bitWrite(playerOptions,5,bParam);
}

// ####################################################################################
void setPlaying(bool bParam) {
	bitWrite(playerOptions,4,bParam);
}

// ####################################################################################
void setTwoByteSamples(bool bParam) {
	bitWrite(playerOptions,2,bParam);
}

// ####################################################################################
void set16bit(bool bParam) {
	bitWrite(playerOptions,1,bParam);
}





// ####################################################################################
void player_setupPWMPins(){
	
	// configure both PWM-output pins
	pinMode(AUDIO_PIN_OUT_HIGH,OUTPUT);
	pinMode(AUDIO_PIN_OUT_LOW,OUTPUT);

}


// ####################################################################################
void player_setupTimers(){
	
	// setup TimerCounter1 for pwm output of pcm data
	
	//ludgerknorps ISR-debug
	#if defined (ISR_BUFFER_DEBUG_PIN)
		pinMode(ISR_BUFFER_DEBUG_PIN,OUTPUT);
		setPinD0_D21(ISR_BUFFER_DEBUG_PIN);
	#endif
	#if defined (ISR_PCMFEED_DEBUG_PIN)
		pinMode(ISR_PCMFEED_DEBUG_PIN,OUTPUT);
		setPinD0_D21(ISR_PCMFEED_DEBUG_PIN);
	#endif
	
	//Reset Timer/Counter Control Register A and B
	TCCR1A = 0;
	TCCR1B = 0;
	
	//Modus Phase+FrequCorrect PWM-Mode TOP = ICR1 (Mode 8), see [ATMEGA328 datasheet DS40002061A-page 141]
	TCCR1A &= ~_BV(WGM10);
	TCCR1A &= ~_BV(WGM11);
	TCCR1B &= ~_BV(WGM12);
	TCCR1B |=  _BV(WGM13);
	
	// TOP = ICR1 is set later by metadata read from PCM-file...
	//~ ICR1 = ...;	
	
	//Timer1 prescaler: set to 1
	TCCR1B &= ~_BV(CS12);
	TCCR1B &= ~_BV(CS11);
	TCCR1B |=  _BV(CS10);

	//noninverting PWM-Signal for channels A and B
	TCCR1A |=  _BV(COM1A1);
	TCCR1A &= ~_BV(COM1A0);
	TCCR1A |=  _BV(COM1B1);
	TCCR1A &= ~_BV(COM1B0);

}




// ####################################################################################
bool player_checkWAVFile(char* filename){
	
	unsigned int i;

	//check for the string WAVE starting at 8th bit in header of file
	#if defined (SDFAT)
		sdc_File.open(filename);
	#else
		sdc_File = SD.open(filename);
	#endif

	#if defined (debug)
		Serial.print(F("FILENAME: "));
		Serial.println(filename);
	#endif

	// first check: can I open the file at all?
	if( ! player_isFileOpen() ){ 
		return 0;
	}
	seek(8);
	
	// second check: correct WAVE identifier?
	char wavIdentifier[] = {'W','A','V','E'};
	for (i = 0; i < 4; i++){
		if(sdc_File.read() != wavIdentifier[i]){
			#if defined (debug)
				Serial.println(F("WAV ERROR INCORRECT IDENTIFIER"));
			#endif
			return 0;
		}
	}

	// third check: file must be mono (no stereo allowed!)
	seek(22);
	if (sdc_File.read() != '1'){
		#if defined (debug)
			Serial.println(F("WAV ERROR STEREO"));
		#endif
		return 0;
	}
	
	// fourth check: sample rate must match user defined value, see header file...
	// we need to read two bytes, as sample rate is type unsigned integer
	seek(24);
	i = sdc_File.read()	| ( sdc_File.read() << 8 ); // first byte read is the LSB, second byte is the HSB
	if ( i != PCM_SAMPLE_RATE ){
		#if defined (debug)
			Serial.print(F("WAV ERROR SAMPLE_RATE = "));
			Serial.println(i);
		#endif
		return 0;
	}
	
	// fith check: bits per sample must be 8 or 16	
	// we need to read two bytes, as bitspersample is type unsigned integer in WAV file
	seek(34);
	i = sdc_File.read() | ( sdc_File.read() << 8 ); // first byte read is the LSB, second byte is the HSB
	if (i == 16){
		set16bit(true);
		setTwoByteSamples(true);
	}else if (i == 8){ 
		set16bit(false);
		setTwoByteSamples(false);
	} else {
		#if defined (debug)
			Serial.print(F("WAV ERROR BPS = "));
			Serial.println(i);
		#endif
		return 0;
	}

	// ok, if we came to here, file seems to be in order...	
	seek(44); // goto start of PCM-data in WAV file
	return 1;

}


// ####################################################################################
void player_stop(){
	setPlaying(false);
	// stop overrides paused or interupted
	setStopped(true);
	setInterupted(false);
	setPaused(false);
	DISABLE_PCM_FEED_INTERUPT; // do that first before deactivating BufferInterupt
	DISABLE_BUFFER_INTERUPT;

	if (player_isFileOpen()){ 
		sdc_File.close(); 
	}

}

// ####################################################################################
void player_pause(){
	// pause only affects player if it is not stopped and not interupted
	if (!isStopped() && !isInterupted()){
	
		if (isPaused() ){
			setPaused(false);
			setPlaying(true);
			ENABLE_BUFFER_INTERUPT;
			ENABLE_PCM_FEED_INTERUPT; // do that after activating BufferInterupt
			
		}else if (!isPaused() ){
			setPaused(true);
			setPlaying(false);
			DISABLE_PCM_FEED_INTERUPT;
		}
	}
}

/*
 * translation of functions between SDFAT library and regular SD library
*/

#if defined (SDFAT)
	// ####################################################################################
	bool player_seek(unsigned long pos ){
		return sdc_File.seekSet(pos);
	}
	// ####################################################################################
	bool player_isFileOpen(){
		return sdc_File.isOpen();
	}
	// ####################################################################################
	unsigned long player_fPosition(){
		return sdc_File.curPosition();
	}
#else
	// ####################################################################################
	bool player_seek( unsigned long pos ){
		return sdc_File.seek(pos);
	}
	// ####################################################################################
	bool player_isFileOpen(){
		if(sdc_File){ return 1;}
	}
	// ####################################################################################
	unsigned long player_fPosition( ){
		return sdc_File.position();
	}
#endif



// ####################################################################################
void player_play(char* filename){
	player_play(filename,0);
}


// ####################################################################################
void player_play(char* filename, unsigned long seekPoint){

	// start with clean settings...
	setPWMPins();
	player_stop();
  
	// check that WAV file is valid and PCM data in it is correctly encoded
	if(!player_checkWAVFile(filename)){
		#if defined (debug)
			Serial.println(F("WAV ERROR player_checkWAVFile"));
		#endif
		return;
	}

	// after player_checkWAVFile the player_fPosition() currently is 44, so take that into account when calculating seek position
	if(seekPoint > 0){ 
		seekPoint = (SAMPLE_RATE*seekPoint) + player_fPosition();
		seek(seekPoint);
	}
	
	setStopped(false); 
	setInterupted(false);
	setPaused(false);

    // We generate dual PWM using Mode 8 (see ATMEGA328 datasheet DS40002061A-page 141).
    // As such the sample rate (defined by ICR1 setting) is calculated as [see DS40002061A-page 137]
    //		SampleRate = f_clockI-O / (2*N*TOP)
    // In our case 
	// 			f_clockI-O is the f_CPU and 
	// 			N (the prescaler for timer/counter1) is set to 1 and
	//			TOP is defined by ICR1
    // Thus: 
	//		SampleRate = f_CPU / (2*ICR1)
	// From there our ICR1 setting is derived as
	// 		ICR1 = f_CPU / (2*SampleRate)	
	ICR1 = F_CPU / 2 / SAMPLE_RATE;
	
	#if defined (debug)
		Serial.print(F("TimerCount1 max-value: "));
		Serial.println(ICR1);
	#endif

    byte tmp = (sdc_File.read() + sdc_File.peek()) / 2;


	//rampUp = 0;
	unsigned int mod;
	byte theBuffer;
	
	if(player_SWvolume > 0){ mod = OCR1A >> player_SWvolume; }else{ mod = OCR1A << (player_SWvolume*-1); }
	if(tmp > mod){
		// fill all buffers
		for(theBuffer = 0; theBuffer < NUMBER_OF_PCM_BUFFERS; theBuffer++){
			for(unsigned int i=0; i<PCM_BUFFER_SIZE; i++){ 
				mod = constrain(mod+1,mod, tmp); 
				buffer[theBuffer][i] = mod; 
			}
		}
	}else{
		// fill all buffers
		for(theBuffer = 0; theBuffer < NUMBER_OF_PCM_BUFFERS; theBuffer++){
			for(unsigned int i=0; i<PCM_BUFFER_SIZE; i++){ 
				mod = constrain(mod-1,tmp ,mod); 
				buffer[0][i] = mod; 
			}
		}
	}

	// set "i-am-not-free"-flags for all buffers
    for(theBuffer = 0; theBuffer < NUMBER_OF_PCM_BUFFERS; theBuffer++){
		buffEmpty[theBuffer] = false;
    }
    currentReadBuffer = 0; // well, we start reading at the beginning...
    currentWriteBuffer = 0; // now all buffers are full, so we wait for the first one to become empty and then refill it via our interupt, see below
    buffCount = 0;
    noInterrupts();
	player_setupTimers();

	ENABLE_BUFFER_INTERUPT;
    ENABLE_PCM_FEED_INTERUPT; // do that after activating BufferInterupt

    interrupts();

}


// ####################################################################################
void LKpcm::setVolume(byte vol) {
	// transform vol ( which is 0..8 ) into internal volume (which is -4..4)
	if (vol <= 8){
		player_SWvolume = vol - 4 ;
	}
}

// ####################################################################################
//ludgerknorps debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
//~ void LKpcm::sendRecordedMicros(){
  //~ byte i = 0;
  //~ Serial.println(myMicrosCounter);
  
  //~ if (myMicrosCounter >= 10) {
    //~ // aufgenommene myMicros via Serial ausgeben
    //~ Serial.print(i);
    //~ Serial.print(F(":"));
    //~ Serial.println(myMicros[i]);
      
    //~ for (i=1; i< 10; i++){
      //~ Serial.print(i);
      //~ Serial.print(F(":"));
      //~ Serial.println( myMicros[i] - myMicros[i-1] );
    //~ }
    //~ // aufgenommene myMicros löschen
    //~ //for (i=0; i< 10; i++){
    //~ //  myMicros[i] = 0;
    //~ //}
    //~ // klarmachen für neue myMicros - Aufnahme
    //~ myMicrosCounter = 0;
  //~ }
//~ }






// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/*
 * INTERUPTS
 */

/*
 * We use two interupts to feed the PCM data from the SD-card to the two pins that form the DA-converter:
 * 	a) the TIMER1_CAPT_vect - called "BufferInterupt" - for loading data from SDC into a buffer (in RAM) and
 *  b) the TIMER1_OVF_vect - called "PCMFeedInterupt" - for feeding data from the buffer to the DAC
 * 
 * The PCMFeedInterupt is the time-critical thing, as we need to feed data every 1/sample-rate seconds.
 * 	# the TIMER1_OVF_vect is used and is configured to the correct frequency by setting the ICR1 register
 * 		which in turn defines when the timer/counter1 will "roll over" thus setting the periode for the output-waveform.
 * 	# every call of TIMER1_OVF_vect writes the next two bytes (16bit!) into the two OCR1A and OCR1B registers.
 * 	# these define the generated waveform (dutycyle) at the two pins OC1A and OC1B (pins 9/10 of the Arduino). 
 * 	# This waveform is apparent at the pins for the next cycle until the next call of TIMER1_OVF_vect.
 * 
 * The PCMFeedInterupt reads PCM-data from the buffer. The buffer needs to be refilled with data from the SD-card.
 * This is done using a second interupt, the TIMER1_CAPT_vect.
 * 	# Important: it must be possible for TIMER1_OVF_vect to interupt TIMER1_CAPT_vect: the output to the pins is the timecritical thing.
 * 	# Each call to TIMER1_CAPT_vect checks whether the buffer needs refilling.
 * 	# if so, the buffer is filled from SD-card.
 * 	# also, if there is no more data to read from the SD-card, then the playback is stopped.
 * 
 * 	# TIMER1_CAPT_vect is an "input capture" interupt - but we do not caputre any inputs (at the relevant pin ICP1 and neither from the analog comparator for timer/counter1).
 * 	# But there is an additional possibility to call TIMER1_CAPT_vect 
 * 		as described in the ATMEGA328P datasheet (DS40002061A / page 145 / chapter 16.11.9 'TIFR1 – Timer/Counter1 Interrupt Flag Register':
 * 			"When the Input Capture Register (ICR1) is set by the WGM13:0 to be used as the TOP value, 
 * 			 the ICF1 Flag is set when the counter reaches the TOP value."
 * 		As we use this mode (ICR1 defines the TOP value) this is our interupt trigger-source.
 */

// ####################################################################################
// ####################################################################################
// ####################################################################################

// the Buffer-interupt
ISR(TIMER1_CAPT_vect){

	// The first step is to disable this interrupt before manually enabling global interrupts.
	// This allows this interrupt vector to continue loading data while allowing the overflow interrupt
	// to interrupt it. ( Nested Interrupts )
	//	DISABLE_BUFFER_INTERUPT
	//Then enable global interupts before this interrupt is finished, so the music can interrupt the buffering
	//	sei();

	if(buffEmpty[currentWriteBuffer]){
		
		//ludgerknorps ISR-debug
		#if defined (ISR_BUFFER_DEBUG_PIN)
			//~ if (currentWriteBuffer == 0  ) { 
				clrPinD0_D21(ISR_BUFFER_DEBUG_PIN);
			//~ }
		#endif

		// because of performance, no helper-function is used inside ISR.
		// we use predefined function-macros instead.	
		DISABLE_BUFFER_INTERUPT;
		
		sei();

		// close file, if less then one buffer is left in file
		if( sdc_File.available() < PCM_BUFFER_SIZE){
		  	// because of performance, no helper-function is used inside ISR.
			// we use predefined function-macros instead.
			DISABLE_PCM_FEED_INTERUPT;			 
		  	bitWrite(playerOptions,4,false); 	// setPlaying(false);
	  	} else {
			sdc_File.read( (byte*)buffer[currentWriteBuffer] , PCM_BUFFER_SIZE );
			buffEmpty[currentWriteBuffer] = false;
			// goto next buffer (or "roll over" to first buffer, if we were at the last one
			if (currentWriteBuffer < NUMBER_OF_PCM_BUFFERS - 1 ) { 
				currentWriteBuffer++ ;
			} else { 
				currentWriteBuffer = 0;
			}
			// because of performance, no helper-function is used inside ISR.
			// we use predefined function-macros instead.
			ENABLE_BUFFER_INTERUPT;			
			
		}
		
		
		//ludgerknorps ISR-debug
		#if defined (ISR_BUFFER_DEBUG_PIN)
			setPinD0_D21(ISR_BUFFER_DEBUG_PIN);
		#endif
		
		
   	}
   	
}


// ####################################################################################
// ####################################################################################
// ####################################################################################
// the PCMFeedInterupt
ISR(TIMER1_OVF_vect){
	//ludgerknorps ISR-debug
	#if defined (ISR_PCMFEED_DEBUG_PIN)
		clrPinD0_D21(ISR_PCMFEED_DEBUG_PIN);
	#endif

	if( !is16bit() ){
		if(player_SWvolume < 0 ){  
			OCR1A = OCR1B = buffer[currentReadBuffer][buffCount] >> (player_SWvolume*-1);
	  	}else{ 
			OCR1A = OCR1B = buffer[currentReadBuffer][buffCount] << player_SWvolume;
	  	}
	  	++buffCount;

	}else{
		// is 16bit, so we have to correct for unsigned data in the pcm-samples
		// see https://en.wikipedia.org/wiki/WAV or https://stackoverflow.com/questions/10731226/how-to-determine-if-8bit-wav-file-is-signed-or-unsigned-using-java-and-without
		// 	<=8 bit WAV files are always unsigned, >=9 bit are always signed
		// we do the conversion here and not in the Buffer-interupt, 
		// as there it uses the library-function sdcFile.read(buffer) 
		// which simply bitbangs the data from SD card to buffer.
		// So the buffer holds signed data in byte type (unsigned) variables.
		// we need to correct for that:
		// buffervalue 	= B00000000 has to be interpreted as "pcm amplitude is middle of range" --> 127
		//				= B11111111 has to be interpreted as "pcm amplitude is middle of range -1" --> 126
		//				= B10000000 has to be interpreted as "pcm amplitude is low end of range " --> 0
		//				= B01111111 has to be interpreted as "pcm amplitude is upper end of range" --> 255
		// in short: we need to add 2^8-1 = 127
		if(is16bit()){
			buffer[currentReadBuffer][buffCount] += 127;
		}
		
		if(player_SWvolume < 0 ){ OCR1A = buffer[currentReadBuffer][buffCount] >> (player_SWvolume*-1);
						 OCR1B = buffer[currentReadBuffer][buffCount+1] >> (player_SWvolume*-1);

		}else{    		 OCR1A = buffer[currentReadBuffer][buffCount] << player_SWvolume;
						 OCR1B = buffer[currentReadBuffer][buffCount+1] << player_SWvolume;
		}
  		
  		buffCount+=2;
  	}

  	if(buffCount >= PCM_BUFFER_SIZE){
		buffCount = 0;
		buffEmpty[currentReadBuffer] = true;
		// goto next buffer (or "roll over" to first buffer, if we were at the last one
		if (currentReadBuffer < NUMBER_OF_PCM_BUFFERS - 1 ) { 
			currentReadBuffer++ ;
		} else { 
			currentReadBuffer = 0;
		}
  	}
  	
  	//ludgerknorps debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
  	//~ if (myMicrosCounter < 10) {
		//~ myMicros[myMicrosCounter] = micros();
		//~ myMicrosCounter++;
	//~ }
	
	//ludgerknorps ISR-debug
	#if defined (ISR_PCMFEED_DEBUG_PIN)
		setPinD0_D21(ISR_PCMFEED_DEBUG_PIN);
	#endif
  	
}











 
