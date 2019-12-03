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


bool player_setup(){

      #if defined (debug)
          Serial.println(F("player_setup INFO started..."));
      #endif

      // call all relevant sub-setup-functions.
      // the bool-return is fake and just kept in order to be compatible with overall setup (done in fsmMain)
      player_setupPWMPins();
      player_setupTimers();


      // set our internal "remember" variables:  
          
      // create our "current playlist" string
      player_current_playlist_dirname = ' ';  // default, has no meaning
      // dito for "current track" string
      player_current_track = 255; // default
      get_new_track_player_filename(); // make filename from current_track_number

      #if defined (debug)
          Serial.println(F("player_setup INFO finished."));
      #endif

      return true; // nothing else to report here...
}

// ####################################################################################
// calculates the new track's filename
void get_new_track_player_filename(){

    #if defined (debug)
        Serial.print(F("get_new_track_player_filename "));
        Serial.print(player_current_track);
        Serial.print(F(" : "));
        Serial.print(sizeof(player_current_track_filename));
        delay(100);
    #endif
    

    char temp_filename[sizeof(player_current_track_filename)];
    

    if ( player_current_track == 255 ){
        strcpy(player_current_track_filename, " "); // default
    } else {
        itoa(player_current_track, temp_filename, 10); // 10 = convert using decimal system
        #if defined (debug)
            Serial.print(F(" : "));
            Serial.print(temp_filename);
            delay(100);
        #endif
        
        strcat(temp_filename, SUFFIX_PCM_FILES);

        #if defined (debug)
            Serial.print(F(" : "));
            Serial.print(temp_filename);
            Serial.print(F(" : "));
            Serial.print(player_current_playlist_dirname);
            delay(100);
        #endif

        // a short array to hold the playlist dirname 
        char s[4] = "/ /"; // initialize 
        s[1] = player_current_playlist_dirname; // overwrite
        
        strcpy(player_current_track_filename, s);
        strcat(player_current_track_filename, temp_filename);

        #if defined (debug)
            Serial.print(F(" : "));
            Serial.print(player_current_track_filename);
            delay(100);
        #endif
    }

    #if defined (debug)
        Serial.println();
        delay(100);
    #endif
  
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

  // in order to start with a clean setup we diable both configured ISR at first
  DISABLE_BUFFER_INTERUPT;
  DISABLE_PCM_FEED_INTERUPT;
	
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
	// Sample rate is fixed to user defined PCM_SAMPLE_RATE, thus it is sufficient to do this setup once here...
	ICR1 = F_CPU / 2 / PCM_SAMPLE_RATE;
 
  // just to be absolutely sure, we again disable both configured ISR 
  DISABLE_BUFFER_INTERUPT;
  DISABLE_PCM_FEED_INTERUPT;
  
}


// ####################################################################################
void player_setupBuffers(){

	// set "i-am-free"-flags for all buffers
    for(byte theBuffer = 0; theBuffer < NUMBER_OF_PCM_BUFFERS; theBuffer++){
		  player_bufferEmpty[theBuffer] = true;
    }
    
    // start with a defined buffer...
    player_currentWriteBuffer = 0;
    player_currentReadBuffer = 0;
}



// ####################################################################################
bool player_openFile(char* filename){
	
	// deactivate ISRs
	// close currently open file
	// reset buffers to defined starting conditions	
	player_stop();
	
	
	// check that WAV file is valid and PCM data in it is correctly encoded
	if( ! player_checkWAVFile(filename)){
		#if defined (debug)
			Serial.println(F("WAV ERROR player_checkWAVFile"));
		#endif
		return false;
	}

  return true;
 
}



