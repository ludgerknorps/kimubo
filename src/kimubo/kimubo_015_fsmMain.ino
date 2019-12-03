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
 * Abschnitt finite state machine smMain
 */
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions

    // see 010_globals
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smMain_state_init(NULL, &smMain_state_init_onState, NULL);
		State smMain_state_idle(&smMain_state_idle_onEnter, NULL, NULL);
		State smMain_state_playWav(&smMain_state_playWav_onEnter, NULL, NULL);
		State smMain_state_pause(&smMain_state_pause_onEnter, NULL, NULL);
		State smMain_state_seek(&smMain_state_seek_onEnter, NULL, NULL);
		State smMain_state_seekBack(&smMain_state_seekBack_onEnter, NULL, NULL);
		State smMain_state_playMessage(&smMain_state_playMessage_onEnter, NULL, NULL);
		State smMain_state_powerSave(&smMain_state_powerSave_onEnter, NULL, &smMain_state_powerSave_onExit);



    Fsm smMain(&smMain_state_init);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
		// see kimubo.h
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		
    // see 010_globals
		

		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. long definition of all callback functions
		
		/* =========================================================== */
		// called once at device startup
		void smMain_state_init_onState(){
				#if defined (debug)
            Serial.println(F("smMain STATE init"));
        #endif
				
				if (  sdc_setup() &&
				      keyb_setup() &&
              player_setup()
				   ){
            #if defined (debug)
                Serial.println(F("smMain STATE init I'm triggering..."));
            #endif
            smMain.trigger(smMain_event_init_completed); 
				} else {
            #if defined (debug)
                Serial.println(F("smMain STATE init ERROR! "));
            #endif
            while (1); // do endless...
				}
		}
		
		
		/* =========================================================== */
		void smMain_state_idle_onEnter(){
			// tbd.
				#if defined (debug)
          Serial.println(F("smMain ENTERED STATE idle"));
        #endif
        
        player_stop();
		}

    /* =========================================================== */
    void smMain_state_playWav_onEnter(){
      // tbd.
        #if defined (debug)
          Serial.println(F("smMain ENTERED STATE playWav"));
        #endif

        // in one of the transition actions (or even before them) we started playing, if everything went right and there is a (potentially next) file to play.
        // so right now, we should be playing.
        // if not, then there was no (next) file --> we now need to switch to state idle as we are stopped.
        if ( ! player_isPlaying ){
            smMain.trigger(smMain_event_stop);
        } else {
            // do nothing, the ISRs are pumping the music out :-)
        }
    }

    /* =========================================================== */
    void smMain_state_pause_onEnter(){
      // tbd.
        #if defined (debug)
          Serial.println(F("smMain ENTERED STATE pause"));
        #endif
    }
		
		/* =========================================================== */
		void smMain_state_seek_onEnter(){
			// tbd.
				#if defined (debug)
          Serial.println(F("smMain ENTERED STATE seek"));
        #endif

		}
		
		/* =========================================================== */
		void smMain_state_seekBack_onEnter(){
			  #if defined (debug)
          Serial.println(F("smMain ENTERED STATE seekBack"));
        #endif
        
		}

    /* =========================================================== */
    void smMain_state_playMessage_onEnter(){
         #if defined (debug)
          Serial.println(F("smMain ENTERED STATE playMessage"));
        #endif

    }

    /* =========================================================== */
    void smMain_state_powerSave_onEnter(){
        #if defined (debug)
          Serial.println(F("smMain ENTERED STATE powerSave"));
        #endif

    }

    /* =========================================================== */
    void smMain_state_powerSave_onExit(){
      // tbd.
        
    }
		
		
		
		
		
		/* =========================================================== */
		void smMain_trans_B1_onTransition(){
		    #if defined (debug)
          Serial.print(F("smMain TRANSITION B1 with Key "));
          Serial.println(keyb_current_playListKey);   
        #endif

        // goto right dir on sdc
        // open first file e.g. "0.WAV"
        // start playing at begin of file 0

        #if defined (debug)
            Serial.print(F("smMain TRANSITION B1 INFO in this playlist player_track_number_min / player_track_number_max is "));
            Serial.print(player_track_number_min[keyb_current_playListKey - '0']); 
            Serial.print(F(" / "));  
            Serial.println(player_track_number_max[keyb_current_playListKey - '0']); 
        #endif

        if ( player_track_number_min[keyb_current_playListKey - '0'] != 255) {
            // there are files in this dir
            
            // remember current playlist
            player_current_playlist_dirname = keyb_current_playListKey;

            // begin with first track 
            player_current_track = player_track_number_min[player_current_playlist_dirname - '0'];

            get_new_track_player_filename();
            #if defined (debug)
                Serial.print(F("smMain TRANSITION B1 INFO starting with file "));
                Serial.println(player_current_track_filename);
            #endif
            
            if ( pf_open(player_current_track_filename) == FR_OK ){
                player_play(player_current_track_filename);
                #if defined (debug)
                    Serial.print(F("smMain TRANSITION B1 INFO playing playlist/file "));
                    Serial.println(player_current_track_filename);   
                #endif
            } else {
                player_stop(); // cannot open file?!
                #if defined (debug)
                    Serial.print(F("smMain TRANSITION B1 ERROR cannot open file? "));
                    Serial.println(player_current_track_filename);   
                #endif
            }
        } else {
            // no file in dir --> do nothing, as player is not playing it will immediatly go back to idle.
            #if defined (debug)
                Serial.print(F("smMain TRANSITION B1 INFO a playlist without files was selected "));
                Serial.println(player_current_playlist_dirname);   
            #endif
        };    
		}
		
		/* =========================================================== */
		void smMain_trans_B2_onTransition(){
				#if defined (debug)
          Serial.println(F("smMain TRANSITION B2"));
        #endif
        // select next track (stay in same dir/playlist)
        if ( player_current_track == player_track_number_max[atoi(player_current_playlist_dirname)] ){
            player_stop(); // there is no further file in this dir
            #if defined (debug)
                Serial.print(F("smMain TRANSITION B2 INFO player stopped after last file in dir "));
                Serial.println(player_current_track_filename);   
            #endif
        } else {
            // ok, there IS another file, but it need not be the sequential next one...
            do {
                player_current_track++;
                get_new_track_player_filename();
            } while (pf_open(player_current_track_filename) != FR_OK);
            player_play(player_current_track_filename);
            #if defined (debug)
                Serial.print(F("smMain TRANSITION B2 INFO playing playlist/file "));
                Serial.println(player_current_track_filename);   
            #endif 
        }
		}
		
		/* =========================================================== */
		void smMain_trans_B3_onTransition(){
			  #if defined (debug)
          Serial.println(F("smMain TRANSITION B3"));
        #endif
        // goto right dir on sdc
        // open first file e.g. "0.WAV" 
        // start playing at begin of file 0

        // difference to transition B1 (idle -> play):
        // if there are no files in new selected playlist, we continue to play the files from the provious dir/playlist
        
        // only do something, if the pressed key is not belonging to currently played playlist
        if ( keyb_current_playListKey != player_current_playlist_dirname ){

             if ( player_track_number_min[atoi(keyb_current_playListKey)] != 255) {
                  // there are files in this dir
                  
                  // remember current playlist
                  player_current_playlist_dirname = keyb_current_playListKey;
      
                  // begin with first track 
                  player_current_track = player_track_number_min[atoi(player_current_playlist_dirname)];
      
                  get_new_track_player_filename();
                  if ( pf_open(player_current_track_filename) == FR_OK ){
                      player_play(player_current_track_filename);
                  } else {
                      player_stop(); // cannot open file?!
                  }
              } else {
                  // no file in dir --> do nothing, player keeps playing previous playlist/track
                  #if defined (debug)
                      Serial.print(F("smMain TRANSITION B3 INFO a playlist without files was selected "));
                      Serial.println(player_current_playlist_dirname);   
                  #endif
              } 
        } else {
            // we do nothing, just write debug
            #if defined (debug)
                Serial.print(F("smMain TRANSITION B3 INFO current playlist was re-selected: "));
                Serial.println(player_current_playlist_dirname);   
            #endif
        }
		}
		
		
		/* =========================================================== */
		void smMain_trans_D1_onTransition(){
			  #if defined (debug)
          Serial.println(F("smMain TRANSITION D1"));
        #endif
        player_pause();
		}
		
		/* =========================================================== */
		void smMain_trans_D2_onTransition(){
			  #if defined (debug)
          Serial.println(F("smMain TRANSITION D2"));
        #endif
        player_pause();
		}

   /* =========================================================== */
   void smMain_trans_E1_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION E1"));
        #endif
        // select next track (stay in same dir/playlist)
        if ( player_current_track == player_track_number_max[atoi(player_current_playlist_dirname)] ){
            // there is no further file in this dir
            // do nothing, just continue playing this last track 
            // here E1 differs from B2!
            #if defined (debug)
                Serial.print(F("smMain TRANSITION E1 INFO skip requested but already last file in dir "));
                Serial.println(player_current_track_filename);   
            #endif
        } else {
            // ok, there IS another file, but it need not be the sequential next one...
            do {
                player_current_track++;
                get_new_track_player_filename();
            } while (pf_open(player_current_track_filename) != FR_OK);
            player_play(player_current_track_filename);
            #if defined (debug)
                Serial.print(F("smMain TRANSITION E1 INFO playing playlist/file "));
                Serial.println(player_current_track_filename);   
            #endif 
        }
    }
    
    /* =========================================================== */
    void smMain_trans_E2_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION E2"));
        #endif
        // select previous track (stay in same dir/playlist)
        // or go back to begin of current track, if playtime < 1sek.

        if (  sdc_fileSystem.fptr <= ( (player_is16bit ? 2 : 1) * PCM_SAMPLE_RATE) ) {
            // go back to begin of current track
            player_play(player_current_track_filename);
            #if defined (debug)
                Serial.print(F("smMain TRANSITION E2 INFO skipback requested at begin of a track --> go back to begin of current track "));
                Serial.println(player_current_track_filename);   
            #endif
        } else {
            if ( player_current_track == player_track_number_min[atoi(player_current_playlist_dirname)] ){
                // there is no previous file in this dir
                // skip back to beginning of this first track
                player_play(player_current_track_filename);
                #if defined (debug)
                    Serial.print(F("smMain TRANSITION E2 INFO skipback requested at first track in playlist --> go back to begin of this first track "));
                    Serial.println(player_current_track_filename);   
                #endif
            } else {
                // ok, there IS a privious file, but it need not be the sequential next one...
                do {
                    player_current_track--;
                    get_new_track_player_filename();
                } while (pf_open(player_current_track_filename) != FR_OK);
                player_play(player_current_track_filename);
                #if defined (debug)
                    Serial.print(F("smMain TRANSITION E2 INFO playing playlist/file "));
                    Serial.println(player_current_track_filename);   
                #endif 
                
            }
        }
        
    }

    /* =========================================================== */
    void smMain_trans_F2_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION F2"));
        #endif
        player_pause();
    }
    
    /* =========================================================== */
    void smMain_trans_F5_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION F5"));
        #endif
        player_pause();
    }

    /* =========================================================== */
    void smMain_trans_G1_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G1"));
        #endif
        player_pause();
    }
    
    /* =========================================================== */
    void smMain_trans_G2_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G2"));
        #endif
        player_pause();
    }

    /* =========================================================== */
    void smMain_trans_G4_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G4"));
        #endif
        player_pause();
    }
    
    /* =========================================================== */
    void smMain_trans_G5_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G5"));
        #endif
        player_pause();
    }

    /* =========================================================== */
    void smMain_trans_G6_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G6"));
        #endif
        player_pause();
    }
    
    /* =========================================================== */
    void smMain_trans_G7_onTransition(){
        #if defined (debug)
          Serial.println(F("smMain TRANSITION G7"));
        #endif
        player_pause();
    }


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 5. init function for smMain
    bool smMain_init_stateMachine(){
      // initialize the whole statemachine, add transitions and so on...

      #if defined (debug)
        Serial.print(F("FSM INFO created state smMain_state_init : "));
        Serial.println( (long) &smMain_state_init , HEX);
        Serial.print(F("FSM INFO created state smMain_state_idle : "));
        Serial.println( (long) &smMain_state_idle , HEX);
        Serial.print(F("FSM INFO created state smMain_state_playWav : "));
        Serial.println( (long) &smMain_state_playWav , HEX);
        Serial.print(F("FSM INFO created state smMain_state_pause : "));
        Serial.println( (long) &smMain_state_pause , HEX);
        Serial.print(F("FSM INFO created state smMain_state_seek : "));
        Serial.println( (long) &smMain_state_seek , HEX);
        Serial.print(F("FSM INFO created state smMain_state_seekBack : "));
        Serial.println( (long) &smMain_state_seekBack , HEX);
        Serial.print(F("FSM INFO created state smMain_state_playMessage : "));
        Serial.println( (long) &smMain_state_playMessage , HEX);
        Serial.print(F("FSM INFO created state smMain_state_powerSave : "));
        Serial.println( (long) &smMain_state_powerSave , HEX);
    #endif
      
      // A1 transition init to idle
      // we are ready but not doing anything yet
        smMain.add_transition(  &smMain_state_init, 
                  &smMain_state_idle, 
                  smMain_event_init_completed, 
                  NULL);
                  
      // --------- PLAYING / play a track and at end of track continue with next track ---------
      
      // B1 transitions idle to playWav (normal player, not messages)
      // start playing something
        smMain.add_transition(  &smMain_state_idle,  
                  &smMain_state_playWav,  
                  smMain_event_playWav,  
                  &smMain_trans_B1_onTransition);
      
      // B2 transitions playWav to playWav
      // we have finished to play something and we now want to continue with next track
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_playWav,  
                  smMain_event_playNextWav,  
                  &smMain_trans_B2_onTransition);
                  
      // B3 transitions playWav to playWav
      // we are playing and user wants to play other playlist
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_playWav,  
                  smMain_event_playOtherPlaylist,  
                  &smMain_trans_B3_onTransition);
                  
      // --------- STOPPING / stop to play ---------
                  
      // C1 transitions playWav to idle
      // we are playing or have finished to play and now want to stop playing
      // this transition is also used by sleepTimer (in order to end playing and later go to low power mode)
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_idle,  
                  smMain_event_stop,  
                  NULL);
                  
      // C2 transitions pause to idle
      // we have paused and now want to stop playing
      // this transition is ONLY used by sleepTimer (in order to end playing and later go to low power mode)
        smMain.add_transition(  &smMain_state_pause,  
                  &smMain_state_idle,  
                  smMain_event_stop,  
                  NULL);
      
      // --------- PAUSING / halt playing and later continue playing seamlessless --------------
      
      // D1 transitions playWav to pause
      // we are playing something and want to pause
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_pause,  
                  smMain_event_pause,  
                  &smMain_trans_D1_onTransition);
                
      // D2 transitions pause to playWav
      // we have paused and now want to resume (aka. continue to play)
        smMain.add_transition(  &smMain_state_pause,  
                  &smMain_state_playWav,  
                  smMain_event_resume,  
                  &smMain_trans_D2_onTransition);
                  
      // no extra transition for events 
      //            smMain_event_playOtherPlaylist
      //            smMain_event_skip
      //            smMain_event_skipBack
      //            smMain_event_seek
      //            smMain_event_seekBack
      // if user presses one of those buttons in pause, nothing happens at all!
                  
      // -------- SKIPPING / Going to next or previous track by pressing buttons ---------------
                  
      // E1 transitions playWav to playWav
      // we are playing something and want to skip (to next track)
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_playWav,  
                  smMain_event_skip,  
                  &smMain_trans_E1_onTransition);                     
                  
        // E2 transitions playWav to playWav
      // we are playing something and want to skipBack (to previous track)
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_playWav,  
                  smMain_event_skipBack,  
                  &smMain_trans_E2_onTransition); 
      
      // --------- SEEKING / Going fast-forward or fast-backward in track -----------------------
                  
      // F1 transitions playWav to seek
      // we are playing something and want to seek (forward)
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_seek,  
                  smMain_event_seek,  
                  NULL);  
      
      // F2 transitions seek to seek
      // hop forward while seeking
        smMain.add_timed_transition(  &smMain_state_seek,  
                      &smMain_state_seek,  
                      SEEK_DURATION_PLAYING,  
                      &smMain_trans_F2_onTransition); 
                  
      // F3 transitions seek to playWav
      // we were seeking and now return to play
        smMain.add_transition(  &smMain_state_seek,  
                  &smMain_state_playWav,  
                  smMain_event_continue,  
                  NULL);  
      
      // F4 transitions playWav to seekBack
      // we are playing something and want to seek (backward)
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_seekBack,  
                  smMain_event_seekBack,  
                  NULL);
      
      // F5 transitions seek to seek
      // hop backward while seeking backwards
        smMain.add_timed_transition(  &smMain_state_seek,  
                      &smMain_state_seek,  
                      SEEK_DURATION_PLAYING,  
                      &smMain_trans_F5_onTransition); 
                        
      // F6 transitions seekBack to playWav
      // we were seeking and now return to play
        smMain.add_transition(  &smMain_state_seekBack,  
                  &smMain_state_playWav,  
                  smMain_event_continue,  
                  NULL);
      
      // --------- PLAYMESSAGES / play a message and if necessary interrupt playing normal wav for that --------------
      
      // G1 transitions idle to playMessage
      // we are not doing anything and now and want to play a message
        smMain.add_transition(  &smMain_state_idle,  
                  &smMain_state_playMessage,  
                  smMain_event_playMessage,  
                  &smMain_trans_G1_onTransition); 
                  
      // G2 transitions playMessage to playMessage
      // we are playing a message, finished with one part of the message and now want to play the next part
        smMain.add_transition(  &smMain_state_playMessage,  
                  &smMain_state_playMessage,  
                  smMain_event_playNextMessagePart,  
                  &smMain_trans_G2_onTransition);           
      
      // G3 transitions playMessage to idle
      // we finished playing a message and return to do nothing
        smMain.add_transition(  &smMain_state_playMessage,  
                  &smMain_state_idle,  
                  smMain_event_returnToIdle,  
                  NULL);  
      
      // G4 transitions playWav to playMessage
      // we are playing a normal wav and now want to interupt that for a message
        smMain.add_transition(  &smMain_state_playWav,  
                  &smMain_state_playMessage,  
                  smMain_event_playMessage,  
                  &smMain_trans_G4_onTransition);           
      
      // G5 transitions playMessage to playWav
      // we finished playing a message and return to playing normally
        smMain.add_transition(  &smMain_state_playMessage,  
                  &smMain_state_playWav,  
                  smMain_event_continue,  
                  &smMain_trans_G5_onTransition); 
                  
      // G6 transitions pause to playMessage
      // we are pausing a normal wav and now want to interupt that for a message
        smMain.add_transition(  &smMain_state_pause,  
                  &smMain_state_playMessage,  
                  smMain_event_playMessage,  
                  &smMain_trans_G6_onTransition);           
      
      // G7 transitions playMessage to pause
      // we finished playing a message and return to pausing 
        smMain.add_transition(  &smMain_state_playMessage,  
                  &smMain_state_pause,  
                  smMain_event_returnToPause,  
                  &smMain_trans_G7_onTransition);           
      
      // --------- POWERSAVE / send device to low power mode and wake up again --------------
      
      // H1 transitions idle to powerSave
      // we are doing nothing and now want to save power
        smMain.add_timed_transition(  &smMain_state_idle,  
                      &smMain_state_powerSave,  
                      POWERSAVE_AFTER,  
                      NULL);
                      
      // H2 transitions powerSave to idle
      // recover from low power state
        smMain.add_transition(  &smMain_state_powerSave,  
                  &smMain_state_idle,  
                  smMain_event_wakeUpToIdle,  
                  NULL);  
      
      // H3 transitions pause to powerSave
      // we are pausing (for quite some time!) and now want to save power
        smMain.add_timed_transition(  &smMain_state_pause,  
                      &smMain_state_powerSave,  
                      POWERSAVE_AFTER_PAUSE,  
                      NULL);  
      
      // H4 transitions powerSave to pause
      // recover from low power state
        smMain.add_transition(  &smMain_state_powerSave,  
                  &smMain_state_pause,  
                  smMain_event_wakeUpToPause,  
                  NULL);
                  
      // no need for a transition from playWav to powerSave:
      // sleepTimer does not directly set powerSave-state but instead
      // stops playing - after which the normal H1 transition is used to 
      // put device into low power state after a certain time...

      smMain.printTransitionTable();

      #if defined (debug)
          Serial.println(F("smMain smMain_init_stateMachine completed!"));
      #endif

      return 1; // no error

      
                            
    } // smMain_init_stateMachine()
	
