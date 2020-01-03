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
 * Abschnitt MatrixKeyboard
 */

   typedef char KeypadEvent;
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 0. a semi-statemachine for the keys
	
		static char keybPressedKey = NULL; 	// is set to a key that is pressed alone. Used to act only at release of this key, not on pressing it. Also used for acting on hold of key.
		static char keybHeldKey = NULL; 		//used to remember which key was held (to act on its release)

    static char keyb_current_playListKey = ' ';

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. Definition KeyScanCodes
    // see .h file
					
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. Aus wievielen Reihen und Spalten ist Matrix des Keyboards aufgebaut?
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. Which pin has which matrix-keyboard-row/-column?
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. Which Pins are rows, which are columns 
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. damit µC zwischendurch in Powerdown gehen kann, müssen alle Keyboard-Inputs (=Reihen, s.o.) mit je einer Diode an diesen Interrupt-Pin angeschlossen sein; dieser weckt dann den µC auf)
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. Die TastaturBelegung selbst = welche Taste ist wo?
    // see .h file
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 7. keypad settings
    // see .h file
		  
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 8. the keypad/keyboard itself		
		// see 010_globals
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	// 9. the eventListener callback function for different types of keys

    void DEBUG_keypad_callback_onEvent( char key){
        switch (keypad.getState()){
          case PRESSED:
            #if defined (debug)
                Serial.print(F("PRESSED "));
                Serial.println(key);
            #endif
            break; // we don't react on keypressing, only on holding and especially on releasing a key
            
          case HOLD:
            #if defined (debug)
                Serial.print(F("HOLD "));
                Serial.println(key);
            #endif    
            break;
        
          case RELEASED:
            #if defined (debug)
                Serial.print(F("RELEASED "));
                Serial.println(key);
            #endif
            break;
        }
    }
    
	
		void keypad_callback_onEvent( char key){
			

				switch (keypad.getState()){
					case PRESSED:
						keybPressedKey = key;
						break; // we don't react on keypressing, only on holding and especially on releasing a key
						
					case HOLD:
						keybHeldKey = key;
						if ( key ='F' ) {
							// seeking while button is held
							// TBD later release
						} else if ( key ='R') {
							// seeking back while button is held
							// TBD later release
						}
						break;

					case RELEASED:
					
						if (key >= KEYSCAN_1 && key <=KEYSCAN_9) {
              // TBD for later release!     
              //    discrimination between short and long-press-release
              //    right now both lead to playing playlist
              // if (keybHeldKey == NULL) {
							if (true) {       
  								// key released after pressed but not held
  								// play playlist nr. <key> from beginning
  
                  #if defined (debug)
                      Serial.print(F("RELEASED "));
                      Serial.println(key);
                  #endif
  
                  // if 
                  //    a) we are not playing --> play playlist starting from first track
                  //    b) we are already playing this playlist --> then ignore key-event
                  //    c) we are playing other playlist --> play playlist starting from first track
  
                  if ( lkpcm_isStopped ){
                      // case a)
                      // remember what key/playlist we want to play
                      keyb_current_playListKey = key;
                      trans_B1_play_playlist_from_first_track();
                  } else {
                      if ( player_current_playlist_dirname == key ) {
                          //case b)
                          #if defined (debug)
                              Serial.print(F("kimubo INFO current playlist was re-selected (inoring!) : "));
                              Serial.println(player_current_playlist_dirname);   
                          #endif
                      } else {
                          //case c)
                          // remember what key/playlist we want to play
                          keyb_current_playListKey = key;
                          trans_B1_play_playlist_from_first_track();
                      }
                  } // else lkpcm_isStopped
//              } else {
//  								// key released after held
//  								// play playlist nr. <key> from stored position (from EEPROM)
//                  // TBD for later release!
//                  #if defined (debug)
//                      Serial.print(F("RELEASED AFTER HOLD "));
//                      Serial.println(key);
//                  #endif
  						}  // long or short pressed?

              
						} else if (key==KEYSCAN_FFWD) {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to next track in playlist

                #if defined (debug)
                    Serial.print(F("RELEASED "));
                    Serial.println(key);
                #endif
                trans_E1_skip_forward();
                
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else

                #if defined (debug)
                    Serial.print(F("RELEASED AFTER HOLD "));
                    Serial.println(key);
                #endif                
							}
						} else if (key==KEYSCAN_REW) {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to previous track in playlist
                
                #if defined (debug)
                    Serial.print(F("RELEASED "));
                    Serial.println(key);
                #endif                    
                
                trans_E2_skip_backward();
                
                
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else

                #if defined (debug)
                    Serial.print(F("RELEASED AFTER HOLD "));
                    Serial.println(key);
                #endif                                        
							}
						}
						
						// clean up the flags 
						keybPressedKey = NULL;
						keybHeldKey = NULL;
						break; //case RELEASED
            
				} // switch

		} // keypadEvent

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // 10. the keypad setup function
      bool keyb_setup(){

          keypad.setHoldTime(KEYB_HOLD_TIME);
          keypad.setDebounceTime(KEYB_DEBOUNCE_TIME);
          // add an event listener 
          keypad.addEventListener(keypad_callback_onEvent); 
          // A second callback just for debugging purpose: it does nothing fsm-wise but just prints the key-events on Serial
          //keypad.addEventListener(DEBUG_keypad_callback_onEvent);
 
          return true; // nothing to setup yet        
      }
    

    
   

//  EOF	

		 