// ####################################################################################
bool player_checkWAVFile(char* filename){
	
	unsigned int i;
  FRESULT result;

	//check for the string WAVE starting at 8th bit in header of file

  result = pf_open(filename);

  switch ( result ) {

      case FR_OK:
          // do nothing
          break;
      case FR_NO_FILE:
          #if defined (debug)
              Serial.print(F("WAV ERROR FR_NO_FILE "));
              Serial.println(filename);
          #endif
          return 0;
      case FR_DISK_ERR:
          #if defined (debug)
              Serial.print(F("WAV ERROR FR_DISK_ERR "));
              Serial.println(filename);
          #endif
          return 0;
      case FR_NOT_ENABLED:
          #if defined (debug)
              Serial.print(F("WAV ERROR FR_NOT_ENABLED "));
              Serial.println(filename);
          #endif
          return 0;
  }

	#if defined (debug)
		Serial.print(F("FILENAME: "));
		Serial.println(filename);
	#endif

  // read one first block into our buffer.
  // we have to check on the metadata - which is in the first 44 bytes of the file / of the buffer.
  // but afterwards we want to continue reading block-wise from the SDC (and an SDC-block is 512 bytes).
  // so it would be not-ideal to read 44 bytes manually and then read 512bytes (or so) each.
  // As every of these 512bytes would spa across more than one SDC-block.

  // so...
  // fill one buffer with data from sdc, other buffers will be filled by ISR "BufferInterupt"
  // because of player_setupBuffers() read and write buffers are currently set to be 0.
  // block-read from SDcard
  pf_read( (byte*)player_buffer[player_currentWriteBuffer] , PCM_BUFFER_SIZE, &player_bytesFromSDC );

  // if buffer is not fully filled (i.e. pf_read reached EOF) then playing will be stopped by ISR

  if ( player_bytesFromSDC < 44 ) {
      #if defined (debug)
        Serial.println(F("WAV ERROR File too small (for metadata)"));
      #endif
      return 0;
  }
  
  //player_bufferEmpty[player_currentWriteBuffer] = false;
  // goto next buffer (or "roll over" to first buffer, if we were at the last one
  // should not be necessary here because of clean starting setup for buffers at player_openFile()...
  if (player_currentWriteBuffer < NUMBER_OF_PCM_BUFFERS - 1 ) { 
    ++player_currentWriteBuffer ;
  } else { 
    player_currentWriteBuffer = 0;
  }
  
  // now we can use the data from this buffer to check the metadata
  // as state above: because of player_setupBuffers() the player_currentReadBuffer is also pointing to the buffer 0 which is the one we used here...
	
	// seek position 8
	// second check: correct WAVE identifier?
	char wavIdentifier[] = {'W','A','V','E'};
	for (i = 0; i < 4; i++){
		if( player_buffer[player_currentReadBuffer][8 + i] != wavIdentifier[i]){
			#if defined (debug)
				Serial.println(F("WAV ERROR INCORRECT IDENTIFIER"));
			#endif
			return 0;
		}
	}

//	// third check: file must be mono (no stereo allowed!)
//	// seek position 22
//	if ( player_buffer[player_currentReadBuffer][22] != '1'){
//		#if defined (debug)
//			Serial.print(F("WAV ERROR STEREO " ));
//      //Serial.print( player_buffer[player_currentReadBuffer][20] );
//      //Serial.print( player_buffer[player_currentReadBuffer][21] );
//      Serial.println( player_buffer[player_currentReadBuffer][22] );
//      //Serial.print( player_buffer[player_currentReadBuffer][23] );
//      //Serial.print( player_buffer[player_currentReadBuffer][24] );
//      delay(100);
//		#endif
//		return 0;
//	}
	
//	// fourth check: sample rate must match user defined value, see header file...
//	// we need to read two bytes, as sample rate is type unsigned integer
//	// seek position 24
//	i = player_buffer[player_currentReadBuffer][24]	| ( player_buffer[player_currentReadBuffer][25] << 8 ); // first byte read is the LSB, second byte is the HSB
//	if ( i != PCM_SAMPLE_RATE ){
//		#if defined (debug)
//			Serial.print(F("WAV ERROR SAMPLE_RATE = "));
//			Serial.println(i);
//		#endif
//		return 0;
//	}
	
	// fifth check: bits per sample must be 8 or 16	
	// we need to read two bytes, as bitspersample is type unsigned integer in WAV file
	// seek position 34
	i = player_buffer[player_currentReadBuffer][34] | ( player_buffer[player_currentReadBuffer][35] << 8 ); // first byte read is the LSB, second byte is the HSB
	if (i == 16){
		player_is16bit = true;
	}else if (i == 8){ 
		player_is16bit = false;
	} else {
		#if defined (debug)
			Serial.print(F("WAV ERROR BPS = "));
			Serial.println(i);
		#endif
		return 0;
	}

	// ok, if we came to here, file seems to be in order...	
	// seek position 44 == goto start of PCM-data in WAV file
  readPositionInBuffer = 44;
	return 1;

}


// ####################################################################################
void player_play(char* filename){
	player_play(filename,0);
}


