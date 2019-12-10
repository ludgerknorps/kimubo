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

    char c;
    byte i, j;
    char filenumber[4];

    #if defined (debug)
        Serial.println(F("sdc_setup INFO started..."));
    #endif

    // (1)
        // Initialize at the highest speed supported by the board that is
        // not over 50 MHz. Try a lower speed if SPI errors occur.
        if (!sd.begin(SDC_PIN_CS, SD_SCK_MHZ(50))) {
            #if defined (debug)
                Serial.println(F("sdc_setup ERROR sd.begin failed (1)"));  
            #endif
            return false;   // don't do anything more if not
        
        }
        
        #if defined (debug)
            Serial.println(F("sdc_setup INFO sd.begin ok (1)"));
        #endif   

    // (2)
        // in our root directory there must exist subdirs '0' to '8' and a subdir SDC_SYSTEM_MESSAGES_DIR
        // there can also exist others, but they will be ignored

        if (!sd.chdir("/")) {
            #if defined (debug)
                Serial.println(F("sdc_setup ERROR sd.chdir '/' failed (2)"));  
            #endif
            return false;   // don't do anything more if not
        }

        #if defined (debug)
            Serial.println(F("sdc_setup INFO sd.chdir '/' ok (2)"));
        #endif 

        // now check if each of the subdirs exist
        player_current_track_filename[0] = '/'; // "misuse" this global variable, actually this has (yet) nothing to do with the player, we just want to save some RAM
        player_current_track_filename[2] = '\0';
        for ( c = KEYSCAN_1 ; c <= KEYSCAN_9 ; c++ ){
            player_current_track_filename[1] = c;
            if (!sd.exists(player_current_track_filename) ) {
                #if defined (debug)
                    Serial.print(F("sdc_setup ERROR sd.exists failed (2) : ")); 
                    Serial.println(player_current_track_filename);  
                #endif
                return false;   // don't do anything more if not
            } else {
                #if defined (debug)
                    Serial.print(F("sdc_setup INFO sd.exists ok (2) : "));
                    Serial.println(player_current_track_filename); 
                #endif 
            }
        }

        player_current_track_filename[1] = '\0';
        strcat(player_current_track_filename, SDC_SYSTEM_MESSAGES_DIR);
        if (!sd.exists(player_current_track_filename) ) {
            #if defined (debug)
                Serial.print(F("sdc_setup ERROR sd.exists failed (2) : ")); 
                Serial.println(player_current_track_filename);  
            #endif
            return false;   // don't do anything more if not
        } else {
            #if defined (debug)
                Serial.print(F("sdc_setup INFO sd.exists ok (2) : "));
                Serial.println(player_current_track_filename); 
            #endif 
        }


    // (3)
        // for each dir, get track-number-min and -max, see 010_globals for explanations.
        // if no file in dir --> return "255" for both
        // otherwise return number of "lowest" and the "highest" file in dir
        // i is the number of the dir we are in
        // j is used as another counter for the filenumbers in this dir
        i = 0;
        j = 0;

        for (c = KEYSCAN_1 ; c <= KEYSCAN_9 ; c++) {

            player_current_track_filename[0] = '/';
            player_current_track_filename[1] = c;
            player_current_track_filename[2] = '\0';
            
            // goto relevant dir
            if (!sd.chdir(player_current_track_filename)) {
                #if defined (debug)
                    Serial.print(F("sdc_setup ERROR sd.chdir failed (3) : ")); 
                    Serial.println(player_current_track_filename);  
                #endif
                return false;   // don't do anything more if not
            } else {
                #if defined (debug)
                    Serial.print(F("sdc_setup INFO sd.chdir ok (3) : "));
                    Serial.println(player_current_track_filename); 
                #endif 
            }

            // reset variables
            player_track_number_min[i] = 255;
            player_track_number_max[i] = 255; 

            // so now we scan through this dir and get files with filenames that are to be of format "xxx.WAV"
            // max. 254 such files are allowed
            // no other files are allowed
            // we do not know the sequence in which files are scanned
            for ( j = 0; j<=254; j++ ) {

                // the first two bytes of the player_current_track_filename hold the "/" and DIRname
                // thus we itoa from byte 4 on
                player_current_track_filename[2] = '/';
                player_current_track_filename[3] = '\0';
                itoa( j, filenumber , DEC);
                strcat(player_current_track_filename, filenumber);
                strcat(player_current_track_filename, SUFFIX_PCM_FILES);
              
                if (sd.exists(player_current_track_filename) ) {
                    // only for first file, when player_track_number_min is on initial 255 value
                    if ( player_track_number_min[i] == 255 ) {
                        player_track_number_min[i] = j;
                    }
                    // if a file exists, remember it. As we walk through dir from 0..254, the last remembered file is max!
                    player_track_number_max[i] = j;
                }
            } // for j // next file in this dir please!

            #if defined (debug)
                Serial.print(F("sdc_setup INFO player_track_number_min and max for playlist "));
                Serial.print(c);
                Serial.print(F(" are "));
                Serial.print(player_track_number_min[i]);
                Serial.print(F(" and "));
                Serial.println(player_track_number_max[i]);
            #endif
            
            i++; // i corresponds to c and is incremented each for-loop
        } // for c

    

    #if defined (debug)
        Serial.println(F("sdc_setup INFO finished."));
    #endif

    return true;
  
}

// EOF
