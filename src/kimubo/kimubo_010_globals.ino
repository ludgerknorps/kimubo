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

      // Our Player instance
      static LKpcm player;
      

      // player needs to know a) current playlist/dir and b) current track/file and - only for interupts by system-messages - also needs to remember c) postition-of-interupt
      // we only have playlists '0' to '8' thus length of the playlist-string must be 1
      static char player_current_playlist_dirname;
      static byte player_current_track;
      // in each playlist/dir we have files 0.WAV, 1.WAV, ..., 10.WAV, 11.WAV, ..., 100.WAV, 101.WAV, ... 254.WAV (that is the maximum! 255 is "no-value") Thus length of this string is at most SUFFIX_PCM_FILES + 3
      // additionally: 1 char each for leading and separating "/" and the dirname 
      // to be safe and to also include "SYSTEM" dir: 20
      static char player_current_track_filename[20];
      

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

    
			// Helper function: make filename from current_track_number
      void get_new_track_player_filename();

			

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
    
    // SD already provides instance

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

//    extern State smMain_state_init;
//    extern State smMain_state_idle;
//    extern State smMain_state_playWav;
//    extern State smMain_state_pause;
//    extern State smMain_state_seek;
//    extern State smMain_state_seekBack;
//    extern State smMain_state_playMessage;
//    extern State smMain_state_powerSave;

  // state and transition callback functions
  
//    void smMain_state_init_onState();
//    
//    void smMain_state_idle_onEnter();
//
//    void smMain_state_playWav_onEnter();
//
//    void smMain_state_pause_onEnter();
//    
//    void smMain_state_seek_onEnter();
//    
//    void smMain_state_seekBack_onEnter();
//
//    void smMain_state_playMessage_onEnter();
//    
//    void smMain_state_powerSave_onEnter();
//    void smMain_state_powerSave_onExit();
//    
//    
//    // transitionActions
//    void smMain_trans_B1_onTransition();
//    void smMain_trans_B2_onTransition();
//    void smMain_trans_B3_onTransition();
//    
//    void smMain_trans_D1_onTransition();
//    void smMain_trans_D2_onTransition();
//    
//    void smMain_trans_E1_onTransition();
//    void smMain_trans_E2_onTransition();
//    
//    void smMain_trans_F2_onTransition();
//    void smMain_trans_F5_onTransition();
//    
//    void smMain_trans_G1_onTransition();
//    void smMain_trans_G2_onTransition();
//    void smMain_trans_G4_onTransition();
//    void smMain_trans_G5_onTransition();
//    void smMain_trans_G6_onTransition();
//    void smMain_trans_G7_onTransition();

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

//  // functions
//    bool smMain_init_stateMachine();
//
//  // the FSM itself
//    extern Fsm smMain;

    static unsigned long myTime;

// EOF