// ####################################################################################
void player_play(char* filename, unsigned long seekPoint){

  unsigned long temp_seek_point=0;

	if ( ! player_openFile(filename) ){
		#if defined (debug)
			Serial.println(F("WAV ERROR cannot open new file"));
		#endif
		return false;
	}

	// after player_checkWAVFile() the sdc_File.curPosition() currently == 512 and readPositionInBuffer == 44
	// also SDC-reads shall only be done in modulo512=0 steps so as to utilize SDC-buffer-pages (512 bytes each!) optimally.
	// so take that into account when calculating seek position

  // if calculated seek point is "somewhere" and less than 256 Bytes from next 512byte-chunk away, then go to this 512byte-chunk-border.
  // eg.  seek point is 200bytes --> use 0 bytes
  //      seek point is 387bytes --> use 512 bytes
  //      seek point is 4567890bytes --> use 4567890/512 = 8921 * 512 + 338 --> 8922*512 = 4568064 bytes
//	if(seekPoint > 0){ 
//		if ( player_is16bit ) {
//        seekPoint = (2*PCM_SAMPLE_RATE*seekPoint);
//		} else {
//        seekPoint = (PCM_SAMPLE_RATE*seekPoint) + 44;
//		}
//   
//    temp_seek_point = seekPoint % 512;
//    if (temp_seek_point > 255) {
//        seekPoint += (512 - temp_seek_point);
//    } else {
//        seekPoint -= temp_seek_point;
//    }
//
//    if ( seekPoint < 512 ) {
//        // only move in buffer (the first buffer that is already filled because of player_checkWAVFile()
//        if ( seekPoint < 44 ) {
//            readPositionInBuffer = 44; // correct for metadata
//        } else {
//            readPositionInBuffer = seekPoint; // this should never happen: actually it should never happen, that seekpoint is >0 and < 512!
//        }
//    } else {
//        pf_lseek(seekPoint);
//    }
//	}

	player_isStopped = false;
	player_isInterupted = false;
	player_isPaused = false;

    
	
	#if defined (debug)
		Serial.print(F("TimerCount1 max-value: "));
		Serial.println(ICR1);
	#endif

	// latest here the first buffer needs to be filled.
  // it already is, because of player_checkWWAVFile() function...
  noInterrupts();

	ENABLE_BUFFER_INTERUPT;
  ENABLE_PCM_FEED_INTERUPT; // do that after activating BufferInterupt
    
  player_isPlaying = true;

  interrupts();

}






// ####################################################################################
void player_stop(){
	player_isPlaying = false;
	// stop overrides paused or interupted
	player_isStopped = true;
	player_isInterupted = false;
	player_isPaused = false;
	DISABLE_PCM_FEED_INTERUPT; // do that first before deactivating BufferInterupt
	DISABLE_BUFFER_INTERUPT;

  // we don't need to explicitely close the open file, it will be discarded on next file_open...
	
	// reset/clear buffers into a well-definied setup
	player_setupBuffers();
}

// ####################################################################################
void player_pause(){
	// pause only affects player if it is not stopped and not interupted
	if (!player_isStopped && !player_isInterupted){
	
		if (player_isPaused){
			player_isPaused = false;
			player_isPlaying = true;
			ENABLE_BUFFER_INTERUPT;
			ENABLE_PCM_FEED_INTERUPT; // do that after activating BufferInterupt
			
		} else {
			player_isPaused = true;
			player_isPlaying = false;
			DISABLE_PCM_FEED_INTERUPT;
		}
	}
}





// ####################################################################################
void player_setVolume(byte vol) {
	player_SWvolume = constrain(vol, 0, 9);
}

