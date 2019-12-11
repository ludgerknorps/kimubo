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

//
bool player_setup(){

    player.setupPlayer(AUDIO_PIN_OUT_HIGH, AUDIO_PIN_OUT_LOW);

    return true; // nothing to do here yet
}

 // ####################################################################################
 // calculates the new track's filename
    void get_new_track_player_filename(const byte curFileNameAsByte, const char curPlaylistName, char* trackFilename){
       char temp_filename[4];
      
       if ( curFileNameAsByte == 255 ){
            strcpy(trackFilename, " "); // default
       } else {
            strcpy(trackFilename, "/ /");
            trackFilename[1] = curPlaylistName; // overwrite
            itoa(curFileNameAsByte, temp_filename, 10); // 10 = convert using decimal system
            strcat(trackFilename, temp_filename);
            strcat(trackFilename, SUFFIX_PCM_FILES);

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
            get_new_track_player_filename(player_current_track, player_current_playlist_dirname, player_current_track_filename);
            player.play(player_current_track_filename);

            // as we just started playback, now is a very good time to spend some CPU-cycles on finding the next track.
            // this will very much speed up continuous playback of whole playlist with all its tracks.
            //find_next_and_previous_files_in_current_playlist(); 
            
        } else {
            // no file in dir --> do nothing, and just for good measurement: stop player (it should be stopped already!)
            player.stopPlayback();  
            #if defined (debug)
                Serial.print(F("kimubo INFO a playlist without files was selected "));
                Serial.println(player_current_playlist_dirname);   
            #endif
        }; 
        
    }

/* =========================================================== */
    void trans_B2_play_next_track_in_playlist(){

        if ( lkpcm_isStopped ) {
            return; // do nothing, if player is not playing...
        }
        
        #if defined (debug)
          Serial.print(F("trans_B2_play_next_track_in_playlist "));
          Serial.println(keyb_current_playListKey);   
          Serial.println(player_current_track);
          Serial.println( player_track_number_max[player_current_playlist_dirname - '0']);
        #endif

        if ( player_current_track == player_track_number_max[player_current_playlist_dirname - '0'] ) {
            // we had already readched last track 
            #if defined (debug)
                Serial.print(F("kimubo INFO autostop at end of playlist "));
                Serial.println(player_current_track_filename);   
            #endif 
            player.stopPlayback(); 
            return;
           
        } else {
            player_current_track++;
            get_new_track_player_filename(player_current_track, player_current_playlist_dirname, player_current_track_filename);

            #if defined (debug)
                Serial.print(F("kimubo INFO playing next track "));
                Serial.println(player_current_track_filename);   
            #endif

            player.play(player_current_track_filename);
            return;
        }
   } // trans_B2_play_next_track_in_playlist


/* =========================================================== */
    void trans_E1_skip_forward(){

        if ( lkpcm_isStopped ) {
            return; // do nothing, if player is not playing...
        }
        
        #if defined (debug)
          Serial.print(F("trans_E1_skip_forward "));
          Serial.println(keyb_current_playListKey);   
          Serial.println(player_current_track);
          Serial.println( player_track_number_max[player_current_playlist_dirname - '0']);
        #endif

        if ( player_current_track == player_track_number_max[player_current_playlist_dirname - '0'] ) {
            // we had already readched last track 
            // as we are skipping on command of user, do nothing, there is no next file 
            #if defined (debug)
                Serial.print(F("kimubo INFO cannot skip to next track, last track reached "));
                Serial.println(player_current_track_filename);   
            #endif 
            return;
           
        } else {
            player_current_track++;
            get_new_track_player_filename(player_current_track, player_current_playlist_dirname, player_current_track_filename);

            #if defined (debug)
                Serial.print(F("kimubo INFO skipping to next track "));
                Serial.println(player_current_track_filename);   
            #endif

            player.play(player_current_track_filename);
            return;
        }
   } // trans_E1_skip_forward


/* =========================================================== */
    void trans_E2_skip_backward(){

        if ( lkpcm_isStopped ) {
            return; // do nothing, if player is not playing...
        }
        
        #if defined (debug)
          Serial.print(F("trans_E2_skip_backward "));
          Serial.println(keyb_current_playListKey);   
          Serial.println(player_current_track);
          Serial.println( player_track_number_min[player_current_playlist_dirname - '0']);
        #endif

        if ( player_current_track == player_track_number_min[player_current_playlist_dirname - '0'] ) {
            // we had already readched first track 
            // as we are skipping back on command of user, just start from begin of file again
            #if defined (debug)
                Serial.print(F("kimubo INFO cannot skip to previous track, first track reached and is now replayed"));
                Serial.println(player_current_track_filename);   
            #endif
            player.play(player_current_track_filename);
            return;
           
        } else {
            //  if current file is playing less then x ms then skip to previous file
            if ( lkpcm_isAtBeginOfFile ) {
                player_current_track--;
                get_new_track_player_filename(player_current_track, player_current_playlist_dirname, player_current_track_filename);
    
                #if defined (debug)
                    Serial.print(F("kimubo INFO skipping to previous track "));
                    Serial.println(player_current_track_filename);   
                #endif
                player.play(player_current_track_filename);
                return;
            } else {
                // we just skip back to begin of current file
                #if defined (debug)
                    Serial.print(F("kimubo INFO skipping back to begin of current track"));
                    Serial.println(player_current_track_filename);   
                #endif
                player.play(player_current_track_filename);
                return;
            }
        }
   }
        


// EOF
