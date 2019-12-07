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
 * Abschnitt LOOP
 */

 byte inByte;

	void loop() {

    if ( millis() - myTime > 3000 ){
         myTime = millis();

//        #if defined (debug)
//            Serial.print(F("LOOP  INFO ; 004 FREE RAM "));
//            Serial.println(freeRam());    
//            Serial.print(F("LOOP  INFO ; Buffer readPositionInBuffer "));
//            Serial.println( readPositionInBuffer ); 
//            Serial.print(F("LOOP  INFO ; Buffer player_currentReadBuffer "));
//            Serial.println( player_currentReadBuffer ); 
//            Serial.print(F("LOOP  INFO ; Buffer player_currentWriteBuffer"));
//            Serial.println( player_currentWriteBuffer ); 
//            Serial.print(F("LOOP  INFO ; SDC FPTR "));
//            Serial.println(sdc_fileSystem.fptr ); 
//            //check_mem();      
////            Serial.print(F("FSM   INFO ; 005 Stackpointer / Heappointer "));      
////            Serial.print( (int) SP, HEX);       
////            Serial.print(F(" / "));       
////            Serial.println( (int) (SPH << 8 | SPL), HEX);
//        #endif
    }
    
		// check if something is happening on the keypad (e.g. key pressed released)
		keypad.getKey(); // we only need to check for one key as we do not use multikey - but the whole eventListener-thing in keypad works with "getKeys()".
		// tbd. define MAX_KEYS 1!

//    if (Serial.available() > 0) {
//        // get incoming byte:
//        inByte = Serial.read();
//        simulatedKeypad_release_key_after_shorthold(inByte);
//    }

    // fsmMain
    smMain.run_machine();


	} // loop()	 
	
// EOF
