/* 
 * KIMUBO
 * KInderMUsikBOx
 * 
 * Ein minimalistischer WAV/PCM Spieler für Kinder - und mehr.
 * 
 * (c) 2019 ludgerknorps
 * 
 * tbd.: LGPL 
 * 
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
	 * Abschnitt PCM player
	 */


		// User defined constants	
			#define NUMBER_OF_PCM_BUFFERS 	4  	
			// how many pcm buffers, do we use? 
			// each can be max 256 bytes big (because it is addressed by a byte-type). 
			// buffer size must be even number.
			// all together must fit into RAM. 
			// All buffers together form one big "semi-ring-buffer".
			#define PCM_BUFFER_SIZE 		128  // must be even number (because of 16bit == 2 byte read at-a-time)	 
		
		

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			static const byte AUDIO_PIN_OUT_HIGH		= 9; 
			static const byte AUDIO_PIN_OUT_LOW			= 10; 
			
			
		//ludgerknorps debug: wie viele Microsekunden zwischen PWM Interupt Aufrufen
			//static unsigned long myMicros[10];
			//static byte myMicrosCounter = 0;
		
		//ludgerknorps ISR-debug
			// Interupt-debug: 	at begin of each ISR, one pin (for each interupt) is set low; at end of ISR the pin is set to high again.
			// 					this is for debugging the exact timing via e.g. oscilloscope or logic analyzer
			// ATTENTION: uses fast-setpin-macros, beware of using pins that are used for something else, no checks are done!
				// pin A4 = pin D18, pin A5 = pin D19!
				//#define ISR_BUFFER_DEBUG_PIN 	4
				//#define ISR_PCMFEED_DEBUG_PIN	6

	 
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
			static const char KEYSCAN_1      =  '1' ;  
			static const char KEYSCAN_2      =  '2' ; 
			static const char KEYSCAN_3      =  '3' ;  
			static const char KEYSCAN_4      =  '4' ;  
			static const char KEYSCAN_5      =  '5' ;  
			static const char KEYSCAN_6      =  '6' ; 
			static const char KEYSCAN_7      =  '7' ;  
			static const char KEYSCAN_8      =  '8' ;  
			static const char KEYSCAN_9      =  '9' ;  
			static const char KEYSCAN_FFWD   =  'F' ;  // FastForwar / Skip
			static const char KEYSCAN_REW    =  'R' ;  // Rewind / Skipback
			static const char KEYSCAN_STAT   =  'Z' ;  // Status (output via speech)
			static const char KEYSCAN_SLEEP  =  'S' ;  // Sleep-Timer-Set
			static const char KEYSCAN_LOUD   =  'L' ;  // Parental switch for Volume-Preset: Loud or whisper
			static const char KEYSCAN_PTT    =  'A' ;  // PushToTalk (for walkie talkie)
			static const char KEYSCAN_B      =  'B' ;  // reserved for future use
				  
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
			static const byte KEYB_PIN_ROW1   = A3;  // used as digital pin
			static const byte KEYB_PIN_ROW2   = A2;    // used as digital pin
			static const byte KEYB_PIN_ROW3   = A1;    // used as digital pin
			static const byte KEYB_PIN_ROW4   = A0;    // used as digital pin
			
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
			  { KEYSCAN_7,    KEYSCAN_8,    KEYSCAN_9,      KEYSCAN_LOUD  },
			  { KEYSCAN_FFWD, KEYSCAN_REW,    KEYSCAN_PTT,    KEYSCAN_B   }
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
			static const byte AMP_PIN_LEFTIN 			= 0; // via resistor-ladder to both PWM outputs #9 and #10 (AUDIO_PIN_OUT_HIGH and AUDIO_PIN_OUT_LOW)
			//static const byte AMP_PIN_RIGHTIN 		= 0; // not used
			static const byte AMP_PIN_GNDIN 			= 0; // directly connected to GND
			static const byte AMP_PIN_LEFTOUTPLUS 		= 0; // directly connected to SPEAKER_PIN_PLUS
			static const byte AMP_PIN_LEFTOUTGND 		= 0; // directly connected to SPEAKER_PIN_MINUS
			//static const byte AMP_PIN_RIGHTOUTPLUS 	= 0; // not used
			//static const byte AMP_PIN_LEFTOUTGND 		= 0; // not used
			static const byte AMP_PIN_POWERPLUS 		= 0; // directly connected to BATTERY_PIN_PLUS
			static const byte AMP_PIN_POWERGND 			= 0; // directly connected to GND and BATTERY_PIN_GND
			static const byte AMP_PIN_SWITCHED_PLUS 	= 0; // directly connected to RAW // this is our power supply!
			static const byte AMP_PIN_VOL_FEEDBACK		= A6;
			static const byte AMP_PIN_MUTE	 			= A7; // used as digital pin
			static const byte AMP_PIN_SUSPEND			= A4; // used as digital pin
			
			
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

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			static const byte SDC_PIN_MOSI	 			= 11; // MOSI
			static const byte SDC_PIN_MISO				= 12;  // MISO
			static const byte SDC_PIN_SCK				= 13;  // SCK
			static const byte SDC_PIN_CS				= 4; 
			static const byte SDC_PIN_VCC		 		= 0; // directly connected to RAW
			static const byte SDC_PIN_GND	 			= 0; // directly connected to GND
			
			
			
		// Globals
		
			// define what suffix all PCM/WAV files need to haave on the SD-card.
			// relevant for "normal" PCM files as well as message-files.
			char SUFFIX_PCM_FILES[5] = ".WAV";
			

		



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
		// fast (!) replacements for digitalWrite() and digitalRead() - about 20x faster than digitalWrite().
		// see https://www.best-microcontroller-projects.com/arduino-digitalwrite.html for more explanation
		// use as in 
		// 			setPinD0_D7(5)			--> set Pin D5  aka. PORTD.5 = HIGH
		//			clrPinD8_D13(12)		--> clear Pin D12 aka. PORTB.5 = LOW
		// 			readPinA0_A7(5)			--> read Pin A5  aka. PORTC.5 
		//			writePinD0_D7(5,LOW)	--> clear Pin D5  aka. PORTD.5 = LOW 
			#define setPinD0_D7(pinNo) 		( PORTD |=(1<<(pinNo)) )
			#define setPinD8_D13(pinNo) 	( PORTB |=(1<<(pinNo-8)) )
			#define setPinA0_A7(pinNo) 		( PORTC |=(1<<(pinNo)) )
			#define setPinD14_D21(pinNo) 	( setPinA0_A7(pinNo-13) )

			#define clrPinD0_D7(pinNo) 		( PORTD &=~(1<<(pinNo)) )
			#define clrPinD8_D13(pinNo) 	( PORTB &=~(1<<(pinNo-8)) )
			#define clrPinA0_A7(pinNo) 		( PORTC &=~(1<<(pinNo)) )
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
			#define readPinD0_D7(pinNo) 	( (PORTD &(1<<(pinNo)))!=0 )
			#define readPinD8_D13(pinNo) 	( (PORTB &(1<<(pinNo-8)))!=0 )
			#define readPinA0_A7(pinNo) 	( (PORTD &(1<<(pinNo)))!=0 )

		
		
#endif // KIMUBO_H definition
