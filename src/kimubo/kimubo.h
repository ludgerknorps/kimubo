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
	 * Abschnitt HW/Schaltungsbeschreibung
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
		
		static const byte EEPROM_VCCCOMPENSATION_ADDR	=	EEPROM_BASE_ADDR + 0; // four bytes used
		
		static const byte EEPROM_LAST_PLAYLIST_ADDR		=	EEPROM_VCCCOMPENSATION_ADDR + 4	; // one byte used
		
		static const byte EEPROM_LAST_TRACK_IN_PLAYLIST[8] = {	EEPROM_LAST_PLAYLIST_ADDR + 1,
																EEPROM_LAST_PLAYLIST_ADDR + 3,
																EEPROM_LAST_PLAYLIST_ADDR + 5,
																EEPROM_LAST_PLAYLIST_ADDR + 7,
																EEPROM_LAST_PLAYLIST_ADDR + 9,
																EEPROM_LAST_PLAYLIST_ADDR + 11,
																EEPROM_LAST_PLAYLIST_ADDR + 13,
																EEPROM_LAST_PLAYLIST_ADDR + 15,
																EEPROM_LAST_PLAYLIST_ADDR + 17}; // two bytes used, each
																
		static const byte EEPROM_LAST_POSITION_IN_TRACK_IN_PLAYLIST[8] = {	EEPROM_LAST_TRACK_IN_PLAYLIST[8] + 2,
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
	 * Abschnitt Audio
	 */

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Pins
			// pins are Arduino pin nr.	
			static const byte AUDIO_PIN_OUT_HIGH		= 9; 
			static const byte AUDIO_PIN_OUT_LOW			= 10;  
			
			
		
		
#endif // KIMUBO_H definition