// ####################################################################################
//ludgerknorps debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
//~ void player_sendRecordedMicros(){
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

	if( player_bufferEmpty[player_currentWriteBuffer] ){
		
		//ludgerknorps ISR-debug
		#if defined (ISR_BUFFER_DEBUG_PIN)
			//~ if (player_currentWriteBuffer == 0  ) { 
				clrPinD0_D21(ISR_BUFFER_DEBUG_PIN);
			//~ }
		#endif

		// hold this interupt
		DISABLE_BUFFER_INTERUPT;
		// but allow others
		sei();

    Serial.print("ISR ");
    Serial.print(player_currentWriteBuffer);

    // block-read from SDcard
    pf_read( (byte*)player_buffer[player_currentWriteBuffer] , PCM_BUFFER_SIZE, &player_bytesFromSDC );

    Serial.print(player_bytesFromSDC);

		// stop playback and discard this buffer, if it is not fully filled (i.e. pf_read reached EOF)
		if( player_bytesFromSDC < PCM_BUFFER_SIZE){
  			// hold "player-interupt"
  			DISABLE_PCM_FEED_INTERUPT;			 
		  	player_isPlaying = false; 	
		  	//
		  	//		  	
		  	smMain.trigger(smMain_event_stop);
		  	//
		  	//	
  	} else {
			//player_bufferEmpty[player_currentWriteBuffer] = false;
			// goto next buffer (or "roll over" to first buffer, if we were at the last one
			if (player_currentWriteBuffer < NUMBER_OF_PCM_BUFFERS - 1 ) { 
				player_currentWriteBuffer += 1;
			} else { 
				player_currentWriteBuffer = 0;
			}

      Serial.println(player_currentWriteBuffer);
			
			// un-hold this buffer
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

//	if( ! player_is16bit ){
//		// if 8bit, we only write OCR1A and not OCR1B output-pins:
//		// the hardware is setup for 16bit, thus the signal would be distorted if we outbput something at OCR1B.
//		// instead we set OCR1B to 0
//		OCR1B = 0;
//		if(player_SWvolume == 5){  
//			// no bit shift necessary
//			  OCR1A = player_buffer[player_currentReadBuffer][readPositionInBuffer];
//	  	} else if(player_SWvolume > 5) {  
//			  OCR1A = player_buffer[player_currentReadBuffer][readPositionInBuffer] << (player_SWvolume - 5);
//	  	} else { 
//			  OCR1A = player_buffer[player_currentReadBuffer][readPositionInBuffer] >> player_SWvolume;
//	  	}
//	  	readPositionInBuffer += 1;
//
//	} else {
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
		player_buffer[player_currentReadBuffer][readPositionInBuffer] += 127;
		
		//if (player_SWvolume == 5 ){
			// no bit shift necessary
			OCR1A = player_buffer[player_currentReadBuffer][readPositionInBuffer];
			OCR1B = player_buffer[player_currentReadBuffer][readPositionInBuffer+1];
//		} else if (player_SWvolume > 5 ){
//			// bit shift, we need a temporary 16bit variable for that
//			temp_OCRA_and_B_16bit_for_sw_volume_bitshift = (player_buffer[player_currentReadBuffer][readPositionInBuffer] << 8) | player_buffer[player_currentReadBuffer][readPositionInBuffer+1];
//			temp_OCRA_and_B_16bit_for_sw_volume_bitshift = temp_OCRA_and_B_16bit_for_sw_volume_bitshift << (player_SWvolume - 5);
//			OCR1A = highByte( temp_OCRA_and_B_16bit_for_sw_volume_bitshift);
//			OCR1B = lowByte(  temp_OCRA_and_B_16bit_for_sw_volume_bitshift);
//		} else {
//			// bit shift, we need a temporary 16bit variable for that
//			temp_OCRA_and_B_16bit_for_sw_volume_bitshift = (player_buffer[player_currentReadBuffer][readPositionInBuffer] << 8) | player_buffer[player_currentReadBuffer][readPositionInBuffer+1];
//			temp_OCRA_and_B_16bit_for_sw_volume_bitshift = temp_OCRA_and_B_16bit_for_sw_volume_bitshift >> player_SWvolume;
//			OCR1A = temp_OCRA_and_B_16bit_for_sw_volume_bitshift >> 8;
//			OCR1B = temp_OCRA_and_B_16bit_for_sw_volume_bitshift & B11111111;
//		}
		// two-byte-samples because of 16bit PCM
  	readPositionInBuffer += 2 ;
//  } 

  if(readPositionInBuffer >= PCM_BUFFER_SIZE){
  	readPositionInBuffer = 0;
  	player_bufferEmpty[player_currentReadBuffer] = true;
  	// goto next buffer (or "roll over" to first buffer, if we were at the last one
  	if (player_currentReadBuffer < NUMBER_OF_PCM_BUFFERS - 1 ) { 
  			player_currentReadBuffer += 1;
  	} else { 
  			player_currentReadBuffer = 0;
  	}
  }

  //Serial.println(player_currentReadBuffer);
  	
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











 
