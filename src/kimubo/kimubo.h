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
/*
 * 
 * 
 * 
 * 
 * 
 * 
 * List of used ressources:
 * 
 * 		#######################################
 * 			https://github.com/hnauen/Fritzing/blob/master/GF1002_StereoAmplifier/GF1002_StereoAmplifier.fzpz
 * 			Fritzing part for PAM8403 Amp
 * 			creative commons license 4.0: Attribution-ShareAlike CC BY-SA
 * 		#######################################
 * 
 * 
 * 
 */

#ifndef KIMUBO_H
#define KIMUBO_H



#include <Arduino.h>
	
	


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
	 * Abschnitt DEBUG
	 */

		// defining debug --> serial output delivers information on internals, timing and cpu load may be a little of compared to non-debug-mode
    #define debug


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
   * Abschnitt smMain
   */

    // for seeking, i.e. fast forward or backward in a track
    //        In order to do that we play a short part of the track,
    //        then hop to a position a little further, play again, hop again, etc.
    //        There are two relevant user-defined constants:
    //          a) SEEK_SPEEDUP       how fast shall seeking be (in times of normal play speed)
    //          b) SEEK_DURATION_PLAYING  how much of a track shall be played between hops (in milliseconds)
    #define SEEK_SPEEDUP      10
    #define SEEK_DURATION_PLAYING 10
    //        From those user-defined constants we can derive some internally relevant auto-constants:
    //          c) SEEK_INTERVAL_LENGTH   how long is a one seek interval in total (playing + hopping)
    //          d) SEEK_INTERVAL_HOP_FWD  how far do we hop forward before playing again
    //          e) SEEK_INTERVAL_HOP_BWD  how far do we hop backward before playing again
    #define SEEK_INTERVAL_LENGTH  (1000/SEEK_SPEEDUP)
    #define SEEK_INTERVAL_HOP_FWD (SEEK_INTERVAL_LENGTH-SEEK_DURATION_PLAYING)
    #define SEEK_INTERVAL_HOP_BWD (SEEK_INTERVAL_LENGTH+SEEK_DURATION_PLAYING)
    
    // for powersaving there is one relevant user-defined constant:
    //          a) POWERSAVE_AFTER      after which time (in milliseconds) shall device enter low-power-state
    //                        coming from idle, the low-power-state is recoverable, 
    //                        i.e. we can return to idle on e.g. keypress
    #define POWERSAVE_AFTER     3000000
    //        From this user-defined constant we can derive some internally relevant auto-constants:
    //          b) POWERSAVE_AFTER_PAUSE  after which time in pause-state shall device enter low-power-state
    //                        (in times of POWERSAVE_AFTER, not milliseconds!)
    //                        coming from pause, the low-power-state is recoverable, 
    //                        i.e. we can return to pause on e.g. keypress BUT NOT by just turning
    //                        the volume up again, thus do not choose a too small value here!
    #define POWERSAVE_AFTER_PAUSE (60*POWERSAVE_AFTER)
		

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
	 * Abschnitt PCM player
	 */

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			#define AUDIO_PIN_OUT_HIGH		9 
			#define AUDIO_PIN_OUT_LOW		10

      #define AUDIO_PIN_LOUDNESS  5


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
	 * Abschnitt EEPROM
	 */
	 
		

		/* EEPROM Data Layout:
		 *  EEPROM_BASE_ADDR 
		 *  	+ 0 Bytes = four Byte (type long) for a chip specific compensation value for accurate VCC measurements
		 *
		 *  	+ 4 Bytes = one Byte type byte with ragng (1 ..9) for the last playlist that was played
		 *
		 * 		+ 1 Bytes = two Bytes type unsigned int for the last track that was played in playlist 1
		 *		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 2
		 * 		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 3
		 * 		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 4
		 *		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 5
		 * 		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 6
		 * 		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 7
		 *		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 8
		 * 		+ 2 Bytes = two Bytes type unsigned int for the last track that was played in playlist 9
		 * 
		 * 		+ 2 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 1
		 *		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 2
		 * 		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 3
		 * 		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 4
		 *		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 5
		 * 		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 6
		 * 		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 7
		 *		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 8
		 * 		+ 4 Bytes = four Bytes type unsigned long for the last position in last track that was played in playlist 9
		 * 
		 * 		in sum : 59 byte
		 */
		static const byte EEPROM_BASE_ADDR      		= 	0	; // this one should not be used directly in code, instead use specific addresses form below
																  // can be set to any value between 0 and "real_eeprom_size" - all bytes used (currently 59)
																  // therefore for ATMEGA328P with 1024byte EEPROM: between 0 .. 964
																  // BEWARE:
																  // we have 1024 bytes of EEPROM on the ATmega328P --> addresses in EEPROM need to be of type "short", 
																  // if we need to address whole EEPROM space.
																  // As we only need a subset of the address-space we can use byte type instead for addresses, 
																  // iff (!) we limit base address + max needed space (= upper limit of used space)
																  // to bee 255 or less.
		
		static const byte EEPROM_VCCCOMPENSATION_ADDR	=	EEPROM_BASE_ADDR + 0; // four bytes used
		
		static const byte EEPROM_LAST_PLAYLIST_ADDR		=	EEPROM_VCCCOMPENSATION_ADDR + 4	; // one byte used
		
		static const byte EEPROM_LAST_TRACK_IN_PLAYLIST[9] = {	EEPROM_LAST_PLAYLIST_ADDR + 1,
																EEPROM_LAST_PLAYLIST_ADDR + 3,
																EEPROM_LAST_PLAYLIST_ADDR + 5,
																EEPROM_LAST_PLAYLIST_ADDR + 7,
																EEPROM_LAST_PLAYLIST_ADDR + 9,
																EEPROM_LAST_PLAYLIST_ADDR + 11,
																EEPROM_LAST_PLAYLIST_ADDR + 13,
																EEPROM_LAST_PLAYLIST_ADDR + 15,
																EEPROM_LAST_PLAYLIST_ADDR + 17}; // two bytes used, each
																
		static const byte EEPROM_LAST_POSITION_IN_TRACK_IN_PLAYLIST[9] = {	EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 2,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 6,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 10,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 14,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 18,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 22,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 26,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 30,
																			EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 34}; // four bytes used, each


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
	 * Abschnitt MatrixKeyboard
	 */
		 
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 1. Definition KeyScanCodes
			static const char KEYSCAN_1      =  '0' ;  
			static const char KEYSCAN_2      =  '1' ; 
			static const char KEYSCAN_3      =  '2' ;  
			static const char KEYSCAN_4      =  '3' ;  
			static const char KEYSCAN_5      =  '4' ;  
			static const char KEYSCAN_6      =  '5' ; 
			static const char KEYSCAN_7      =  '6' ;  
			static const char KEYSCAN_8      =  '7' ;  
			static const char KEYSCAN_9      =  '8' ;  
			static const char KEYSCAN_FFWD   =  'F' ;  // FastForwar / Skip
			static const char KEYSCAN_REW    =  'R' ;  // Rewind / Skipback
			static const char KEYSCAN_STAT   =  'Z' ;  // Status (output via speech)
			static const char KEYSCAN_SLEEP  =  'S' ;  // Sleep-Timer-Set
			static const char KEYSCAN_A    	 =  'A' ;  // reserved for future use
			static const char KEYSCAN_B      =  'B' ;  // reserved for future use
      static const char KEYSCAN_C      =  'C' ;  // reserved for future use
				  
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 2. Aus wievielen Reihen und Spalten ist Matrix des Keyboards aufgebaut?

			// real hardware no. of rows/colums
			static const byte KEYB_ROWS = 4;
			static const byte KEYB_COLS = 4;
			
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 3. Which pin has which matrix-keyboard-row/-column?
			
			// pins are Arduino pin nr. 
			static const byte KEYB_PIN_COLUMN1  = 8;
			static const byte KEYB_PIN_COLUMN2  = 7;
			static const byte KEYB_PIN_COLUMN3  = 6;
			static const byte KEYB_PIN_COLUMN4  = 5;
			static const byte KEYB_PIN_ROW1   = A0;  // used as digital pin
			static const byte KEYB_PIN_ROW2   = A1;    // used as digital pin
			static const byte KEYB_PIN_ROW3   = A2;    // used as digital pin
			static const byte KEYB_PIN_ROW4   = A3;    // used as digital pin
			
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 4. Which Pins are rows, which are columns 
		  
			// für Power Down müssen alle Reihen-Pins auf High gesezt werden (und danach wieder auf Low)
			// dazu müssen wir festlagen, welches die Reihen-Pins und welches die Spalten-Pins sind
			static const byte KEYB_RowPins[KEYB_ROWS] = { 
			  KEYB_PIN_ROW1, 
			  KEYB_PIN_ROW2,
			  KEYB_PIN_ROW3,
			  KEYB_PIN_ROW4
			}; //connect to the row pinouts of the keypad 
							
			static const byte KEYB_ColPins[KEYB_COLS] = {
			  KEYB_PIN_COLUMN1,
			  KEYB_PIN_COLUMN2,
			  KEYB_PIN_COLUMN3,
			  KEYB_PIN_COLUMN4
			}; //connect to the column pinouts of the keypad

			
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 5. damit µC zwischendurch in Powerdown gehen kann, müssen alle Keyboard-Inputs (=Reihen, s.o.) mit je einer Diode an diesen Interrupt-Pin angeschlossen sein; dieser weckt dann den µC auf)
			static const byte KEYB_PIN_INTERRUPT = 2;
			
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 6. Die TastaturBelegung selbst = welche Taste ist wo?
			  
			static const char KEYB_keyScans[KEYB_ROWS][KEYB_COLS] = {
			  { KEYSCAN_1,    KEYSCAN_2,    KEYSCAN_3,      KEYSCAN_STAT  },
			  { KEYSCAN_4,    KEYSCAN_5,    KEYSCAN_6,      KEYSCAN_SLEEP },
			  { KEYSCAN_7,    KEYSCAN_8,    KEYSCAN_9,      KEYSCAN_C  },
			  { KEYSCAN_REW,  KEYSCAN_FFWD, KEYSCAN_A,    	KEYSCAN_B   }
			};
		  
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		  // 7. keypad settings
			
			//Set the amount of milliseconds the user will have to hold a button until the HOLD state is triggered. (default = 500)
			#define KEYB_HOLD_TIME  500 
			//Set the amount of milliseconds the keypad will wait until it accepts a new keypress/keyEvent. This is the "time delay" debounce method.  (default = 10)
			#define KEYB_DEBOUNCE_TIME  10  		

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
	 * Abschnitt AMP(lifier)
	 */

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			//~ #define AMP_PIN_LEFTIN 						0 // via resistor-ladder to both PWM outputs #9 and #10 (AUDIO_PIN_OUT_HIGH and AUDIO_PIN_OUT_LOW)
			//~ #define AMP_PIN_RIGHTIN 					0 // not used
			//~ #define AMP_PIN_GNDIN 						0 // directly connected to GND
			//~ #define AMP_PIN_LEFTOUTPLUS 				0 // directly connected to SPEAKER_PIN_PLUS
			//~ #define AMP_PIN_LEFTOUTGND 					0 // directly connected to SPEAKER_PIN_MINUS
			//~ #define AMP_PIN_RIGHTOUTPLUS 				0 // not used
			//~ #define AMP_PIN_LEFTOUTGND 					0 // not used
			//~ #define AMP_PIN_POWERPLUS 					0 // directly connected to BATTERY_PIN_PLUS
			//~ #define AMP_PIN_POWERGND 					0 // directly connected to GND and BATTERY_PIN_GND
			//~ #define AMP_PIN_SWITCHED_PLUS 				0 // directly connected to RAW // this is our power supply!
			// used as analog input pin:
			#define AMP_PIN_VOL_FEEDBACK					A6
			// used as digital output pins:
			#define AMP_PIN_MUTE	 						    3
			// #define AMP_PIN_SUSPEND							  255 
			
			
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
	 * Abschnitt SDCard
	 */

    #define SD_ChipSelectPin 4 

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			#define SDC_PIN_MOSI	 			11 
			#define SDC_PIN_MISO				12  
			#define SDC_PIN_SCK					13 
			#define SDC_PIN_CS					4 
			//~ #define SDC_PIN_VCC		 		0 // directly connected to RAW
			//~ #define SDC_PIN_GND	 			0 // directly connected to GND
			
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Globals
		
			// define what suffix all PCM/WAV files need to have on the SD-card.
			// relevant for "normal" PCM files as well as message-files.
			const char SUFFIX_PCM_FILES[5] = ".WAV";
			
			// where on SDcard are all our files stored?
			// we use hard-fixed "/", no user-definable variable!

      // define how the subdir is named, that holds system-message-pcm-files
      // relevant for "normal" PCM files as well as message-files.
      const char SDC_SYSTEM_MESSAGES_DIR[7] = "SYSTEM";

#endif // KIMUBO_H definition
