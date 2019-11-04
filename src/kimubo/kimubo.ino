
#include "kimubo.h"
	
	
	
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
		static char keybPressedKey = ''; 	// is set to a key that is pressed alone. Used to act only at release of this key, not on pressing it. Also used for acting on hold of key.
		static char keybHeldKey = ''; 		//used to remember which key was held (to act on its release)
		

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. Definition KeyScanCodes
		static const char KEYSCAN_1      = 	'1'	;	 
		static const char KEYSCAN_2      = 	'2'	;	
		static const char KEYSCAN_3      = 	'3'	;	 
		static const char KEYSCAN_4      = 	'4'	;	 
		static const char KEYSCAN_5      = 	'5'	;	 
		static const char KEYSCAN_6      = 	'6'	;	
		static const char KEYSCAN_7      = 	'7'	;	 
		static const char KEYSCAN_8      = 	'8'	;	 
		static const char KEYSCAN_9      = 	'9'	;	 
		static const char KEYSCAN_FFWD   = 	'F'	;	 // FastForwar / Skip
		static const char KEYSCAN_REW    = 	'R'	;	 // Rewind / Skipback
		static const char KEYSCAN_STAT   = 	'Z'	;	 // Status (output via speech)
		static const char KEYSCAN_SLEEP  = 	'S'	;	 // Sleep-Timer-Set
		static const char KEYSCAN_LOUD   = 	'L'	;	 // Parental switch for Volume-Preset: Loud or whisper
		static const char KEYSCAN_PTT    = 	'A'	;	 // PushToTalk (for walkie talkie)
		static const char KEYSCAN_B      = 	'B'	;	 // reserved for future use
					
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. Aus wievielen Reihen und Spalten ist Matrix des Keyboards aufgebaut?

		// real hardware no. of rows/colums
		static const byte KEYB_ROWS = 4;
		static const byte KEYB_COLS = 4;
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. Which pin has which matrix-keyboard-row/-column?
		
		// pins are Arduino pin nr.	
		static const byte KEYB_PIN_COLUMN1 	= 8;
		static const byte KEYB_PIN_COLUMN2 	= 7;
		static const byte KEYB_PIN_COLUMN3 	= 6;
		static const byte KEYB_PIN_COLUMN4 	= 5;
		static const byte KEYB_PIN_ROW1 	= A3;	 // used as digital pin
		static const byte KEYB_PIN_ROW2 	= A2;    // used as digital pin
		static const byte KEYB_PIN_ROW3 	= A1;    // used as digital pin
		static const byte KEYB_PIN_ROW4 	= A0;    // used as digital pin
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. Which Pins are rows, which are columns 
	
		// für Power Down müssen alle Reihen-Pins auf High gesezt werden (und danach wieder auf Low)
		// dazu müssen wir festlagen, welches die Reihen-Pins und welches die Spalten-Pins sind
		static const byte KEYB_RowPins[KEYB_ROWS] = {	
			KEYB_PIN_ROW1, 
			KEYB_PIN_ROW2,
			KEYB_PIN_ROW3,
			KEYB_PIN_ROW4
		}; //connect to the row pinouts of the keypad	
										
		static const byte KEYB_ColPins[KEYB_COLS] = {
			KEYB_PIN_COLUMN1,
			KEYB_PIN_COLUMN2,
			KEYB_PIN_COLUMN3,
			KEYB_PIN_COLUMN4
		}; //connect to the column pinouts of the keypad

		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. damit µC zwischendurch in Powerdown gehen kann, müssen alle Keyboard-Inputs (=Reihen, s.o.) mit je einer Diode an diesen Interrupt-Pin angeschlossen sein; dieser weckt dann den µC auf)
		static const byte KEYB_PIN_INTERRUPT = 2;
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. Die TastaturBelegung selbst = welche Taste ist wo?
			
		static const char KEYB_keyScans[KEYB_ROWS][KEYB_COLS] = {
			{	KEYSCAN_1,		KEYSCAN_2,		KEYSCAN_3,	  	KEYSCAN_STAT	},
			{	KEYSCAN_4,		KEYSCAN_5,		KEYSCAN_6,	    KEYSCAN_SLEEP	},
			{	KEYSCAN_7,		KEYSCAN_8,		KEYSCAN_9,	    KEYSCAN_LOUD	},
			{	KEYSCAN_FFWD,	KEYSCAN_REW,  	KEYSCAN_PTT,  	KEYSCAN_B		}
		};
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 7. keypad settings
		
		//Set the amount of milliseconds the user will have to hold a button until the HOLD state is triggered. (default = 500)
		#define KEYB_HOLD_TIME 	500 
		//Set the amount of milliseconds the keypad will wait until it accepts a new keypress/keyEvent. This is the "time delay" debounce method.  (default = 10)
		#define KEYB_DEBOUNCE_TIME 	10 	
		  
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 8. the keypad/keyboard itself		
		Keypad keypad; // defined in setup()
	
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
				switch (keypad.getState()){
					case PRESSED:
						if ( (key >= '1' && key <='9') || key <='F' || key <='R' ) {
							keybPressedKey = key;
						}
						break;

					case RELEASED:
						if (key >= '1' && key <='9') {
							if (key == keybPressedKey) {
								keybPressedKey = '';
								if (key != keybHeldKey) {
									// play playlist nr. <key> from beginning
								} else {
									keybHeldKey = '';
									// play playlist nr. <key> from stored position (from EEPROM)
								}
							} else {
								// this should never happen because of keybIsMoreThanOneKeyPressed!
							}
						} else if (key=='F') {
							if (key == keybPressedKey) {
								keybPressedKey = '';
								if (key != keybHeldKey) {
									// skip to next track in playlist
								} else {
									keybHeldKey = '';
									// we were seeking, thus we don't do anything at release
								}
							} else {
								// this should never happen because of keybIsMoreThanOneKeyPressed!
							}
						} else if (key=='B') {
							if (key == keybPressedKey) {
								keybPressedKey = '';
								if (key != keybHeldKey) {
									// skip to previous track in playlist
								} else {
									keybHeldKey = '';
									// we were backseeking, thus we don't do anything at release
								}
							} else {
								// this should never happen because of keybIsMoreThanOneKeyPressed!
							}
						}
						break;

					case HOLD:
						if ( key >= '1' && key <='9') {
							keybHeldKey = key;
						} else if ( key <='F' ) {
							keybHeldKey = key;
							// seeking while button is held
							
						} else if ( key <='R') {
							keybHeldKey = key;
							// seeking back while button is held
							
						}
						break;
				} // switch
			} else {
				keybPressedKey = ''; // if several keys are pressed together, don't do anything, thus forget pressed/held keys
				keybHeldKey = '';
			}
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
			Serial.print(F("smMain ChipsVccCompensationValue from EEPROM: "));
			Serial.println(v_smMain_ChipsVccCompensationValue);
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
	
	void setup() {
	
		// Setup of Keypad/keyboard
		Keypad keypad = Keypad( makeKeymap(KEYB_keyScans), KEYB_RowPins, KEYB_ColPins, KEYB_ROWS, KEYB_COLS );
		keypad.setHoldTime(KEYB_HOLD_TIME);
		keypad.setDebounceTime(KEYB_DEBOUNCE_TIME);
		// add an event listener 
		keypad.addEventListener(keypadEvent_frontKeys); 
	
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
	
	} // loop()	
