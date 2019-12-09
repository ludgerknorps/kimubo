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
 * Abschnitt SETUP
 */

	

	void setup() {

    // initialize serial communication at 9600 bits per second:
    Serial.begin(38400);
    Serial.println(F("Hello to the world of KIMUBO!"));
    
    // #################################################################################### 
		// Setup of Keypad/keyboard

		keypad.setHoldTime(KEYB_HOLD_TIME);
		keypad.setDebounceTime(KEYB_DEBOUNCE_TIME);
		// add an event listener 
		keypad.addEventListener(keypad_callback_onEvent); 
    // A second callback just for debugging purpose: it does nothing fsm-wise but just prints the key-events on Serial
    //keypad.addEventListener(DEBUG_keypad_callback_onEvent);

    sdc_setup();
    keyb_setup();
    player.setupPlayer(9, 10);

    player.play("/1/1.WAV");


	} // setup()
  
// EOF 
