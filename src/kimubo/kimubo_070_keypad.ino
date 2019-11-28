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
	
		static bool keybIsMoreThanOneKeyPressed = false; // true, if more than one key is pressed - we don't want multikey right now!
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
	
		void keypad_callback_onEvent( char key){
			// if more than one key is pressed, then
			// don't do anything with pressed keys
			// don't react on released keys
			
			// even better way: redefine LIST_MAX to be 1
			// then only one key can be added to keys-list. :-)
			
			if ( keypad.numKeys() > 1 ) {
				keybPressedKey = NULL; // if several keys are pressed together, don't do anything, thus forget pressed/held keys and clear all keys from keylist.
				keybHeldKey = NULL;
				keypad.clearList();
			} else {

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
						// Handle errors:
						// 1. case: 
							if (key != keybPressedKey) {
								// this must never happen : how should keybPressedKey != key, if only one key is allowed AND one key was released?
								break;
							}
						// 2. case:
							if ( (keybHeldKey != NULL) && (key != keybHeldKey) ) {
								// this must never happen: how should keybHeldKey != key, if only one key is allowed AND one key was released?
								break;
							}
						// if we came to here, then no errors apparent
					
						if (key >= '1' && key <='9') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// play playlist nr. <key> from beginning

                // remember what key/playlist we want to play as event-triggers for statemachine are stateless (have no memory)
                keyb_current_playListKey = key;
                
                // if player is playing we use transition B3 in smMain, else we use transition B1
                smMain.trigger(smMain_event_playOtherPlaylist); // B3-case
                smMain.trigger(smMain_event_playWav); // B1-case
            } else {
								// key released after held
								// play playlist nr. <key> from stored position (from EEPROM)
                // TBD for later release!
							} 
						} else if (key=='F') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to next track in playlist
                smMain.trigger(smMain_event_skip); // E1 transition in smMain
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='B') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to previous track in playlist
                smMain.trigger(smMain_event_skipBack); // E2 transition in smMain
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='S') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setSleeper()
              // TBD later release
						} else if (key=='L') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setLoudness()
              // TBD later release
						} else if (key=='Z') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// tellStatus()
              // TBD later release
						}
						
						// clean up the flags (remember: only one key allowed at any time!)
						keybPressedKey = NULL;
						keybHeldKey = NULL;
						break;
				} // switch
			} // if
		} // keypadEvent

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    // 10. the keypad setup function
      bool keyb_setup(){
          return true; // nothing to setup yet        
      }


//  EOF	

		 
