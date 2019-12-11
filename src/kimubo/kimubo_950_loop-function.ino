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

  //byte inByte; // needed for simulated keypress via Serial.read()

	void loop() {
    
		// check if something is happening on the keypad (e.g. key pressed released)
		keypad.getKey(); // we only need to check for one key as we do not use multikey 

    // if playing a playlist and we finished a track, continue with next track, iff there is one
    if ( lkpcm_isFinishedPlayingFile ) {
        trans_B2_play_next_track_in_playlist();
    }

//    if (Serial.available() > 0) {
//        // get incoming byte:
//        inByte = Serial.read();
//        DEBUG_simulatedKeypad_release_key_after_shorthold(inByte);
//    }




	} // loop()	 
	
// EOF
