


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
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 0. a semi-statemachine for the keys
	
		static bool keybIsMoreThanOneKeyPressed = false; // true, if more than one key is pressed - we don't want multikey right now!
		static char keybPressedKey = NULL; 	// is set to a key that is pressed alone. Used to act only at release of this key, not on pressing it. Also used for acting on hold of key.
		static char keybHeldKey = NULL; 		//used to remember which key was held (to act on its release)
		

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
		extern Keypad keypad; // defined in setup()
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	// 9. the eventListener callback function for different types of keys
	
		void keypadEvent(KeypadEvent key){
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
						break;
						
					case HOLD:
						keybHeldKey = key;
						if ( key ='F' ) {
							// seeking while button is held
							
						} else if ( key ='R') {
							// seeking back while button is held
							
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
							} else {
								// key released after held
								// play playlist nr. <key> from stored position (from EEPROM)
							} 
						} else if (key=='F') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to next track in playlist
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='B') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to previous track in playlist
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='S') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setSleeper()
						} else if (key=='L') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setLoudness()
						} else if (key=='Z') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// tellStatus()
						}
						
						// clean up the flags (remember: only one key allowed at any time!)
						keybPressedKey = NULL;
						keybHeldKey = NULL;
						break;
				} // switch
			} // if
		} // keypadEvent


//  EOF	

		 
