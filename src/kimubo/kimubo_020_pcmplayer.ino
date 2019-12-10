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
 // calculates the new track's filename
    void get_new_track_player_filename(){
    
        #if defined (debug)
            Serial.print(F("get_new_track_player_filename "));
            Serial.print(player_current_track);
            Serial.print(F(" : "));
            Serial.print(sizeof(player_current_track_filename));
        #endif
       
       char temp_filename[sizeof(player_current_track_filename)];
      
       if ( player_current_track == 255 ){
            strcpy(player_current_track_filename, " "); // default
       } else {
            itoa(player_current_track, temp_filename, 10); // 10 = convert using decimal system
          
            strcat(temp_filename, SUFFIX_PCM_FILES);
    
            // a short array to hold the playlist dirname 
            char s[4] = "/ /"; // initialize 
            s[1] = player_current_playlist_dirname; // overwrite
            strcpy(player_current_track_filename, s);
            strcat(player_current_track_filename, temp_filename);
    
            #if defined (debug)
                Serial.print(F(" : "));
                Serial.println(player_current_track_filename);
                delay(100);
            #endif
       }
    } // get_new_track_player_filename()

 

/* =========================================================== */
    void trans_B1_play_playlist_from_first_track(){
        #if defined (debug)
          Serial.print(F("trans_B1_play_playlist_from_first_track "));
          Serial.println(keyb_current_playListKey);   
        #endif

        // goto right dir on sdc
        // open first file e.g. "0.WAV"
        // start playing at begin of file 0

        if ( player_track_number_min[keyb_current_playListKey - '0'] != 255) {
            // there are files in this dir
            
            // remember current playlist
            player_current_playlist_dirname = keyb_current_playListKey;

            // begin with first track 
            player_current_track = player_track_number_min[player_current_playlist_dirname - '0'];
            get_new_track_player_filename();
            player.play(player_current_track_filename);
        } else {
            // no file in dir --> do nothing, and just for good measurement: stop player (it should be stopped already!)
            player.stopPlayback();  
            #if defined (debug)
                Serial.print(F("kimubo INFO a playlist without files was selected "));
                Serial.println(player_current_playlist_dirname);   
            #endif
        };    
    }



// EOF
