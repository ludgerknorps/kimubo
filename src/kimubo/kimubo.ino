#include <BlockDriver.h>
#include <sdios.h>
#include <FreeStack.h>
#include <SdFat.h>
#include <SysCall.h>
#include <SdFatConfig.h>
#include <MinimumSerial.h>


#include "kimubo.h"

//adapted version of libraries!
#include "local.lib/Keypad/src/Keypad.h"
#include "local.lib/Keypad/src/Keypad.cpp"
#include "local.lib/Keypad/src/Key.h"
#include "local.lib/Keypad/src/Key.cpp"

// wir verwenden das Arduino Pro Mini interne EEPROM zum speichern von Einstellungen
#include <EEPROM.h>



	
	
	
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
 * Abschnitt PowerManagement
 */
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Read 1.1V reference against AVcc
	// taken from http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
	// 
	// blocking does not matter to us because we measure between tracks -->  some ms don't matter at all :-)
	
		/* some globals */
		
		long gv_UBat_ChipsVccCompensationValue;
		long gv_UBat_in_millivolt;
		
		/* =========================================================== */
		// called once at device startup
		void setupReadVcc(){
			// STEP 0: Read gv_smUBat_ChipsVccCompensationValue from EEPROM (see below for explanation on that value!)
			EEPROM.get(EEPROM_VCCCOMPENSATION_ADDR, gv_UBat_ChipsVccCompensationValue); // this is a long, i.e. 4 bytes!
			Serial.print(F("gv_UBat_ChipsVccCompensationValue from EEPROM: "));
			Serial.println(gv_UBat_ChipsVccCompensationValue);
			if ( gv_UBat_ChipsVccCompensationValue == -1 ) {
				// nothing was stored in EEPROM --> default to assumption that Vcc is exactely 5.00V
				// see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/ for details.
				Serial.println(F("WARNING no ChipsVccCompensationValue found in EEPROM, defaulting to assumption that Vcc is exactely 5.00V"));
				gv_UBat_ChipsVccCompensationValue = 1125300L; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
			}
		} // setupReadVcc()
		
		/* =========================================================== */
		// called by smMain at device startup + before each track + after last track + after waking up from sleep
		long readVcc() {
			// STEP 1: save current ADMUX settings to temporary variable
			byte uBat_OldADMUX = ADMUX;
			// STEP 2: set the reference to Vcc and the measurement to the internal 1.1V reference
			// ATMEGA328P, others see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
			ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
			delay(2); // blocking wait, does not matter as we measure between tracks --> 2ms don't matter at all :-)
			// STEP 3: start conversion
			ADCSRA |= _BV(ADSC);
			while (bit_is_set(ADCSRA,ADSC)); // bloking measuring
			//  bit is reset --> conversion finished
			// STEP 5: calculate Vcc
			/* 		we know the measured value is     Vcc*(ADC-measure)/1023
			 * 		AND we know that we can expect ADC-measure to be 1.1V = 1100mV
			 * 		Solve that for Vcc to get         Vcc = 1100 * 1023 / ADC-measure
			 * 		Now, ADC measure is NOT REALLY 1100mV on each individual arduino chip
			 * 		therefore, we have to use a compensation value taken from EEPROM (which must be first stored there beforehand!)
			 * 		gv_smUBat_ChipsVccCompensationValue: this one is individual for each chip!
			 * 		(if you don't have an individually measured compensationvalue, then take the generik gv_smUBat_ChipsVccCompensationValue = 1100 * 1023 = 1125300L)
			 */
			byte low  = ADCL; // must read ADCL first - it then locks ADCH  
			byte high = ADCH; // unlocks both
			gv_UBat_in_millivolt = (high<<8) | low; // not yet compensated Vcc (in mV)!
			gv_UBat_in_millivolt = gv_UBat_ChipsVccCompensationValue / gv_UBat_in_millivolt; // now its compensated!
			// STEP 6: clean up
			ADMUX = uBat_OldADMUX;	
		} // readVcc()




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
 * Abschnitt SETUP
 */

  Keypad keypad = Keypad( makeKeymap(KEYB_keyScans), KEYB_RowPins, KEYB_ColPins, KEYB_ROWS, KEYB_COLS );
  
  void setup() {
  
    // Setup of Keypad/keyboard
    
    keypad.setHoldTime(KEYB_HOLD_TIME);
    keypad.setDebounceTime(KEYB_DEBOUNCE_TIME);
    // add an event listener 
    keypad.addEventListener(keypadEvent); 
  
  } // setup()


 

	
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
		keypad.getKeys(); // we only need to check for one key as we do not use multikey - but the whole eventListener-thing in keypad works with "getKeys()".
		// tbd. define MAX_KEYS 1!
	
	} // loop()	
