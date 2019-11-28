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
 


bool sdc_setup(){

    char c; // temporary var.
    byte i; // temporary var.
    byte j; // temporary var.
    bool minTrackNumberSet;
  
  	// Initialize SDcard
  	// Try a lower speed if SPI errors occur.
  	if (!sdc_fileSystem.begin(SDC_PIN_CS, SDC_SPI_SPEED )) {
  		sdc_fileSystem.initErrorPrint();
  		return false;
  	}
  	
  	// set root as current working directory
  	if (!sdc_fileSystem.chdir("/")) {
  		sdc_fileSystem.errorPrint(F("Failed to go to SDC root dir"));
  		return false;
  	}
  
  	// in our root directory there must exist subdirs '0' to '8' and a subdir SDC_SYSTEM_MESSAGES_DIR
    // if not, create them
  	for (c=KEYSCAN_1 ; c <= KEYSCAN_9 ; c++) {
  		if ( ! sdc_fileSystem.exists(c) ){
  			sdc_fileSystem.mkdir(c);
        #if defined (debug)
            Serial.print(F("sdc_setup WARN had to create new playlist directory "));
            Serial.println(c);
        #endif
  		}
  	}
    if ( ! sdc_fileSystem.exists(SDC_SYSTEM_MESSAGES_DIR) ){
        sdc_fileSystem.mkdir(SDC_SYSTEM_MESSAGES_DIR);
        #if defined (debug)
            Serial.println(F("sdc_setup WARN had to create new system message directory "));
        #endif
    }
  
  
    // for each dir, get track-number-min and -max, see 010_globals for explanations.
    // if no file in dir --> return "255" for both
    // otherwise return number of "lowest" and the "highest" file in dir
    i = 0;
    for (c=KEYSCAN_1 ; c <= KEYSCAN_9 ; c++) {
        // goto relevant dir
        sdc_fileSystem.chdir(keyb_current_playListKey);
        // reset variables
        player_track_number_min[i] = 255;
        player_track_number_max[i] = 255; 
        minTrackNumberSet = false;
        
        for (j=0; j<=254; j++){
            // ok this is a hack: we are using the global variable player_current_track_filename allthough at this stage (init of machine) we don'T have any player yet.
            // But SRAM is expensive on an arduino...
            itoa(j, player_current_track_filename, 10); // 10 = convert using decimal system
            strcat(player_current_track_filename, SUFFIX_PCM_FILES);
            if ( sdc_fileSystem.exists(player_current_track_filename) ){
                if ( ! minTrackNumberSet ) {
                    player_track_number_min[i] = j;
                    minTrackNumberSet = true;
                }
                player_track_number_max[i] = j;
            }
        }
        #if defined (debug)
            Serial.print(F("sdc_setup INFO player_track_number_min and max for playlist "));
            Serial.print(c);
            Serial.print(F(" are "));
            Serial.print(player_track_number_min[i]);
            Serial.print(F(" and "));
            Serial.println(player_track_number_max[i]);
        #endif
        i++;
        
    }

    player_current_track_filename[0] = '\0'; // reset the misused variable...
  
}
