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

    // default values for our variables
    
    player_previous_track_number = 255;
    player_next_track_number = 255;

    player.setupPlayer(AUDIO_PIN_OUT_HIGH, AUDIO_PIN_OUT_LOW);

    return true; // nothing to do here yet
}

 // ####################################################################################
 // calculates the new track's filename
    void get_new_track_player_filename(byte curFileNameAsByte, char curPlaylistName, char* trackFilename){
    
//        #if defined (debug)
//            Serial.print(F("get_new_track_player_filename "));
//            Serial.print(curFileNameAsByte);
//            Serial.print(F(" and dir "));
//            Serial.print(curPlaylistName);
//        #endif
       
       char temp_filename[4];
      
       if ( curFileNameAsByte == 255 ){
            strcpy(trackFilename, " "); // default
       } else {
//            itoa(curFileNameAsByte, temp_filename, 10); // 10 = convert using decimal system
//          
//            strcat(temp_filename, SUFFIX_PCM_FILES);
//    
//            // a short array to hold the playlist dirname 
//            char s[4] = "/ /"; // initialize 
//            s[1] = curPlaylistName; // overwrite
//            strcpy(trackFilename, s);
//            strcat(trackFilename, temp_filename);
//    
//            #if defined (debug)
//                Serial.print(F(" : "));
//                Serial.println(trackFilename);
//            #endif

            strcpy(trackFilename, "/ /");
            trackFilename[1] = curPlaylistName; // overwrite
            itoa(curFileNameAsByte, temp_filename, 10); // 10 = convert using decimal system
            strcat(trackFilename, temp_filename);
            strcat(trackFilename, SUFFIX_PCM_FILES);

//            #if defined (debug)
//                Serial.print(F(" : "));
//                Serial.println(trackFilename);
//            #endif
       }
    } // get_new_track_player_filename()


// ####################################################################################
// while playing each track we spend some time on finding the previous track and the next track -> this will very much speed up continuous playback of whole playlist with all its tracks.
    void find_next_and_previous_files_in_current_playlist(){
  
        byte tempTrackNumber;
        char tempFileName[sizeof(player_current_track_filename)];

        #if defined (debug)
            Serial.print(F("kimubo INFO looking for previous/next tracks in dir : ")); 
            Serial.println(player_current_playlist_dirname);  
        #endif
        
        // player_current_track holds number of currently played track (as integer)
        // player_current_playlist_dirname holds current dir name
        
        // first, find previous track
        if ( player_current_track == player_track_number_min[player_current_playlist_dirname - '0'] ) {
            player_previous_track_number = 255; // 255 says "this is first track"
            #if defined (debug)
                Serial.print(F("kimubo INFO previous file : ")); 
                Serial.println(player_previous_track_number);  
                delay(100);
            #endif
        } else {
            for ( tempTrackNumber = player_current_track-1 ; tempTrackNumber >= player_track_number_min[player_current_playlist_dirname - '0'] ; tempTrackNumber-- ) {
                get_new_track_player_filename(tempTrackNumber, player_current_playlist_dirname, tempFileName);
                if ( sd.exists(tempFileName)) {
                    #if defined (debug)
                        Serial.print(F("kimubo INFO previous file : ")); 
                        Serial.println(tempTrackNumber);  
                    #endif
                    player_previous_track_number = tempTrackNumber;
                    break; // do not continue with for loop
                }
            } // for
        }
        
        // now find next track
        if ( player_current_track == player_track_number_max[player_current_playlist_dirname - '0'] ) {
            player_next_track_number = 255; // 255 says "this is last track"
            #if defined (debug)
                Serial.print(F("kimubo INFO next file : ")); 
                Serial.println(player_next_track_number);  
            #endif
        } else {
            for ( tempTrackNumber = player_current_track+1 ; tempTrackNumber <= player_track_number_max[player_current_playlist_dirname - '0'] ; tempTrackNumber++ ) {
                get_new_track_player_filename(tempTrackNumber, player_current_playlist_dirname, tempFileName);
                if ( sd.exists(tempFileName)) {
                    #if defined (debug)
                        Serial.print(F("kimubo INFO next file : ")); 
                        Serial.println(tempFileName);  
                    #endif
                    player_next_track_number = tempTrackNumber;
                    break; // do not continue with for loop
                }
            } // for
        } 
        
    } // find_next_and_previous_files_in_current_playlist

 

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

        SdBaseFile nextFile;
        byte i = player_current_track;

        if ( lkpcm_isStopped ) {
            return; // do nothing, if player is not playing...
        }
        
        #if defined (debug)
          Serial.print(F("trans_B2_play_next_track_in_playlist "));
          Serial.println(keyb_current_playListKey);   
          Serial.println(player_current_track);
          Serial.println( player_track_number_max[player_current_playlist_dirname - '0']);
        #endif

        if ( i == player_track_number_max[player_current_playlist_dirname - '0'] ) {
            // we had already readched last track 
            player.stopPlayback(); 
            return;
           
        } else {
            for (;;){
                i++;
                get_new_track_player_filename(i, player_current_playlist_dirname, player_current_track_filename);
                Serial.println(F("TRying..."));   
                Serial.println(i);
                Serial.println(player_current_track_filename);   
                if (nextFile.open(player_current_track_filename, O_RDONLY )){
                    player_current_track = i;
                    nextFile.close();
                    break;
                }

                if (i == player_track_number_max[player_current_playlist_dirname - '0']) {
                    player_current_track = i;
                    break;
                }
            } 
            
            nextFile.close();
            
            if (player_current_track > player_track_number_max[player_current_playlist_dirname - '0']) {
                #if defined (debug)
                    Serial.print(F("kimubo ERROR no player_track_number_max?? "));
                    Serial.println(player_track_number_max[player_current_playlist_dirname - '0']);   
                #endif
            } else {
                // we found a next file
                #if defined (debug)
                    Serial.print(F("kimubo INFO playing net track "));
                    Serial.println(player_current_track_filename);   
                #endif
    
                player.play(player_current_track_filename);
            }
            
            
        }
   }
        


// EOF
