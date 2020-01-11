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
 * Abschnitt Messages
 */
	/* 
	 * We have certain user interactions (setSleepTimer, readStatus, setLoudness) that require a feedback from the 
	 * KIMUBO to the user (in this case: parental user).
	 * Feedback is provided by the KIMUBO "speaking" to the user via pre-fabricated PCM files ("message-files").
	 * Those are also stored on the SDC in directory "/KIMUBOsystemmessages".
	 * 
	 * Prefabricated message-files have filenames 
	 * 			001.WAV, 002.WAV, 003.WAV, ..., 255.WAV
	 * We will be able to handle 255 different such files - addressing is done via a byte-type variable.
	 * 
	 * Playing messages is possible if either
	 * 		a) no normal WAV is being played (== player is stopped)
	 * 		b) normal WAV is being played (== player gets interrupted and remembers current position in normal WAV, message is played, normal WAV continues seamlessly)
	 * 
	 * Thus, 
	 * 		REQ1) message-player needs to be able to interrupt normal player and 
	 * 		REQ2) normal player needs to be 
	 * 			a) able to be interrupted and
	 * 			b) remember current position and 
	 * 			c) continue from that onwards.
	 * 
	 * Messages consist of several messge-tokens, each token to be stored inside a specific message-file.
	 * E.g. if we want to read out the current BAT voltage, then we might want to say:
	 * 		"Batteriespannung beträgt 4,8V"		("battery voltage is 4.8V")
	 * This would translate to tokens
	 * 		TOKEN	MESSAGE (german)					MESSAGE (engl. transl.)		BYTEVALUE (e.g.)
	 * 	  ----------------------------------------------------------------------------------------
	 * 		TOKEN1	"Batteriespannung beträgt "			("battery voltage is ")		17
	 * 		TOKEN2	"vier"								("four")					4
	 * 		TOKEN3	"Komma"								("point")					11
	 * 		TOKEN4	"acht"								("eight")					8
	 * 		TOKEN5	"Volt"								("volts")					18
	 * 
	 * These tokens are represented by a specific byte-value each.
	 * 
	 * If we want to define this message we need a byte[] = [17, 4, 11, 8, 18].
	 * If we want to play that message we need to
	 * 		- (if necessary) interupt current normal playing
	 * 		- iterate over our byte[] and play the corresponding message-files
	 * 		- (if necessary) continue with normal playing
	 * 
	 * In order to find the correct message-file from its corresponding byte-value a conversion is necessary: getMessageFilenameFromByteValue(byte byteValue)
	 */
		
	//  global variable, we only need one filename at a time
	char messageFileName[11];

	/* =========================================================== */
	void getMessageFilenameFromByteValue(const byte byteValue){
		
		// we overwrite the char-array at address messageFileName, thus we need a pointer to that address which we can manipulate
		const char* temp_zero = "0";
		const char* temp_doublezero = "00";
		char temp_filename[4];

		// add path up front
		strcpy(messageFileName, SDC_SYSTEM_MESSAGES_DIR); 
		
		// all message-files have a fixed filename-length of xxx.yyy, 
		//     where xxx is the corresponding byte value (as a String withlength 3) and 
		//     .yyy is the globally defined SUFFIX_PCM_FILES (typically something like ".WAV")
		if (byteValue < 10) {
			strcat(messageFileName, temp_doublezero); 
			itoa(byteValue, temp_filename, 10); 	// write the single digit into next position ( messageFileName now is "/M/00DTxxxx" where M is dir and D is digit, and T='\0', and x can be any char including '\0')
		} else if (byteValue < 100) {
			strcat(messageFileName, temp_zero); 
			itoa(byteValue, temp_filename, 10); 	// write two digits into next positions ( messageFileName now is "/M/0DDTxxxx" where M is dir and D is digit, and T='\0', and x can be any char including '\0')
		} else {
			itoa(byteValue, temp_filename, 10); 	// write three digits into next positions ( messageFileName now is "/M/DDDTxxxx" where M is dir and D is digit, and T='\0', and x can be any char including '\0')
		}
		strcat(messageFileName, temp_filename); 
		// now add file-suffix 
		strcat(messageFileName, SUFFIX_PCM_FILES); 
		// messageFileName now is "/M/DDD.WAVT" where M is dir and D is digit, ".WAV" is just that and T='\0'
		#if defined (debug)
	        Serial.print(F("kimubo INFO created message filename "));
	        Serial.println(messageFileName);   
	    #endif
		return messageFileName;
		
	} // getMessageFilenameFromByteValue()


	/* =========================================================== */
	void playMessage(const byte* theMessage, const byte theMessageLength){
		playMessage(theMessage, theMessageLength, true);
	}

	/* =========================================================== */
	void playMessage(const byte* theMessage, const byte theMessageLength, const bool fromProgMem){

		byte i;
		byte single_message;
		for ( i=0; i < theMessageLength; i++){
			if (fromProgMem) {
				single_message = pgm_read_byte(theMessage+i);
			} else {
				single_message = theMessage[i];
			}
			#if defined (debug)
		        Serial.print(F("kimubo INFO playMessage "));
		        Serial.print((int) theMessage);   
		        Serial.print(F(" message #"));
		        Serial.print(i); 
		        Serial.print(F(" which is ")); 
		        Serial.println((byte) single_message);
		    #endif
			
			getMessageFilenameFromByteValue(single_message);
			player.play(messageFileName);
			// wait for finishing playing this message-part:
			while (lkpcm_isPlaying) {
				;
			}
			lkpcm_isFinishedPlayingFile = false; // manually reset this as there is no "next file" in message-part...
		} // for
		
	} // playMessage()
	
// EOF
 
