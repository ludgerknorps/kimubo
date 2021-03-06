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
		#define readPinD0_D7(pinNo) 	(( PORTD >> pinNo) & 1 )
		#define readPinD8_D13(pinNo) 	(( PORTB >> (pinNo-8) & 1 )
		#define readPinA0_A7(pinNo) 	(( PORTC >> pinNo) & 1 )



    bool isFirstLoop;

    long playerStoppedSince; // used for powersaving after playing stopped


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
		
		// Our Player instance
		static LKpcm player;
		
		
		// player needs to know a) current playlist/dir and b) current track/file and - only for interupts by system-messages - also needs to remember c) postition-of-interupt
		// we only have playlists '0' to '8' thus length of the playlist-string must be 1
		static char player_current_playlist_dirname;
		static byte player_current_track;
		// in each playlist/dir we have files 0.WAV, 1.WAV, ..., 10.WAV, 11.WAV, ..., 100.WAV, 101.WAV, ... 254.WAV (that is the maximum! 255 is "no-value") Thus length of this string is at most SUFFIX_PCM_FILES + 3
		// additionally: 1 char each for leading and separating "/" and the dirname and trailing '\0'
		// = 4+3+1+1+1+3 = 13
		// to be safe and to also include "SYSTEM" dir: 18
		static char player_current_track_filename[18];
		
		
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
		
		// loudness is set via softwarevolume controll of lkpcm lib
		//volatile bool player_is_loudness;
		void setLoudness(); 
		void unsetLoudness();  
		
			// Helper function: make filename from current_track_number
		void get_new_track_player_filename();
		
		// Helper function: while playing each track we spend some time on finding the previous track and the next track -> this will very much speed up continuous playback of whole playlist with all its tracks.
		void find_next_and_previous_files_in_current_playlist();
		
		// Helper function: store currently played track to EEPROM and then retrieve it 
		void remember_current_playlist_and_track_in_eeprom(const byte trackNr, const byte playlistNr);
		uint16_t get_last_playlist_and_track_from_eeprom();


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
 * Abschnitt AMP
 */

 		void amp_setup();
		void amp_activate();
		void amp_shutdown();
			

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

		long gv_UBat_ChipsVccCompensationValue; // read from EEPROM at startup
		long gv_UBat_in_millivolt; 	// holds the measured value

		void setupReadVcc();
		void readVcc();
		bool isLowBat();
		void shutDownBecauseOfUndervoltage();
		void checkBatteryVoltageAndShutdownIfNeccessary();

		void batteryVoltageAsMessageToParents();


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
    
    // File system object.
		SdFat sd;    


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
    void keypad_callback_onEvent_normalMode( char key);
    void keypad_callback_onEvent_parentAdminMode( char key );

    void keyb_addEventListenerNormalMode();
    void keyb_addEventListenerParentAdminMode();

    void keyb_waitForAllKeysToBeReleased();

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
 * Abschnitt Messages
 */
	//  global variable, we only need one filename at a time
	extern char messageFileName[11];

	void getMessageFilenameFromByteValue(byte byteValue);
	void playMessage(const byte* theMessage, const byte theMessageLength);
	void playMessage(const byte* theMessage, const byte theMessageLength, const bool fromProgMem);

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
 * Abschnitt ParentAdminMode
 */

	extern byte gv_parentAdminModeSettings;

	void parentAdminModeLoop();
	bool checkForParentAdminModeAtDeviceStartup();
	void parentAdminModeReadVcc();

	#define setParentAdminSettingLoudness() 		( gv_parentAdminModeSettings |= (1<<(0)) )
	#define setParentAdminSettingGreeting()			( gv_parentAdminModeSettings |= (1<<(1)) )
	#define setParentAdminSettingAutoplay() 		( gv_parentAdminModeSettings |= (1<<(2)) )
	#define clrParentAdminSettingLoudness() 		( gv_parentAdminModeSettings &= ~(1<<(0)) )
	#define clrParentAdminSettingGreeting() 		( gv_parentAdminModeSettings &= ~(1<<(1)) )
	#define clrParentAdminSettingAutoplay() 		( gv_parentAdminModeSettings &= ~(1<<(2)) )
	#define readParentAdminSettingLoudness() 		(( gv_parentAdminModeSettings >> 0) & 1 )
	#define readParentAdminSettingGreeting() 		(( gv_parentAdminModeSettings >> 1) & 1 )
	#define readParentAdminSettingAutoplay() 		(( gv_parentAdminModeSettings >> 2) & 1 )

// EOF
