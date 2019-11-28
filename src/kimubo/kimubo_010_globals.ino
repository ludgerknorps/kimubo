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
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt fast (!) replacements for digitalWrite() and digitalRead()
 */
	
	// ludgerknorps stuff
	// fast (!) replacements for digitalWrite() and digitalRead() - about 10x faster than digitalWrite().
	// (for general information see https://www.best-microcontroller-projects.com/arduino-digitalwrite.html for more explanation)
	// use as in 
	// 			setPinD0_D7(5)			--> set Pin D5  aka. PORTD.5 = HIGH
	//			clrPinD8_D13(12)		--> clear Pin D12 aka. PORTB.5 = LOW
	// 			readPinA0_A7(5)			--> read Pin A5  aka. PORTC.5 
	//			writePinD0_D7(5,LOW)	--> clear Pin D5  aka. PORTD.5 = LOW 
		#define setPinD0_D7(pinNo) 		( PORTD |= (1<<(pinNo)) )
		#define setPinD8_D13(pinNo) 	( PORTB |= (1<<(pinNo-8)) )
		#define setPinA0_A7(pinNo) 		( PORTC |= (1<<(pinNo)) )
		#define setPinD14_D21(pinNo) 	( setPinA0_A7(pinNo-13) )

		#define clrPinD0_D7(pinNo) 		( PORTD &= ~(1<<(pinNo)) )
		#define clrPinD8_D13(pinNo) 	( PORTB &= ~(1<<(pinNo-8)) )
		#define clrPinA0_A7(pinNo) 		( PORTC &= ~(1<<(pinNo)) )
		#define clrPinD14_D21(pinNo) 	( clrPinA0_A7(pinNo-13) )

		// more comfortable but slightly slower 
		#define writePinD0_D7(pinNo, boolValue) 	( (boolValue && setPinD0_D7(pinNo))  || clrPinD0_D7(pinNo) )
		#define writePinD8_D13(pinNo, boolValue) 	( (boolValue && setPinD8_D13(pinNo)) || clrPinDD8_D13(pinNo) )
		#define writePinA0_A7(pinNo, boolValue) 	( (boolValue && setPinA0_A7(pinNo))  || clrPinA0_A7(pinNo) )
		
		// more comfortable but even more (slightly) slower 
		#define setPinD0_D21(pinNo) 				( ((pinNo)>13) ? setPinA0_A7(pinNo-13) : ( (pinNo)>8 ? setPinD8_D13(pinNo-8) : setPinD0_D7(pinNo) ) )
		#define clrPinD0_D21(pinNo) 				( ((pinNo)>13) ? clrPinA0_A7(pinNo-13) : ( (pinNo)>8 ? clrPinD8_D13(pinNo-8) : clrPinD0_D7(pinNo) ) )
		#define writePinD0_D21(pinNo, boolValue) 	( (boolValue && setPinD0_D21(pinNo))  || clrPinD0_D21(pinNo) )
		
		// sometimes reading is better than writing...
		#define readPinD0_D7(pinNo) 	( PORTD >> pinNo) & 1 )
		#define readPinD8_D13(pinNo) 	( PORTB >> (pinNo-8) & 1 )
		#define readPinA0_A7(pinNo) 	( PORTC >> pinNo) & 1 )





// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt Audio / PCM Player
 */

		// global variables	

      // player needs to know a) current playlist/dir and b) current track/file and - only for interupts by system-messages - also needs to remember c) postition-of-interupt
      // we only have playlists '0' to '8' thus length of the playlist-string must be 1
      static char player_current_playlist_dirname;
      static byte player_current_track;
      // in each playlist/dir we have files 0.WAV, 1.WAV, ..., 10.WAV, 11.WAV, ..., 100.WAV, 101.WAV, ... 254.WAV (that is the maximum! 255 is "no-value") Thus length of this string is at most SUFFIX_PCM_FILES + 3
      static char player_current_track_filename[strlen(SUFFIX_PCM_FILES) + 3];
      

      // we also have to memorize:
      // a) how many tracks are in each of the dirs? --> this is done at startup of the device (and means, never ever remove SDC in operation!)
      //    actually we rather remember: highest "track-number" per dir as there might be missing tracks in a dir
      //    e.g.   /1 has four tracks "/1/0.WAV", "/1/1.WAV", /1/2.WAV", /1/3.WAV" --> highest tracknumber is "3" and there are four files
      //      but: /2 has also four files "/2/0.WAV", "/2/1.WAV", /2/7.WAV", /2/8.WAV" --> highest tracknumber is "8" and there are still only four files
      //    the player must know that "8" is highest tracknumber. If it is playing "/2/1.WAV" and wants to play next track, it knows (because of tracknumber-max) that there are "next tracks".
      //    It then tries to play "/2/2.WAV" and encounters "notExist", tries "/2/3.WAV", again "notExist", ..., tries "/2/7.WAV", here you go!
      // b) same also for lowest "track-number", e.g. if /2 has four files "/2/2.WAV", "/2/3.WAV", /2/7.WAV", /2/8.WAV"
      volatile byte player_track_number_min[9]; // one array field per playlist (we dont need that for "systemmessages").
      volatile byte player_track_number_max[9]; // one array field per playlist (we dont need that for "systemmessages").

    
			// array of buffers for PCM data coming from SDC
			volatile byte player_buffer[NUMBER_OF_PCM_BUFFERS][PCM_BUFFER_SIZE];
			
			// array-index-"pointer" to buffer from to points of view: a) I want to read buffer, b) I want to write buffer
			// we write to other buffer than reading from, it is a buffer after all ...
			volatile byte player_currentReadBuffer = 0;
			volatile byte player_currentWriteBuffer = 0;
			// flags for status of each buffer (full/empty --> non-writable/writable)
			volatile bool player_bufferEmpty[NUMBER_OF_PCM_BUFFERS];
			// writing is doneblock-wise in blocks of PCM_BUFFER_SIZE, but reading is done byte-wise (or double-byte-wise in case of 16bit PCM)
			// thus, we need a position-"pointer" (array-index) for intra-buffer
			volatile byte readPositionInBuffer = 0;

			bool player_is16bit = false;
			bool player_isPaused = false;
			volatile bool player_isPlaying = false; // must be volatile because we change it from inside the ISR at end of a file
			bool player_isStopped = false;
			bool player_isInterupted = false;

			byte player_SWvolume = 5; // software volume range is 0 (most silent) to 9 (max volume), default value is 5 (middle)
			
			volatile unsigned int temp_OCRA_and_B_16bit_for_sw_volume_bitshift = 0;
			
		// global public functions (because of ISR we cannot put them easily into a class!)
			
			// general "make ready" functions
      bool player_setup();
      
			void player_setupPWMPins();
			void player_setupTimers();
			
			// setup/reset buffers
			void player_setupBuffers();

			bool player_openFile(char* filename);
			bool player_checkWAVFile(char* filename);
			
			void player_play(char* filename);
			void player_play(char* filename, unsigned long seekPoint);
			void player_stop();
			void player_pause();
			
			// set Volume goes from 0 .. 8 with 4 being default (= interprets the pcm data without  amplification or reduction)
			void player_setVolume(byte vol);

      // Helper function: make filename from current_track_number
      void get_new_track_player_filename();

			// we want to 
			// 1) use several helper-functions in order to clean up the code and
			// 2) on the other side don't want to use function calls inside of ISR (wherever possible) because of performance impacts and
			// 3) we do want to code behaviour only oncc.
			// Thus - allthough is is not optimal coding style - we declare macros, use them inside of functions and inside ISR as well

			#define ENABLE_BUFFER_INTERUPT			(TIMSK1 |= _BV(ICIE1))
			#define DISABLE_BUFFER_INTERUPT			(TIMSK1 &= ~_BV(ICIE1))
			#define ENABLE_PCM_FEED_INTERUPT		(TIMSK1 |= _BV(TOIE1))
			#define DISABLE_PCM_FEED_INTERUPT		(TIMSK1 &= ~_BV(TOIE1))
			
			
			// additional debugging functions
			//void player_sendRecordedMicros();


// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt UBat measurement
 */




		long gv_UBat_ChipsVccCompensationValue;
		long gv_UBat_in_millivolt; 


// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt SD Card
 */
    
    // SDFAT
    // File system object.
        SdFatEX sdc_fileSystem;
        SdBaseFile sdc_Dir;
        SdBaseFile sdc_File;

    // functions
		bool sdc_setup();



// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt KEypad
 */
    Keypad keypad = Keypad( makeKeymap(KEYB_keyScans), KEYB_RowPins, KEYB_ColPins, KEYB_ROWS, KEYB_COLS );

    bool keyb_setup();

    extern char keyb_current_playListKey;



// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt FSM smMain
 */

    extern State smMain_state_init;
    extern State smMain_state_idle;
    extern State smMain_state_playWav;
    extern State smMain_state_pause;
    extern State smMain_state_seek;
    extern State smMain_state_seekBack;
    extern State smMain_state_playMessage;
    extern State smMain_state_powerSave;

  // state and transition callback functions
  
    void smMain_state_init_onEnter();
    
    void smMain_state_idle_onEnter();

    void smMain_state_playWav_onEnter();

    void smMain_state_pause_onEnter();
    
    void smMain_state_seek_onEnter();
    
    void smMain_state_seekBack_onEnter();

    void smMain_state_playMessage_onEnter();
    
    void smMain_state_powerSave_onEnter();
    void smMain_state_powerSave_onExit();
    
    
    // transitionActions
    void smMain_trans_B1_onTransition();
    void smMain_trans_B2_onTransition();
    void smMain_trans_B3_onTransition();
    
    void smMain_trans_D1_onTransition();
    void smMain_trans_D2_onTransition();
    
    void smMain_trans_E1_onTransition();
    void smMain_trans_E2_onTransition();
    
    void smMain_trans_F2_onTransition();
    void smMain_trans_F5_onTransition();
    
    void smMain_trans_G1_onTransition();
    void smMain_trans_G2_onTransition();
    void smMain_trans_G4_onTransition();
    void smMain_trans_G5_onTransition();
    void smMain_trans_G6_onTransition();
    void smMain_trans_G7_onTransition();

  // events (values don't matter, need to be unique each within this statemachine, though)
    #define smMain_event_init_completed         1
    #define smMain_event_stop             5
    #define smMain_event_playWav            10
    #define smMain_event_playNextWav          12
    #define smMain_event_playOtherPlaylist        13
    #define smMain_event_pause              15
    #define smMain_event_resume             20
    #define smMain_event_skip             25
    #define smMain_event_skipBack           30
    #define smMain_event_seek             35
    #define smMain_event_seekBack           40
    #define smMain_event_gotoSleep            45
    #define smMain_event_wakeUpToIdle         47
    #define smMain_event_wakeUpToPause          48
    #define smMain_event_playMessage          50
    #define smMain_event_playNextMessagePart      53
    #define smMain_event_continue           55
    #define smMain_event_returnToPause          60
    #define smMain_event_returnToIdle         65
    #define smMain_event_fatalerror_detected      255

  // functions
    bool smMain_init_stateMachine();

  // the FSM itself
    Fsm smMain(&smMain_state_init);

// EOF
