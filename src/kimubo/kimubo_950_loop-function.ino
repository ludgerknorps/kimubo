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



	void loop() {
    
		// check if something is happening on the keypad (e.g. key pressed released)
		keypad.getKey(); // we only need to check for one key as we do not use multikey 

    // check loudness switch
    if ( player_is_loudness != digitalRead(AUDIO_PIN_LOUDNESS ) ) {
        toggleLoudness();
    }

    // if playing a playlist and we finished a track, continue with next track, iff there is one
    if ( lkpcm_isFinishedPlayingFile ) {
        trans_B2_play_next_track_in_playlist();
        //trans_E1_skip_forward();
    }


    #if defined (AUTO_PLAY)
        if (isFirstLoop) {
            isFirstLoop = false;
            play_last_playlist_and_track_from_eeprom();
        }
    #endif


    // if playing stopped, go to powersave mode after configurable time
    if ( lkpcm_isStopped ) {
    	if ( playerStoppedSince == 0L ){
    		playerStoppedSince = millis(); 
    	} else {
    		if ( millis() - playerStoppedSince > POWERSAVE_IF_STOP_AFTER ){
    			// save some mAh from the battery and go to deep sleep 
    			// KIMUBO will only recover via Power-Off-Power-On

				#if defined (debug)
					Serial.print(F("INFO KIMUBO SHUTDOWN BECAUSE OF INACTIVITY after waiting for [ms] "));
					Serial.println(POWERSAVE_IF_STOP_AFTER);
					delay(100); // wait for Serial message to finish...
				#endif
				amp_shutdown();
				
				set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
				sleep_enable();  
				sleep_mode();
				// now it sleeps... and draws almost no current...
				
    		}
    	}
    } else {
    	// if player is not stopped, reset playerStoppedSince
    	playerStoppedSince = 0L;
    }

 



	} // loop()	 
	
// EOF
